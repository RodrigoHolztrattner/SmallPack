////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackClientCommunicationCenter.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SmallPackClientCommunicationCenter.h"
#include <iostream>

#include "..\SmallPackPacker.h"

using namespace boost::asio::ip;

SmallPack::Client::SmallPackClientCommunicationCenter::SmallPackClientCommunicationCenter(boost::asio::io_service& _ioService) : SmallPack::SmallPackCommunicationCenter(_ioService), m_ServerConnection(_ioService)
{
	// Set the initial data
	// ...
}

SmallPack::Client::SmallPackClientCommunicationCenter::~SmallPackClientCommunicationCenter()
{
}

bool SmallPack::Client::SmallPackClientCommunicationCenter::Initialize(const char* _serverAddress, const char* _serverPort, uint16_t _selfPort)
{
	// Set the current port
	m_ControllerData.currentPort = _selfPort;

	// Prepare our socket
	m_ControllerData.connectionSocket = udp::socket(m_ControllerData.ioService, udp::endpoint(udp::v4(), m_ControllerData.currentPort));

	// Log
	std::cout << "Listenning on port: " << _selfPort << " and connected to server: " << _serverAddress << " at port: " << _serverPort << std::endl;

	// Initialize the server connection
	bool result = m_ServerConnection.Initialize(_serverAddress, _serverPort);
	if (!result)
	{
		return false;
	}

	// Set the server answer port
	m_ServerConnection.SetAnswerPort(atoi(_serverPort)); problema é que ele ta recebendo a mensagem do server por uma certa porta 
		e ele compara com 22222

	return true;
}

std::vector<SmallPack::NetworkMessage> SmallPack::Client::SmallPackClientCommunicationCenter::Update(SmallPackMessagePackList* _messagePackList, SmallPackPacker* _packer, uint32_t _totalTime, float _elapsedTime)
{
	// Our network message vector
	std::vector<NetworkMessage> messageVector;

	// While there are new message packs
	udp::endpoint senderEndpoint;
	SmallPack::MessagePack* newPack = nullptr;
	while ((newPack = CheckForNewMessages(_messagePackList, senderEndpoint)) != nullptr)
	{
		// Get all messages from this pack
		GetMessagesFromPack(_packer, newPack, senderEndpoint, messageVector, false);
	}

	// Check for system messages
	CheckForSystemMessages(_packer, messageVector, _totalTime, _elapsedTime);

	// Do the frame update for the server communication channel
	m_ServerConnection.FrameUpdate(_totalTime, _elapsedTime);

	// For each client communication channel
	for (auto & clientCommunicationChannel : m_ClientConnections)
	{
		// Do the frame update for this channel
		clientCommunicationChannel->FrameUpdate(_totalTime, _elapsedTime);
	}

	// Return the message vector
	return messageVector;
}

void SmallPack::Client::SmallPackClientCommunicationCenter::ProcessPingMessage(SmallPackPacker* _packer, NetworkMessage* _message, PingCommandType _type, uint32_t _totalTime)
{
	// Get the sender communication channel
	Client::SmallPackCommunicationChannel* communicationChannel = GetSenderCommunicationChannel(_message->senderInfo.address, _message->senderInfo.port);
	if (communicationChannel == nullptr)
	{
		// Ignore this message, invalid sender
		return;
	}

	// Check the ping type
	if (_type == PingCommandType::Answer)
	{
		// Process the ping answer for this channel
		communicationChannel->ProcessPingAnswer(_packer, _message, _totalTime);
	}
	else if (_type == PingCommandType::Request)
	{
		// Request a ping message from this channel
		communicationChannel->RequestPing();
	}
}

void SmallPack::Client::SmallPackClientCommunicationCenter::CommitMessages(SmallPackPacker* _packer, SmallPackMessageComposer* _composer, uint32_t _totalTime)
{
	// Commit all queued messages for the server
	m_ServerConnection.CommitQueueMessage(_packer, _composer, m_ControllerData.currentPort, 1, _totalTime);

	// For each client communication channel
	for (auto & clientCommunicationChannel : m_ClientConnections)
	{
		// Commit all queued messages for this channel
		clientCommunicationChannel->CommitQueueMessage(_packer, _composer, m_ControllerData.currentPort, 1, _totalTime);
	}
}

bool SmallPack::Client::SmallPackClientCommunicationCenter::CommunicationChannelExists(boost::asio::ip::address _senderAddress, uint32_t _port, uint32_t _answerPort, bool _createIfNeed)
{
	// Get the sender communication channel
	Client::SmallPackCommunicationChannel* communicationChannel = GetSenderCommunicationChannel(_senderAddress, _port, _answerPort, _createIfNeed);
	if (communicationChannel == nullptr)
	{
		return false;
	}

	return true;
}

void SmallPack::Client::SmallPackClientCommunicationCenter::SendSystemMessageToCommunicationChannel(SmallPackPacker* _packer, NetworkMessage* _systemMessage, uint32_t _totalTime, float _elapsedTime)
{
	// Get the sender communication channel
	Client::SmallPackCommunicationChannel* communicationChannel = GetSenderCommunicationChannel(_systemMessage->senderInfo.address, _systemMessage->senderInfo.port);
	if (communicationChannel == nullptr)
	{
		// Ignore this message, invalid sender
		return;
	}

	// Process this message
	communicationChannel->ProcessSystemMessage(_packer, _systemMessage, _totalTime);
}

bool SmallPack::Client::SmallPackClientCommunicationCenter::BroadcastMessageToAllClients(NetworkMessage& _message)
{
	return true;
}

SmallPack::Client::SmallPackCommunicationChannel* SmallPack::Client::SmallPackClientCommunicationCenter::GetSenderCommunicationChannel(boost::asio::ip::address _senderAddress, uint32_t _port, uint32_t _answerPort, bool _createIfNeed)
{
	// Check if the sender is the server itself
	if (m_ServerConnection.IsHost(_senderAddress, _port))
	{
		return &m_ServerConnection;
	}

	// For each communication channel
	for (int i = 0; i < m_ClientConnections.size(); i++)
	{
		// Get the current communication channel
		Client::SmallPackCommunicationChannel* currentCommunicationChannel = m_ClientConnections[i];

		// Check if the sender is the real owner
		if (currentCommunicationChannel->IsHost(_senderAddress, _port))
		{
			return currentCommunicationChannel;
		}
	}

	// We didnt found a valid communication channel, check if we should create one
	if (_createIfNeed)
	{
		// Create a new communication channel for this sender
		Client::SmallPackCommunicationChannel* newCommunicationChannel = new Client::SmallPackCommunicationChannelNonReliable(m_ControllerData.ioService);

		// Initialize the new communication channel
		bool result = newCommunicationChannel->Initialize(_senderAddress.to_string().c_str(), std::to_string(_port).c_str()); //TODO enviar o address porque assim ele não copia direito
		if (!result)
		{
			delete newCommunicationChannel;
			return nullptr;
		}

		// Insert into our vector the new created communication channel
		m_ClientConnections.push_back(newCommunicationChannel);

		// Set the answer port
		newCommunicationChannel->SetAnswerPort(_answerPort);

		// Log
		std::cout << "Creating new connection with address: " << _senderAddress.to_string() << " at port: " << _port << std::endl;

		return newCommunicationChannel;
	}

	return nullptr;
}