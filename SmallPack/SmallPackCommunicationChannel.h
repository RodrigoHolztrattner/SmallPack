////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackCommunicationChannel.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "SmallPackConfig.h"
#include "SmallPackMessages.h"

#include <boost\asio.hpp>

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

public:
	SmallPackCommunicationChannel(boost::asio::io_service& _ioService);
	//SmallPackCommunicationChannel(const SmallPackCommunicationChannel&);
	~SmallPackCommunicationChannel();

public:

	// Initialize this channel
	bool Initialize(const char* _host, const char* _port);

	// Send a message to this channel
	void Send(SmallPack::NetworkMessage& _message);

public:

///////////////
// VARIABLES //
private: //////

	// The channel ID
	uint32_t m_ChannelIdentifier;

	// The current io service
	boost::asio::io_service& m_ioService;

	// Our socket and iterator
	boost::asio::ip::udp::socket m_Socket;
	boost::asio::ip::udp::resolver::iterator m_Iterator;
};

// SmallPack
SmallPackNamespaceEnd(SmallPack)