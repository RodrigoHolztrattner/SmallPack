////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackServerCommunicationCenter.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "../SmallPackConfig.h"
#include "../SmallPackCommunicationCenter.h"
#include "../SmallPackMessagePackReceiveBuffer.h"
#include "../SmallPackMessages.h"
#include "../SmallPackCommunicationChannel.h"

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
class SmallPackServerCommunicationCenter : public SmallPack::SmallPackCommunicationCenter
{

public:
	SmallPackServerCommunicationCenter(boost::asio::io_service& _ioService);
	~SmallPackServerCommunicationCenter();

public:

	// Broadcast the client connection info
	void BroadcastClientConnectionInfo(SmallPackPacker* _packer, uint32_t _currentTime);

	// Broadcast a message
	void BroadcastMessage(SmallPackPacker* _packer, NetworkMessage* _message, uint32_t _currentTime);

	// Initialize the communication controller
	bool Initialize(uint16_t _selfPort);

	// Create a client communication channel
	bool CreateClientCommunicationChannel(boost::asio::ip::address _senderAddress, uint32_t _port);

protected:

	// Create a new communication channel
	virtual SmallPackCommunicationChannel* CreateCommunicationChannel(boost::asio::ip::address _senderAddress, uint32_t _port);

///////////////
// VARIABLES //
private: //////

};

// Short type
typedef SmallPackServerCommunicationCenter SmallPackCommunicationCenter;

// Server
SmallPackNamespaceEnd(Server)

// SmallPack
SmallPackNamespaceEnd(SmallPack)