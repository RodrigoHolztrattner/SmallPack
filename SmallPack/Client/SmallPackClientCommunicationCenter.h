////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackClientCommunicationCenter.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "..\SmallPackConfig.h"

#include "..\SmallPackCommunicationCenter.h"
#include "SmallPackClientCommunicationChannel.h"
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

	// Check if a sender has a communication channel
	Client::SmallPackCommunicationChannel* GetSenderCommunicationChannel(boost::asio::ip::address _senderAddress, uint32_t _port, bool _createIfNeed = false);

	// <temp> return the server communication channel
	Client::SmallPackCommunicationChannelReliable* GetServerCommunicationChannel() { return &m_ServerConnection; }

	// Do the update for our communication center
	virtual std::vector<NetworkMessage> Update(SmallPackMessagePackList* _messagePackList, SmallPackPacker* _packer, uint32_t _totalTime, float _elapsedTime);

	// Commit all messages inside this communication center
	virtual void CommitMessages(SmallPackPacker* _packer, SmallPackMessageComposer* _composer, uint32_t _totalTime);

protected:

	// Check if we have a given communication channel
	virtual bool CommunicationChannelExists(boost::asio::ip::address _senderAddress, uint32_t _port, bool _createIfNeed);

	// Send a system message to the given communication channel
	virtual void SendSystemMessageToCommunicationChannel(SmallPackPacker* _packer, NetworkMessage* _systemMessage, uint32_t _totalTime, float _elapsedTime);

	// Process a ping message
	virtual void ProcessPingMessage(SmallPackPacker* _packer, NetworkMessage* _message, PingCommandType _type, uint32_t _totalTime);

///////////////
// VARIABLES //
private: //////

	// The server communication channel
	Client::SmallPackCommunicationChannelReliable m_ServerConnection;

	// All the p2p connections
	std::vector<Client::SmallPackCommunicationChannel*> m_ClientConnections;
};

// Short type
typedef SmallPackClientCommunicationCenter SmallPackCommunicationCenter;

// Client
SmallPackNamespaceEnd(Client)

// SmallPack
SmallPackNamespaceEnd(SmallPack)