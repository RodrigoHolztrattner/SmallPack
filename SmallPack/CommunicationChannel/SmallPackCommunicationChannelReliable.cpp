////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackCommunicationChannelReliable.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SmallPackCommunicationChannelReliable.h"
#include "../SmallPackPacker.h"
#include <cstdio>

using namespace boost::asio::ip;

SmallPack::SmallPackCommunicationChannelReliable::SmallPackCommunicationChannelReliable(boost::asio::io_service& _ioService) : SmallPack::SmallPackCommunicationChannel::SmallPackCommunicationChannel(_ioService)
{
}

/*
SmallPack::SmallPackCommunicationChannelReliable::SmallPackCommunicationChannelReliable(const SmallPack::SmallPackCommunicationChannelReliable& other)
{
}
*/

SmallPack::SmallPackCommunicationChannelReliable::~SmallPackCommunicationChannelReliable()
{
}

void SmallPack::SmallPackCommunicationChannelReliable::FrameUpdate(uint32_t _currentTime, float _timeElapsed)
{
	// Check for expired messages
	CheckForExpiredMessages(_currentTime);

	// Do the base frame update
	SmallPackCommunicationChannel::FrameUpdate(_currentTime, _timeElapsed);
}

void SmallPack::SmallPackCommunicationChannelReliable::ProcessSystemMessage(SmallPackPacker* _packer, NetworkMessage* _message, uint32_t _currentTime)
{
	// Look for the delivery confirmation message type
	if (_message->messageHeader.IsFromType(SystemCommands::DeliveryConfirmation))
	{
		// Get the confirmation data
		CommandDeliveryConfirmation deliveryConfirmationData;
		_message->GetDataObject(deliveryConfirmationData);

		// Process the delivery confirmation for this message
		ProcessDeliveryConfirmation(_packer, deliveryConfirmationData);
	}

	// Call the base function
	SmallPackCommunicationChannel::ProcessSystemMessage(_packer, _message, _currentTime);
}

void SmallPack::SmallPackCommunicationChannelReliable::SendQueuedMessages(uint32_t _currentTime)
{
	// For each queued message pack
	for (int i = 0; i < m_SendQueue.size(); i++)
	{
		// Send this message pack
		SendMessagePack(m_SendQueue[i]);

		// Insert the message into our reliable vector
		m_ReliableMessageData.push_back(ReliableData(m_SendQueue[i], _currentTime));
	}

	// Clear the send queue
	m_SendQueue.clear();
}

void SmallPack::SmallPackCommunicationChannelReliable::ProcessDeliveryConfirmation(SmallPackPacker* _packer, CommandDeliveryConfirmation _deliveryConfirmation)
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

void SmallPack::SmallPackCommunicationChannelReliable::CheckForExpiredMessages(uint32_t _currentTime)
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
			SendMessagePack(reliableData.messagePack);

			// Set the new sent time
			reliableData.timeSent = _currentTime;
		}
	}
}