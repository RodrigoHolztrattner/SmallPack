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

	// Set our authentication token
	m_ControllerData.authenticationToken = 1;

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

bool SmallPack::Client::SmallPackClientCommunicationCenter::BroadcastMessageToAllClients(NetworkMessage& _message)
{
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
	return new SmallPackCommunicationChannelNonReliable(m_ControllerData.ioService);
}