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