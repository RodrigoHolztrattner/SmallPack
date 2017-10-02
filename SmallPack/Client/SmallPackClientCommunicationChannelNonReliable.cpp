////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackClientCommunicationChannelNonReliable.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SmallPackClientCommunicationChannelNonReliable.h"
#include <cstdio>

using namespace boost::asio::ip;

SmallPack::Client::SmallPackClientCommunicationChannelNonReliable::SmallPackClientCommunicationChannelNonReliable(boost::asio::io_service& _ioService, bool _canExpire) : SmallPack::SmallPackCommunicationChannel(_ioService, _canExpire)
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
	SmallPackCommunicationChannel::FrameUpdate(_currentTime, _timeElapsed);
}

void SmallPack::Client::SmallPackClientCommunicationChannelNonReliable::ProcessChannelInternalMessage(SmallPackPacker* _packer, NetworkMessage* _message, uint32_t _currentTime)
{
	// Call the base function
	SmallPackCommunicationChannel::ProcessChannelInternalMessage(_packer, _message, _currentTime);
}