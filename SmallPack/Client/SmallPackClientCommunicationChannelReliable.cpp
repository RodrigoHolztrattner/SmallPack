////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackClientCommunicationChannelReliable.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SmallPackClientCommunicationChannelReliable.h"
#include "..\SmallPackPacker.h"
#include "..\SmallPackMessageComposer.h"
#include <cstdio>

using namespace boost::asio::ip;

SmallPack::Client::SmallPackClientCommunicationChannelReliable::SmallPackClientCommunicationChannelReliable(boost::asio::io_service& _ioService) : SmallPack::SmallPackCommunicationChannel(_ioService)
{
}

/*
SmallPack::SmallPackClientCommunicationChannelReliable::SmallPackClientCommunicationChannelReliable(const SmallPack::SmallPackClientCommunicationChannelReliable& other)
{
}
*/

SmallPack::Client::SmallPackClientCommunicationChannelReliable::~SmallPackClientCommunicationChannelReliable()
{
}

void SmallPack::Client::SmallPackClientCommunicationChannelReliable::FrameUpdate(uint32_t _currentTime, float _timeElapsed)
{
	// Check for expired messages
	CheckForExpiredMessages(_currentTime);

	// Do the base frame update
	SmallPackCommunicationChannel::FrameUpdate(_currentTime, _timeElapsed);
}

void SmallPack::Client::SmallPackClientCommunicationChannelReliable::ProcessSystemMessage(SmallPackPacker* _packer, NetworkMessage* _message, uint32_t _currentTime)
{
	// Look for the delivery confirmation message type
	if (_message->messageHeader.IsFromType(SystemCommands::DeliveryConfirmation))
	{
		// Get the confirmation data
		CommandDeliveryConfirmation deliveryConfirmationData;
		if (SmallPackMessageComposer::GetDataObject(_message, deliveryConfirmationData))
		{
			// Process the delivery confirmation for this message
			ProcessDeliveryConfirmation(_packer, deliveryConfirmationData);
		}
	}

	// Call the base function
	SmallPackCommunicationChannel::ProcessSystemMessage(_packer, _message, _currentTime);
}

void SmallPack::Client::SmallPackClientCommunicationChannelReliable::ProcessSentMessagePack(SmallPackPacker* _packer, MessagePack* _messagePack, uint32_t _currentTime)
{
	// Insert the message into our reliable vector
	m_ReliableMessageData.push_back(ReliableData(_messagePack, _currentTime));
}

void SmallPack::Client::SmallPackClientCommunicationChannelReliable::ProcessDeliveryConfirmation(SmallPackPacker* _packer, CommandDeliveryConfirmation _deliveryConfirmation)
{
	// For each message inside our reliable vector
	for (int i = 0; i < m_ReliableMessageData.size(); i++)
	{
		// Get the current reliable data
		ReliableData& reliableData = m_ReliableMessageData[i];

		// Check the ids
		if (_deliveryConfirmation.confirmationIdentifier >= reliableData.messagePack->GetPackIdentification())
		{
			// Now we are free to release this message pack
			_packer->ReleaseMessagePack(reliableData.messagePack);

			// Remove this message from our vector
			m_ReliableMessageData.erase(m_ReliableMessageData.begin() + i);
			i--;
		}
	}
}

void SmallPack::Client::SmallPackClientCommunicationChannelReliable::CheckForExpiredMessages(uint32_t _currentTime)
{
	// Define the delay we can afford using the ping data
	uint32_t affordableDelay = m_PingInfo.ping * 2 /* ping-pong */ + m_PingInfo.ping * 0.4 /* extra margin */;

	// For each message inside our reliable vector
	for (int i = 0; i < m_ReliableMessageData.size(); i++)
	{
		// Get the current reliable data
		ReliableData& reliableData = m_ReliableMessageData[i];

		// Check if this message expired
		if (_currentTime - reliableData.timeSent >= affordableDelay)
		{
			// Re-send this message
			// SendMessagePack(reliableData.messagePack);

			// Set the new sent time
			reliableData.timeSent = _currentTime;
		}
	}
}