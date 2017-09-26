////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackServerCommunicationCenter.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SmallPackServerCommunicationCenter.h"

#include "../SmallPackPacker.h"
#include <iostream>

using namespace boost::asio::ip;

SmallPack::Server::SmallPackServerCommunicationCenter::SmallPackServerCommunicationCenter(boost::asio::io_service& _ioService) : SmallPack::SmallPackCommunicationCenter(_ioService)
{
	// Set the initial data
	// ...
}

SmallPack::Server::SmallPackServerCommunicationCenter::~SmallPackServerCommunicationCenter()
{
}

bool SmallPack::Server::SmallPackServerCommunicationCenter::Initialize(uint16_t _selfPort)
{
	// Set the current port
	m_ControllerData.currentPort = _selfPort;

	// Prepare our socket
	m_ControllerData.connectionSocket = udp::socket(m_ControllerData.ioService, udp::endpoint(udp::v4(), m_ControllerData.currentPort));

	// Log
	std::cout << "Listenning on port: " << _selfPort << std::endl;

	return true;
}

std::vector<SmallPack::NetworkMessage> SmallPack::Server::SmallPackServerCommunicationCenter::Update(SmallPackMessagePackList* _messagePackList, SmallPackPacker* _packer, uint32_t _totalTime, float _elapsedTime)
{
	// Our network message vector
	std::vector<NetworkMessage> messageVector;

	// While there are new message packs
	udp::endpoint senderEndpoint;
	SmallPack::MessagePack* newPack = nullptr;
	while ((newPack = CheckForNewMessages(_messagePackList, senderEndpoint)) != nullptr)
	{
		// Get all messages from this pack
		GetMessagesFromPack(_packer, newPack, senderEndpoint, messageVector, true);
	}

	// Check for system messages
	CheckForSystemMessages(_packer, messageVector, _totalTime, _elapsedTime);

	// For each client communication channel
	for (auto & clientCommunicationChannel : m_ClientConnections)
	{
		clientCommunicationChannel->FrameUpdate(_totalTime, _elapsedTime);
	}

	// Return the message vector
	return messageVector;
}

void  SmallPack::Server::SmallPackServerCommunicationCenter::ProcessPingMessage(SmallPackPacker* _packer, NetworkMessage* _message, PingCommandType _type, uint32_t _totalTime)
{
	// Get the sender communication channel
	Server::SmallPackCommunicationChannel* communicationChannel = GetSenderCommunicationChannel(_message->senderInfo.address, _message->senderInfo.port);
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

void SmallPack::Server::SmallPackServerCommunicationCenter::CommitMessages(SmallPackPacker* _packer, SmallPackMessageComposer* _composer, uint32_t _totalTime)
{
	// For each client communication channel
	for (auto & clientCommunicationChannel : m_ClientConnections)
	{
		// Commit all queued messages for this channel
		clientCommunicationChannel->CommitQueueMessage(_packer, _composer, _totalTime);
	}
}

bool SmallPack::Server::SmallPackServerCommunicationCenter::CommunicationChannelExists(boost::asio::ip::address _senderAddress, uint32_t _port, bool _createIfNeed)
{
	// Get the sender communication channel
	Server::SmallPackCommunicationChannel* communicationChannel = GetSenderCommunicationChannel(_senderAddress, _port, _createIfNeed);
	if (communicationChannel == nullptr)
	{
		return false;
	}

	return true;
}

void SmallPack::Server::SmallPackServerCommunicationCenter::SendSystemMessageToCommunicationChannel(SmallPackPacker* _packer, NetworkMessage* _systemMessage, uint32_t _totalTime, float _elapsedTime)
{
	// Get the sender communication channel
	Server::SmallPackCommunicationChannel* communicationChannel = GetSenderCommunicationChannel(_systemMessage->senderInfo.address, _systemMessage->senderInfo.port);
	if (communicationChannel == nullptr)
	{
		// Ignore this message, invalid sender
		return;
	}

	// Process this message
	communicationChannel->ProcessSystemMessage(_packer, _systemMessage, _totalTime);
}

SmallPack::Server::SmallPackServerCommunicationChannel* SmallPack::Server::SmallPackServerCommunicationCenter::GetSenderCommunicationChannel(boost::asio::ip::address _senderAddress, uint32_t _port, bool _createIfNeed)
{
	// For each communication channel
	for (int i = 0; i < m_ClientConnections.size(); i++)
	{
		// Get the current communication channel
		SmallPackServerCommunicationChannel* currentCommunicationChannel = m_ClientConnections[i];

		// Check if the sender is the real owner
		if (currentCommunicationChannel->IsHost(_senderAddress, _port))
		{
			return currentCommunicationChannel;
		}
	}

	// We didnt found a valid communication channel, check if we should create one
	if (_createIfNeed)
	{
		// New client connected alert
		std::cout << "New client connected" << std::endl;

		// Create a new communication channel for this sender
		SmallPackServerCommunicationChannel* newCommunicationChannel = new SmallPackServerCommunicationChannel(m_ControllerData.ioService);

		// Initialize the new communication channel
		bool result = newCommunicationChannel->Initialize(_senderAddress, _port);
		if (!result)
		{
			delete newCommunicationChannel;
			return nullptr;
		}

		// Insert into our vector the new created communication channel
		m_ClientConnections.push_back(newCommunicationChannel);

		// Log
		std::cout << "Creating new connection with address: " << _senderAddress.to_string() << " at port: " << _port << std::endl;

		return newCommunicationChannel;
	}

	return nullptr;
}