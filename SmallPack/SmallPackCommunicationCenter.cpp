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

void SmallPack::SmallPackCommunicationCenter::GetMessagesFromPack(SmallPackPacker* _packer, SmallPack::MessagePack* _messagePack, udp::endpoint& _senderEndpoint, std::vector<NetworkMessage>& _messageVector, bool _createNewCommunicationChannels)
{
	// Check if the sender is connected to any of our communication channels
	if (!CommunicationChannelExists(_senderEndpoint.address(), _senderEndpoint.port(), _createNewCommunicationChannels))
	{
		// Ignore this message pack, invalid sender!
		return;
	}

	// Grab all messages inside this pack
	std::vector<NetworkMessage> messages = _packer->UnpackMessagePack(_messagePack);

	// Set the message sender info for each message
	for (auto & message : messages)
	{
		// Set both the address and the port
		message.senderInfo.address = _senderEndpoint.address();
		message.senderInfo.port = _senderEndpoint.port();
	}

	// Append the messages
	_messageVector.insert(std::end(_messageVector), std::begin(messages), std::end(messages));

	// Release the message pack
	_packer->ReleaseMessagePack(_messagePack);
}

void SmallPack::SmallPackCommunicationCenter::CheckForSystemMessages(SmallPackPacker* _packer, std::vector<NetworkMessage>& _messageVector, uint32_t _totalTime, float _elapsedTime)
{
	// For each received message
	for (int i = 0; i<_messageVector.size(); i++)
	{
		// Get the current message
		NetworkMessage* currentMessage = &_messageVector[i];

		// Check the ping request
		if (CheckFlag(currentMessage->messageHeader.messageFlags, PingCommandType::Request))
		{
			// Process this ping message
			ProcessPingMessage(_packer, currentMessage, PingCommandType::Request, _totalTime);
		}

		// Check the ping answer
		if (CheckFlag(currentMessage->messageHeader.messageFlags, PingCommandType::Answer))
		{
			// Process this ping message
			ProcessPingMessage(_packer, currentMessage, PingCommandType::Answer, _totalTime);
		}

		// Check for system messages
		if (currentMessage->messageHeader.messageOperator == SmallPack::Operator::System)
		{
			// Send this system message to the correct communication channel
			SendSystemMessageToCommunicationChannel(_packer, currentMessage, _totalTime, _elapsedTime);

			// Remove this message from the vector
			_messageVector.erase(_messageVector.begin() + i);
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

		// boost::system::error_code ignored_error;
		// m_ControllerData.connectionSocket.send_to(boost::asio::buffer(data, bufferSize), _endpoint, 0, ignored_error);

		// Copy into the message pack
		newMessagePack->CopyFromByteStream(data, length);
	}

	return newMessagePack;
}

/*


SmallPack::MessagePack* SmallPack::Client::SmallPackClientCommunicationCenter::CheckForNewMessages(SmallPackMessagePackList* _messagePackList, udp::endpoint& _endpoint)
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


*/