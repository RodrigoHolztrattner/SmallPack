////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackCommunicationCenter.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SmallPackCommunicationCenter.h"
#include "SmallPackPacker.h"

#include <iostream>

using namespace boost::asio::ip;

SmallPack::SmallPackCommunicationCenter::SmallPackCommunicationCenter(boost::asio::io_service& _ioService) : m_ControllerData(_ioService)
{
	// Set the initial data
	// ...
}

SmallPack::SmallPackCommunicationCenter::~SmallPackCommunicationCenter()
{
}

std::vector<SmallPack::NetworkMessage> SmallPack::SmallPackCommunicationCenter::Update(SmallPackMessagePackList* _messagePackList, SmallPackPacker* _packer, uint32_t _totalTime, float _elapsedTime)
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

	// Check for internal messages
	CheckForInternalMessages(_packer, messageVector, _totalTime, _elapsedTime);

	// For each client communication channel
	for (int i=0; i<m_ClientConnections.size(); i++)
	{
		// Get the client channel
		SmallPackCommunicationChannel* clientCommunicationChannel = m_ClientConnections[i];

		// Do the frame update for this channel
		clientCommunicationChannel->FrameUpdate(_totalTime, _elapsedTime);

		// Check if we should delete this channel
		if (clientCommunicationChannel->ShouldDelete(_totalTime, _elapsedTime))
		{
			// Log
			std::cout << " - Deleting communication channel" << std::endl;

			// Delete the channel
			delete m_ClientConnections[i];

			// Remove the channel from the vector
			m_ClientConnections.erase(m_ClientConnections.begin() + i);

			// Go back 1 from the current index
			i--;
		}
	}

	// Return the message vector
	return messageVector;
}

void SmallPack::SmallPackCommunicationCenter::GetMessagesFromPack(SmallPackPacker* _packer, SmallPack::MessagePack* _messagePack, udp::endpoint& _senderEndpoint, std::vector<NetworkMessage>& _messageVector, bool _createNewCommunicationChannels)
{
	// Grab all messages inside this pack
	std::vector<NetworkMessage> messages = _packer->UnpackMessagePack(_messagePack);

	// Check if we have at last one message
	if (!messages.size())
	{
		// Ignore this message pack!
		return;
	}

	// Get the answer port from the first message
	uint32_t answerPort = messages[0].messageHeader.answerPort;
	
	// Get the sender communication channel and check if he is connected to any of our communication channels
	SmallPackCommunicationChannel* communicationChannel = GetSenderCommunicationChannel(_senderEndpoint.address(), answerPort, _createNewCommunicationChannels);
	if (communicationChannel == nullptr)
	{
		// Ignore this message pack, invalid sender!
		return;
	}

	// Set the message sender info for each message
	for (auto & message : messages)
	{
		// Set both the address and the port
		message.senderInfo.address = _senderEndpoint.address();
		message.senderInfo.port = answerPort; //  _senderEndpoint.port();
	}

	// Append the messages
	_messageVector.insert(std::end(_messageVector), std::begin(messages), std::end(messages));

	// Release the message pack
	_packer->ReleaseMessagePack(_messagePack);
}

void SmallPack::SmallPackCommunicationCenter::ProcessPingMessage(SmallPackPacker* _packer, NetworkMessage* _message, CommandFlags _flags, uint32_t _totalTime)
{
	// Get the sender communication channel
	SmallPackCommunicationChannel* communicationChannel = GetSenderCommunicationChannel(_message->senderInfo.address, _message->senderInfo.port, false);
	if (communicationChannel == nullptr)
	{
		// Ignore this message, invalid sender
		return;
	}

	// Check the ping type
	if (_flags == CommandFlags::PingAnswer)
	{
		// Process the ping answer for this channel
		communicationChannel->ProcessPingAnswer(_packer, _message, _totalTime);
	}
	else if (_flags == CommandFlags::PingRequest)
	{
		// Request a ping message from this channel
		communicationChannel->RequestPing();
	}
}

