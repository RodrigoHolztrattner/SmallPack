////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackServerCommunicationChannel.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "../SmallPackConfig.h"
#include "../SmallPackMessages.h"
#include "../SmallPackMessagePackList.h"

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

// Server
SmallPackamespaceBegin(Server)

////////////////////////////////////////////////////////////////////////////////
// Class name: SmallPackServerCommunicationChannel
////////////////////////////////////////////////////////////////////////////////
class SmallPackServerCommunicationChannel
{

	// The channel data
	struct ChannelData
	{
		// The constructor itself
		ChannelData(boost::asio::io_service& _ioService) : ioService(_ioService), socket(_ioService) {}

		// How much time this connection is active
		clock_t totalTimeActive;

		// The current io service
		boost::asio::io_service& ioService;

		// Our socket and iterator
		boost::asio::ip::udp::socket socket;
		boost::asio::ip::udp::resolver::iterator iterator;

		// The channel address
		boost::asio::ip::address address;

		// The channel port
		uint32_t port;
	};

	// The ping info
	struct PingInfo
	{
		// The initial ping
		static const uint32_t InitialPing = 5000;

		// The current ping time
		uint32_t ping;

		// If we are expecting a ping message
		bool expectingPing;

		// The ping id we are expected to receive
		uint32_t pingMessageIdentifier;

		// The time elapsed since the last ping
		clock_t lastPingElapsedTime;
	};

public:
	SmallPackServerCommunicationChannel(boost::asio::io_service& _ioService);
	//SmallPackServerCommunicationChannel(const SmallPackServerCommunicationChannel&);
	~SmallPackServerCommunicationChannel();

public:

	// Initialize this channel
	bool Initialize(boost::asio::ip::address _senderAddress, uint32_t _port);

	// Do the frame update for this communication channel
	void FrameUpdate(uint32_t _currentTime, float _timeElapsed);

	// Process a system message
	void ProcessSystemMessage(SmallPackPacker* _packer, NetworkMessage* _message, uint32_t _currentTime);

	// Insert a message pack (that will be sent) into our queue
	void QueueMessage(SmallPack::MessagePack* _messagePack);

	// Verify if the given host is the owner of this channel
	bool IsHost(boost::asio::ip::address _address, uint32_t _port);

protected:

	// Send a given message pack
	void SendMessagePack(SmallPack::MessagePack* _messagePack);

	// Process a ping message
	void ProcessPingCommand(CommandPing _pingData, uint32_t _currentTime);

	// Send all queued messages
	void SendQueuedMessages(uint32_t _currentTime);

private:

///////////////
// VARIABLES //
protected: ////

	// The channel data
	ChannelData m_ChannelData;

	// The ping info
	PingInfo m_PingInfo;

	// The send queue
	std::vector<MessagePack*> m_SendQueue;

private:

	// The channel ID
	uint32_t m_ChannelIdentifier;
};

// Short type
typedef SmallPackServerCommunicationChannel SmallPackCommunicationChannel;

// Server
SmallPackNamespaceEnd(Server)

// SmallPack
SmallPackNamespaceEnd(SmallPack)