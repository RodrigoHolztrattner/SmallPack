////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackCommunicationChannelNonReliable.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SmallPackCommunicationChannelNonReliable.h"
#include <cstdio>

using namespace boost::asio::ip;

SmallPack::SmallPackCommunicationChannelNonReliable::SmallPackCommunicationChannelNonReliable(boost::asio::io_service& _ioService) : SmallPack::SmallPackCommunicationChannel::SmallPackCommunicationChannel(_ioService)
{
}

/*
SmallPack::SmallPackCommunicationChannelNonReliable::SmallPackCommunicationChannelNonReliable(const SmallPack::SmallPackCommunicationChannelNonReliable& other)
{
}
*/

SmallPack::SmallPackCommunicationChannelNonReliable::~SmallPackCommunicationChannelNonReliable()
{
}


void SmallPack::SmallPackCommunicationChannelNonReliable::FrameUpdate(uint32_t _currentTime, float _timeElapsed)
{
	// Do the base frame update
	SmallPackCommunicationChannel::FrameUpdate(_currentTime, _timeElapsed);
}

void SmallPack::SmallPackCommunicationChannelNonReliable::ProcessSystemMessage(SmallPackPacker* _packer, NetworkMessage* _message, uint32_t _currentTime)
{
	// Call the base function
	SmallPackCommunicationChannel::ProcessSystemMessage(_packer, _message, _currentTime);
}

void SmallPack::SmallPackCommunicationChannelNonReliable::SendQueuedMessages(uint32_t _currentTime)
{
	// For each queued message pack
	for (int i = 0; i < m_SendQueue.size(); i++)
	{
		// Send this message pack
		SendMessagePack(m_SendQueue[i]);
	}

	// Clear the send queue
	m_SendQueue.clear();
}
