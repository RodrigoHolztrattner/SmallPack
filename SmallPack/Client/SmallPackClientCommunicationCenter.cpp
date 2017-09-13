////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackClientCommunicationCenter.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SmallPackClientCommunicationCenter.h"

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

	// Initialize the server connection
	bool result = m_ServerConnection.Initialize(_serverAddress, _serverPort);
	if (!result)
	{
		return false;
	}

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
		GetMessagesFromPack(_packer, newPack, messageVector, false);
	}

	// Check for system messages
	CheckForSystemMessages(_packer, messageVector, _totalTime, _elapsedTime);

	// Do the frame update for the server communication channel
	m_ServerConnection.FrameUpdate(_totalTime, _elapsedTime);

	// For each client communication channel
	for (auto & clientCommunicationChannel : m_ClientConnections)
	{
		clientCommunicationChannel->FrameUpdate(_totalTime, _elapsedTime);
	}

	// Return the message vector
	return messageVector;
}

bool SmallPack::Client::SmallPackClientCommunicationCenter::CommunicationChannelExists(boost::asio::ip::address _senderAddress, uint32_t _port, bool _createIfNeed)
{
	// Get the sender communication channel
	Client::SmallPackCommunicationChannel* communicationChannel = GetSenderCommunicationChannel(_senderAddress, _port, _createIfNeed);
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

SmallPack::Client::SmallPackCommunicationChannel* SmallPack::Client::SmallPackClientCommunicationCenter::GetSenderCommunicationChannel(boost::asio::ip::address _senderAddress, uint32_t _port, bool _createIfNeed)
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

		return newCommunicationChannel;
	}

	return nullptr;
}