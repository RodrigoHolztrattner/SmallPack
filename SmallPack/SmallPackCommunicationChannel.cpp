////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackCommunicationChannel.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SmallPackCommunicationChannel.h"
#include <cstdio>
#include <iostream>
#include <stdlib.h>     /* atoi */

using namespace boost::asio::ip;

SmallPack::SmallPackCommunicationChannel::SmallPackCommunicationChannel(boost::asio::io_service& _ioService) : m_ioService(_ioService), m_Socket(_ioService)
{
}

/*
SmallPack::SmallPackCommunicationChannel::SmallPackCommunicationChannel(const SmallPack::SmallPackCommunicationChannel& other)
{
}
*/

SmallPack::SmallPackCommunicationChannel::~SmallPackCommunicationChannel()
{
}

bool SmallPack::SmallPackCommunicationChannel::Initialize(const char* _host, const char* _port)
{
	// Resolve the ip version with the given host and port to get our iterator
	udp::resolver resolver(m_ioService);
	udp::resolver::query query(udp::v4(), _host, _port);
	m_Iterator = resolver.resolve(query);

	// Set the channel data
	m_ChannelData.address.from_string(_host);
	m_ChannelData.port = atoi(_port);
	
	return true;
}

void SmallPack::SmallPackCommunicationChannel::ProcessSystemMessage(NetworkMessage* _message)
{

}

void SmallPack::SmallPackCommunicationChannel::Send(SmallPack::MessagePack* _messagePack)
{
	const uint32_t bufferSize = 2048;
	unsigned char data[bufferSize];

	// Copy the message pack to our bytestream
	uint32_t length =_messagePack->CopyToByteStream(data);

	// Send the message pack through this channel
	m_Socket.send_to(boost::asio::buffer(data, length), *m_Iterator);
}

bool SmallPack::SmallPackCommunicationChannel::IsHost(boost::asio::ip::address _address, uint32_t _port)
{
	if (m_ChannelData.address == _address && m_ChannelData.port == _port)
	{
		return true;
	}

	return false;
}