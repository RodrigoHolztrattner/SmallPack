////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackCommunicationCenter.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "SmallPackConfig.h"

#include "SmallPackCommunicationChannel.h"
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
	bool Initialize(uint16_t _port);

	// Do the update for our communication center
	void Update(SmallPackMessagePackList* _messagePackList, SmallPackPacker* _packer, float _elapsedTime);

public:

	// Send a message to all connected clients
	bool BroadcastMessageToAllClients(NetworkMessage& _message);

private:

	// Check for messages
	SmallPack::MessagePack* CheckForNewMessages(SmallPackMessagePackList* _messagePackList, boost::asio::ip::udp::endpoint& _endpoint);

public:

///////////////
// VARIABLES //
private: //////

	// All the p2p connections
	std::vector<SmallPackCommunicationChannel*> m_ClientConnections;

	// Our controller data
	ControllerData m_ControllerData;

	// The receive buffer
	SmallPackMessagePackReceiveBuffer m_ReceiveBuffer;
};

// SmallPack
SmallPackNamespaceEnd(SmallPack)