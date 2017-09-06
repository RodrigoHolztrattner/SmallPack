////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackCommunicationChannel.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SmallPackCommunicationChannel.h"
#include <cstdio>
#include <iostream>

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
	udp::resolver resolver(m_ioService);
	udp::resolver::query query(udp::v4(), _host, _port);
	m_Iterator = resolver.resolve(query);

	return true;
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