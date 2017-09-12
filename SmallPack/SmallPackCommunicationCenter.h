////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackCommunicationCenter.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "SmallPackConfig.h"

#include "CommunicationChannel/SmallPackCommunicationChannel.h"
#include "CommunicationChannel\SmallPackCommunicationChannelNonReliable.h"
#include "CommunicationChannel\SmallPackCommunicationChannelReliable.h"
#include "SmallPackMessagePackReceiveBuffer.h"
#include "SmallPackMessages.h"

#include <boost\asio.hpp>
#include <vector>

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

// We know the SmallPackPacker class
// class SmallPackPacker;

////////////////////////////////////////////////////////////////////////////////
// Class name: SmallPackCommunicationCenter
////////////////////////////////////////////////////////////////////////////////
class SmallPackCommunicationCenter
{
	// The controller data
	struct ControllerData
	{
		ControllerData(boost::asio::io_service& _ioService) : ioService(_ioService), connectionSocket(_ioService) {}

		// The current io service
		boost::asio::io_service& ioService;

		// Our connection socket and endpoint
		boost::asio::ip::udp::socket connectionSocket;
		boost::asio::ip::udp::endpoint endpoint;

		// Our current port
		uint16_t currentPort;
	};

public:
	SmallPackCommunicationCenter(boost::asio::io_service& _ioService);
	~SmallPackCommunicationCenter();

public:

	// Initialize the communication controller
	bool Initialize(const char* _serverAddress, const char* _serverPort, uint16_t _selfPort);

	// Do the update for our communication center
	void Update(SmallPackMessagePackList* _messagePackList, SmallPackPacker* _packer, uint32_t _totalTime, float _elapsedTime);

	// Send a message to all connected clients
	bool BroadcastMessageToAllClients(NetworkMessage& _message);

	// Check if a sender has a communication channel
	SmallPackCommunicationChannel* GetSenderCommunicationChannel(boost::asio::ip::address _senderAddress, uint32_t _port, bool _createIfNeed = false);

private:

	// Check for messages
	SmallPack::MessagePack* CheckForNewMessages(SmallPackMessagePackList* _messagePackList, boost::asio::ip::udp::endpoint& _endpoint);

public:

///////////////
// VARIABLES //
private: //////

	// The server communication channel
	SmallPackCommunicationChannelReliable m_ServerConnection;

	// All the p2p connections
	std::vector<SmallPackCommunicationChannel*> m_ClientConnections;

	// Our controller data
	ControllerData m_ControllerData;
};

// SmallPack
SmallPackNamespaceEnd(SmallPack)