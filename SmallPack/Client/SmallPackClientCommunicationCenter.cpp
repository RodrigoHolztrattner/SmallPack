////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackClientCommunicationCenter.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SmallPackClientCommunicationCenter.h"
#include <iostream>

#include "..\SmallPackPacker.h"
#include "..\SmallPackMessageComposer.h"

using namespace boost::asio::ip;

SmallPack::Client::SmallPackClientCommunicationCenter::SmallPackClientCommunicationCenter(boost::asio::io_service& _ioService) : SmallPack::SmallPackCommunicationCenter(_ioService), m_ServerConnection(_ioService, false)
{
	// Set the initial data
	// ...
}

SmallPack::Client::SmallPackClientCommunicationCenter::~SmallPackClientCommunicationCenter()
{
}

bool SmallPack::Client::SmallPackClientCommunicationCenter::Initialize(const char* _serverAddress, const char* _serverPort, uint16_t _selfPort, uint32_t _authenticationToken)
{
	// Set the current port
	m_ControllerData.currentPort = _selfPort;

	// Prepare our socket
	m_ControllerData.connectionSocket = udp::socket(m_ControllerData.ioService, udp::endpoint(udp::v4(), m_ControllerData.currentPort));

	// Log
	std::cout << "Listenning on port: " << _selfPort << " and connected to server: " << _serverAddress << " at port: " << _serverPort << std::endl;

	// Set our authentication token
	m_ControllerData.authenticationToken = _authenticationToken;

	// Initialize the server connection
	bool result = m_ServerConnection.Initialize(_serverAddress, _serverPort, m_ControllerData.currentPort, m_ControllerData.authenticationToken);
	if (!result)
	{
		return false;
	}


	return true;
}

std::vector<SmallPack::NetworkMessage> SmallPack::Client::SmallPackClientCommunicationCenter::Update(SmallPackMessagePackList* _messagePackList, SmallPackPacker* _packer, uint32_t _totalTime, float _elapsedTime)
{
	// Our network message vector
	std::vector<NetworkMessage> messageVector = SmallPack::SmallPackCommunicationCenter::Update(_messagePackList, _packer, _totalTime, _elapsedTime);

	// Do the frame update for the server communication channel
	m_ServerConnection.FrameUpdate(_totalTime, _elapsedTime);

	// Return the message vector
	return messageVector;
}

void SmallPack::Client::SmallPackClientCommunicationCenter::CommitMessages(SmallPackPacker* _packer, uint32_t _totalTime)
{
	// Commit all queued messages for the server
	m_ServerConnection.CommitQueueMessage(_packer, _totalTime);

	// Call the parent method
	SmallPack::SmallPackCommunicationCenter::CommitMessages(_packer, _totalTime);
}

void SmallPack::Client::SmallPackClientCommunicationCenter::ProcessClientConnectMessage(SmallPackPacker* _packer, NetworkMessage* _message)
{
	// Get the confirmation data
	std::vector<CommandClientConnectionInfo> connectionInfoData;
	if (SmallPackMessageComposer::GetDataObject(_message, connectionInfoData))
	{
		// For each client we should connect
		for (auto & clientInfo : connectionInfoData)
		{
			// Get the communication channel for this client (create if there is no one)
			SmallPackCommunicationChannel* clientChannel = GetSenderCommunicationChannel(boost::asio::ip::address::from_string(clientInfo.ip), clientInfo.port, true);
		
			// Reset the expire timer for this channel
			clientChannel->ResetExpireTimer();
		}
	}

	// Call the parent function
	SmallPack::SmallPackCommunicationCenter::ProcessClientConnectMessage(_packer, _message);
}

bool SmallPack::Client::SmallPackClientCommunicationCenter::BroadcastMessageToAllClients(SmallPackPacker* _packer, NetworkMessage& _message, uint32_t _totalTime)
{
	// For each client, send the message
	for (auto & client : m_ClientConnections)
	{
		// Queue the message and commit it
		client->QueueMessage(&_message);
		client->CommitQueueMessage(_packer, _totalTime);
	}

	return true;
}

SmallPack::SmallPackCommunicationChannel* SmallPack::Client::SmallPackClientCommunicationCenter::GetSenderCommunicationChannel(boost::asio::ip::address _senderAddress, uint32_t _port, bool _createIfNeed)
{
	// Check if the sender is the server itself
	if (m_ServerConnection.IsHost(_senderAddress, _port))
	{
		return &m_ServerConnection;
	}

	return SmallPack::SmallPackCommunicationCenter::GetSenderCommunicationChannel(_senderAddress, _port, _createIfNeed);
}

SmallPack::SmallPackCommunicationChannel* SmallPack::Client::SmallPackClientCommunicationCenter::CreateCommunicationChannel(boost::asio::ip::address _senderAddress, uint32_t _port)
{
	return new SmallPackCommunicationChannelNonReliable(m_ControllerData.ioService, true);
}