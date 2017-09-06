////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackCommunicationChannel.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "SmallPackConfig.h"
#include "SmallPackMessages.h"
#include "SmallPackMessagePackList.h"

#include <boost\asio.hpp>
#include <ctime>

/////////////
// DEFINES //
/////////////

////////////
// GLOBAL //
////////////

///////////////
// NAMESPACE //
///////////////

// SmallPack
SmallPackamespaceBegin(SmallPack)

////////////////////////////////////////////////////////////////////////////////
// Class name: SmallPackCommunicationChannel
////////////////////////////////////////////////////////////////////////////////
class SmallPackCommunicationChannel
{

	// The channel data
	struct ChannelData
	{
		// The time elapsed since the last ping
		clock_t lastPingElapsedTime;

		// How much time this connection is active
		clock_t totalTimeActive;

		// The channel address
		boost::asio::ip::address address;

		// The channel port
		uint32_t port;
	};

public:
	SmallPackCommunicationChannel(boost::asio::io_service& _ioService);
	//SmallPackCommunicationChannel(const SmallPackCommunicationChannel&);
	~SmallPackCommunicationChannel();

public:

	// Initialize this channel
	bool Initialize(const char* _host, const char* _port);

	// Send a message to this channel
	void Send(SmallPack::MessagePack* _messagePack);

	// Verify if the given host is the owner of this channel
	bool IsHost(boost::asio::ip::address _address, uint32_t _port);

public:

///////////////
// VARIABLES //
private: //////

	// The channel ID
	uint32_t m_ChannelIdentifier;

	// The channel data
	ChannelData m_ChannelData;

	// The current io service
	boost::asio::io_service& m_ioService;

	// Our socket and iterator
	boost::asio::ip::udp::socket m_Socket;
	boost::asio::ip::udp::resolver::iterator m_Iterator;
};

// SmallPack
SmallPackNamespaceEnd(SmallPack)