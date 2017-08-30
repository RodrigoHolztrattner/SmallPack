////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackCommunicationController.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "SmallPackConfig.h"

#include "SmallPackCommunicationChannel.h"
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

////////////////////////////////////////////////////////////////////////////////
// Class name: SmallPackCommunicationController
////////////////////////////////////////////////////////////////////////////////
class SmallPackCommunicationController
{
public:
	SmallPackCommunicationController(boost::asio::io_service& _ioService);
	//SmallPackCommunicationController(const SmallPackCommunicationController&);
	~SmallPackCommunicationController();

public:

	// Initialize the communication controller
	bool Initialize(uint16_t _port);

	// Start accepting incomming communication requests
	void Start();

	// Stop accepting incomming communication requests
	void Stop();

public:

	// Send a message to all connected clients
	bool BroadcastMessageToAllClients(NetworkMessage& _message);

private:

	// Listen for incomming requests
	void ListeningForRequests();

public:

///////////////
// VARIABLES //
private: //////

	// All the p2p connections
	std::vector<SmallPackCommunicationChannel*> m_ClientConnections;

	// The current io service
	boost::asio::io_service& m_ioService;

	// Our connection socket and endpoint
	boost::asio::ip::udp::socket m_ConnectionSocket;
	boost::asio::ip::udp::endpoint m_Endpoint;

	// Our current port
	uint16_t m_CurrentPort;

	// If we should listen for client requests
	bool m_ListenForClientRequests;
	
};

// SmallPack
SmallPackNamespaceEnd(SmallPack)