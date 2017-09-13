////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackClientCommunicationChannelNonReliable.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SmallPackClientCommunicationChannelNonReliable.h"
#include <cstdio>

using namespace boost::asio::ip;

SmallPack::Client::SmallPackClientCommunicationChannelNonReliable::SmallPackClientCommunicationChannelNonReliable(boost::asio::io_service& _ioService) : SmallPack::Client::SmallPackClientCommunicationChannel(_ioService)
{
}

/*
SmallPack::SmallPackClientCommunicationChannelNonReliable::SmallPackClientCommunicationChannelNonReliable(const SmallPack::SmallPackClientCommunicationChannelNonReliable& other)
{
}
*/

SmallPack::Client::SmallPackClientCommunicationChannelNonReliable::~SmallPackClientCommunicationChannelNonReliable()
{
}


void SmallPack::Client::SmallPackClientCommunicationChannelNonReliable::FrameUpdate(uint32_t _currentTime, float _timeElapsed)
{
	// Do the base frame update
	SmallPackClientCommunicationChannel::FrameUpdate(_currentTime, _timeElapsed);
}

void SmallPack::Client::SmallPackClientCommunicationChannelNonReliable::ProcessSystemMessage(SmallPackPacker* _packer, NetworkMessage* _message, uint32_t _currentTime)
{
	// Call the base function
	SmallPackClientCommunicationChannel::ProcessSystemMessage(_packer, _message, _currentTime);
}

void SmallPack::Client::SmallPackClientCommunicationChannelNonReliable::SendQueuedMessages(uint32_t _currentTime)
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
