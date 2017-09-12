////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackServerCommunicationCenter.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "../SmallPackConfig.h"

#include "../SmallPackMessagePackReceiveBuffer.h"
#include "../SmallPackMessages.h"
#include "SmallPackServerCommunicationChannel.h"

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

// Server
SmallPackamespaceBegin(Server)

// We know the SmallPackPacker class
// class SmallPackPacker;

////////////////////////////////////////////////////////////////////////////////
// Class name: SmallPackServerCommunicationCenter
////////////////////////////////////////////////////////////////////////////////
class SmallPackServerCommunicationCenter
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
	SmallPackServerCommunicationCenter(boost::asio::io_service& _ioService);
	~SmallPackServerCommunicationCenter();

public:

	// Initialize the communication controller
	bool Initialize(uint16_t _selfPort);

	// Do the update for our communication center
	void Update(SmallPackMessagePackList* _messagePackList, SmallPackPacker* _packer, uint32_t _totalTime, float _elapsedTime);

	// Send a message to all connected clients
	bool BroadcastMessageToAllClients(NetworkMessage& _message);

	// Check if a sender has a communication channel
	SmallPackServerCommunicationChannel* GetSenderCommunicationChannel(boost::asio::ip::address _senderAddress, uint32_t _port, bool _createIfNeed = false);

private:

	// Check for messages
	SmallPack::MessagePack* CheckForNewMessages(SmallPackMessagePackList* _messagePackList, boost::asio::ip::udp::endpoint& _endpoint);

///////////////
// VARIABLES //
private: //////

	// All the client connections
	std::vector<SmallPackServerCommunicationChannel*> m_ClientConnections;

	// Our controller data
	ControllerData m_ControllerData;
};

// Server
SmallPackNamespaceEnd(Server)

// SmallPack
SmallPackNamespaceEnd(SmallPack)