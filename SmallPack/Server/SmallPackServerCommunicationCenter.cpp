////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackServerCommunicationCenter.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SmallPackServerCommunicationCenter.h"

#include "../SmallPackPacker.h"
#include <iostream>

using namespace boost::asio::ip;

SmallPack::Server::SmallPackServerCommunicationCenter::SmallPackServerCommunicationCenter(boost::asio::io_service& _ioService) : m_ControllerData(_ioService)
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

	return true;
}

void SmallPack::Server::SmallPackServerCommunicationCenter::Update(SmallPackMessagePackList* _messagePackList, SmallPackPacker* _packer, uint32_t _totalTime, float _elapsedTime)
{
	// Our network message vector
	std::vector<NetworkMessage> messageVector;

	// While there are new message packs
	udp::endpoint senderEndpoint;
	SmallPack::MessagePack* newPack = nullptr;
	while ((newPack = CheckForNewMessages(_messagePackList, senderEndpoint)) != nullptr)
	{
		// Check if the sender is connected to any of our communication channels
		SmallPackServerCommunicationChannel* communicationChannel = GetSenderCommunicationChannel(senderEndpoint.address(), senderEndpoint.port(), true);
		if (communicationChannel == nullptr)
		{
			// Ignore this message pack, invalid sender
			continue;
		}

		// Grab all messages inside this pack
		std::vector<NetworkMessage> messages = _packer->UnpackMessagePack(newPack);
		
		// Set the message sender info for each message
		for (auto & message : messages)
		{
			// Set both the address and the port
			message.senderInfo.address = senderEndpoint.address();
			message.senderInfo.port = senderEndpoint.port();
		}

		// Append the messages
		messageVector.insert(std::end(messageVector), std::begin(messages), std::end(messages));

		// Release the message pack
		_packer->ReleaseMessagePack(newPack);
	}

	// For each received message
	for (int i=0; i<messageVector.size(); i++)
	{
		// Get the current message
		NetworkMessage* currentMessage = &messageVector[i];

		// Check for system messages
		if (currentMessage->messageHeader.messageOperator == SmallPack::Operator::System)
		{
			// Get the sender communication channel
			SmallPackServerCommunicationChannel* communicationChannel = GetSenderCommunicationChannel(currentMessage->senderInfo.address, currentMessage->senderInfo.port);
			if (communicationChannel == nullptr)
			{
				// Ignore this message, invalid sender
				messageVector.erase(messageVector.begin() + i);
				i--;
				continue;
			}

			// Process this message
			communicationChannel->ProcessSystemMessage(_packer, currentMessage, _totalTime);

			// Remove this message from the vector
			messageVector.erase(messageVector.begin() + i);
			i--;
		}
	}
}

SmallPack::MessagePack* SmallPack::Server::SmallPackServerCommunicationCenter::CheckForNewMessages(SmallPackMessagePackList* _messagePackList, udp::endpoint& _endpoint)
{
	// Check if we have any messages avaliable
	if (!m_ControllerData.connectionSocket.available())
	{
		return nullptr;
	}

	// Request a new message pack
	MessagePack* newMessagePack = _messagePackList->RequestPack();

	// Receive the message pack
	{
		const uint32_t bufferSize = 2048;
		unsigned char data[bufferSize];
		size_t length = m_ControllerData.connectionSocket.receive_from(boost::asio::buffer(data, bufferSize), _endpoint);

		// Copy into the message pack
		newMessagePack->CopyFromByteStream(data, length);
	}

	return newMessagePack;
}

bool SmallPack::Server::SmallPackServerCommunicationCenter::BroadcastMessageToAllClients(NetworkMessage& _message)
{
	return true;
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
		bool result = newCommunicationChannel->Initialize(_senderAddress.to_string().c_str(), std::to_string(_port).c_str());
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