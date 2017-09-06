////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackCommunicationCenter.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SmallPackCommunicationCenter.h"

#include "SmallPackPacker.h"

using namespace boost::asio::ip;

SmallPack::SmallPackCommunicationCenter::SmallPackCommunicationCenter(boost::asio::io_service& _ioService) : m_ControllerData(_ioService)
{
	// Set the initial data
	// ...
}

SmallPack::SmallPackCommunicationCenter::~SmallPackCommunicationCenter()
{
}

bool SmallPack::SmallPackCommunicationCenter::Initialize(uint16_t _port)
{
	// Set the current port
	m_ControllerData.currentPort = _port;

	// Prepare our socket
	m_ControllerData.connectionSocket = udp::socket(m_ControllerData.ioService, udp::endpoint(udp::v4(), m_ControllerData.currentPort));

	return true;
}

void SmallPack::SmallPackCommunicationCenter::Update(SmallPackMessagePackList* _messagePackList, SmallPackPacker* _packer, float _elapsedTime)
{
	// Our network message vector
	std::vector<NetworkMessage> messageVector;

	// While there are new message packs
	udp::endpoint senderEndpoint;
	SmallPack::MessagePack* newPack = nullptr;
	while ((newPack = CheckForNewMessages(_messagePackList, senderEndpoint)) != nullptr)
	{
		// Check if the sender is connected to any of our communication channels
		// ... TODO

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
	}

	// For each received message
	for (int i=0; i<messageVector.size(); i++)
	{
		// Get the current message
		NetworkMessage* currentMessage = &messageVector[i];

		// Check for system messages
		if (currentMessage->messageHeader.messageOperator == SmallPack::Operator::System)
		{
			// Process this message
			// ...

			// Remove this message from the vector
			messageVector.erase(messageVector.begin() + i);
			i--;
		}
	}
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

bool SmallPack::SmallPackCommunicationCenter::BroadcastMessageToAllClients(NetworkMessage& _message)
{
	return true;
}