////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackClientCommunicationCenter.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "..\SmallPackConfig.h"

#include "..\SmallPackCommunicationCenter.h"
#include "../SmallPackCommunicationChannel.h"
#include "SmallPackClientCommunicationChannelNonReliable.h"
#include "SmallPackClientCommunicationChannelReliable.h"
#include "..\SmallPackMessagePackReceiveBuffer.h"
#include "..\SmallPackMessages.h"

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

// We know the SmallPackMessageComposer class
class SmallPackMessageComposer;

// Client
SmallPackamespaceBegin(Client)

// We know the SmallPackPacker class
// class SmallPackPacker;

////////////////////////////////////////////////////////////////////////////////
// Class name: SmallPackClientCommunicationCenter
////////////////////////////////////////////////////////////////////////////////
class SmallPackClientCommunicationCenter : public SmallPack::SmallPackCommunicationCenter
{

public:
	SmallPackClientCommunicationCenter(boost::asio::io_service& _ioService);
	~SmallPackClientCommunicationCenter();

public:

	// Initialize the communication controller
	bool Initialize(const char* _serverAddress, const char* _serverPort, uint16_t _selfPort);

	// Send a message to all connected clients
	bool BroadcastMessageToAllClients(NetworkMessage& _message);

	// <temp> return the server communication channel
	Client::SmallPackCommunicationChannelReliable* GetServerCommunicationChannel() { return &m_ServerConnection; }

	// Do the update for our communication center
	virtual std::vector<NetworkMessage> Update(SmallPackMessagePackList* _messagePackList, SmallPackPacker* _packer, uint32_t _totalTime, float _elapsedTime);

	// Commit all messages inside this communication center
	virtual void CommitMessages(SmallPackPacker* _packer, uint32_t _totalTime);

protected:

	// Check if a sender has a communication channel
	virtual SmallPackCommunicationChannel* GetSenderCommunicationChannel(boost::asio::ip::address _senderAddress, uint32_t _port, bool _createIfNeed = false);

	// Create a new communication channel
	virtual SmallPackCommunicationChannel* CreateCommunicationChannel(boost::asio::ip::address _senderAddress, uint32_t _port);

///////////////
// VARIABLES //
private: //////

	// The server communication channel
	Client::SmallPackCommunicationChannelReliable m_ServerConnection;
};

// Short type
typedef SmallPackClientCommunicationCenter SmallPackCommunicationCenter;

// Client
SmallPackNamespaceEnd(Client)

// SmallPack
SmallPackNamespaceEnd(SmallPack)