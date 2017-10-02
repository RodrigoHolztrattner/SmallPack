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

// We know the SmallPackMessageComposer class
class SmallPackMessageComposer;

////////////////////////////////////////////////////////////////////////////////
// Class name: SmallPackCommunicationChannel
////////////////////////////////////////////////////////////////////////////////
class SmallPackCommunicationChannel
{
protected:

	// The ping request time (milisseconds)
	static const uint32_t PingRequestInterval = 1000;

	// The time out amount (milisseconds)
	static const uint32_t TimeOutAmount = 5000;

	// The expire time amount (seconds)
	static const uint32_t ExpireTimeAmount = 5;

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
		boost::asio::ip::udp::endpoint endpoint;
		boost::asio::ip::udp::endpoint answerEndpoint;
		//boost::asio::ip::udp::resolver::iterator iterator;

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

		// If the other point is expecting a ping message
		bool receivedRequestedPing;

		// The ping id we are expected to receive
		uint32_t pingExpectedIdentifier;

		// The time since the last ping
		clock_t lastPingTime;

		// The time we sent the last ping request
		uint32_t lastRequestTime;
	};

public:
	SmallPackCommunicationChannel(boost::asio::io_service& _ioService, bool _canExpire);
	//SmallPackCommunicationChannel(const SmallPackCommunicationChannel&);
	~SmallPackCommunicationChannel();

public:

	// Initialize this channel
	virtual bool Initialize(const char* _host, const char* _port, uint32_t _answerPort, uint32_t _authenticationToken);

	// Do the frame update for this communication channel
	virtual void FrameUpdate(uint32_t _currentTime, float _timeElapsed);

	// Process a channel internal message
	virtual void ProcessChannelInternalMessage(SmallPackPacker* _packer, NetworkMessage* _message, uint32_t _currentTime);

	// Process a ping message
	virtual void ProcessPingAnswer(SmallPackPacker* _packer, NetworkMessage* _message, uint32_t _currentTime);

	// Insert a message (that will be sent) into our queue
	void QueueMessage(SmallPack::NetworkMessage* _message);
	
	// Commit all queued messages
	void CommitQueueMessage(SmallPackPacker* _packer, uint32_t _currentTime);

	// Verify if the given host is the owner of this channel
	bool IsHost(boost::asio::ip::address _address, uint32_t _port);

	// Request a ping message for this channel
	void RequestPing();

	// Reset the expiration timer for this channel
	void ResetExpireTimer();

	// If this channel should be reseted
	bool ShouldDelete(uint32_t _currentTime, float _timeElapsed);

	// Return the address
	boost::asio::ip::address GetAddress() { return m_ChannelData.address; }

	// Return the port
	uint32_t GetPort() { return m_ChannelData.port; }

protected:

	// Process a sent message pack
	virtual void ProcessSentMessagePack(SmallPackPacker* _packer, MessagePack* _messagePack, uint32_t _currentTime);

	// Send a given message pack
	void SendMessagePack(SmallPack::MessagePack* _messagePack);

	// Set the channel answer port
	void SetAnswerPort(uint32_t _anwerPort);

	// Set the channel authentication token
	void SetAuthenticationToken(uint32_t _token);

	// Set that this channel can expire
	void SetCanExpire();

private:

	// Process the ping functionality
	void ProcessPingFunctionality(SmallPackPacker* _packer, uint32_t _currentTime);

	// Pack a given message and prepare it to be sent
	void PackMessage(NetworkMessage& _message, SmallPackPacker* _packer);

///////////////
// VARIABLES //
protected: ////

	// The channel data
	ChannelData m_ChannelData;

	// The ping info
	PingInfo m_PingInfo;

	// The send queue
	std::vector<NetworkMessage*> m_SendQueue;

private:

	// The channel ID
	uint32_t m_ChannelIdentifier;

	// The authentication token identifier
	uint32_t m_AuthenticationToken;

	// The answer port for this channel
	uint32_t m_AnswerPort;

	// The message pack send queue
	MessagePack* m_MessagePackSendList;

	// If this connection channel can expire (and the expire time)
	bool m_CanExpire;
	float m_ExpireTime;
};

// SmallPack
SmallPackNamespaceEnd(SmallPack)