void SmallPack::SmallPackCommunicationCenter::CommitMessages(SmallPackPacker* _packer, uint32_t _totalTime)
{
	// For each client communication channel
	for (auto & clientCommunicationChannel : m_ClientConnections)
	{
		// Commit all queued messages for this channel
		clientCommunicationChannel->CommitQueueMessage(_packer, _totalTime);
	}
}

void SmallPack::SmallPackCommunicationCenter::CheckForInternalMessages(SmallPackPacker* _packer, std::vector<NetworkMessage>& _messageVector, uint32_t _totalTime, float _elapsedTime)
{
	// For each received message
	for (int i = 0; i<_messageVector.size(); i++)
	{
		// Get the current message
		NetworkMessage* currentMessage = &_messageVector[i];

		// Get the sender communication channel
		SmallPackCommunicationChannel* communicationChannel = GetSenderCommunicationChannel(currentMessage->senderInfo.address, currentMessage->senderInfo.port, false);
		if (communicationChannel == nullptr)
		{
			// Ignore this message, invalid sender
			continue;
		}

		// Check the ping request
		if (CheckFlag(currentMessage->messageHeader.messageFlags, CommandFlags::PingRequest))
		{
			// Process this ping message
			ProcessPingMessage(_packer, currentMessage, CommandFlags::PingRequest, _totalTime);
		}

		// Check the ping answer
		if (CheckFlag(currentMessage->messageHeader.messageFlags, CommandFlags::PingAnswer))
		{
			// Process this ping message
			ProcessPingMessage(_packer, currentMessage, CommandFlags::PingAnswer, _totalTime);
		}

		// Check for system messages
		if (currentMessage->messageHeader.messageOperator == SmallPack::Operator::System)
		{
			// Check for the client connect info
			if (currentMessage->messageHeader.messageCommand == SystemCommands::ClientConnectInfo)
			{
				// Process the client connect message
				ProcessClientConnectMessage(_packer, currentMessage);
			}
			// No results, just propagate the message
			else
			{
				// Propagate the message to the channel
				communicationChannel->ProcessChannelInternalMessage(_packer, currentMessage, _totalTime);
			}


			// Remove this message from the vector
			_messageVector.erase(_messageVector.begin() + i);
			i--;
		}
	}
}

void SmallPack::SmallPackCommunicationCenter::ProcessClientConnectMessage(SmallPackPacker* _packer, NetworkMessage* _message)
{
	// Do nothing
}

SmallPack::MessagePack* SmallPack::SmallPackCommunicationCenter::CheckForNewMessages(SmallPackMessagePackList* _messagePackList, udp::endpoint& _endpoint)
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

SmallPack::SmallPackCommunicationChannel* SmallPack::SmallPackCommunicationCenter::GetSenderCommunicationChannel(boost::asio::ip::address _senderAddress, uint32_t _port, bool _createIfNeed)
{
	// For each communication channel
	for (int i = 0; i < m_ClientConnections.size(); i++)
	{
		// Get the current communication channel
		SmallPackCommunicationChannel* currentCommunicationChannel = m_ClientConnections[i];

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
		SmallPackCommunicationChannel* newCommunicationChannel = CreateCommunicationChannel(_senderAddress, _port); //  SmallPackCommunicationChannel

		// Initialize the new communication channel
		bool result = newCommunicationChannel->Initialize(_senderAddress.to_string().c_str(), std::to_string(_port).c_str(), m_ControllerData.currentPort, m_ControllerData.authenticationToken);
		if (!result)
		{
			delete newCommunicationChannel;
			return nullptr;
		}

		// Insert into our vector the new created communication channel
		m_ClientConnections.push_back(newCommunicationChannel);

		// Log
		std::cout << " - Creating new connection with address: " << _senderAddress.to_string() << " at port: " << _port << std::endl;

		return newCommunicationChannel;
	}

	return nullptr;
}