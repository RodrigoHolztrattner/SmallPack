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
class SmallPackServerCommunicationCenter : SmallPack::SmallPackCommunicationCenter
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

	// Check if a sender has a communication channel
	SmallPackServerCommunicationChannel* GetSenderCommunicationChannel(boost::asio::ip::address _senderAddress, uint32_t _port, uint32_t _answerPort = 0, bool _createIfNeed = false);

	// Do the update for our communication center
	virtual std::vector<NetworkMessage> Update(SmallPackMessagePackList* _messagePackList, SmallPackPacker* _packer, uint32_t _totalTime, float _elapsedTime);

	// Commit all messages inside this communication center
	virtual void CommitMessages(SmallPackPacker* _packer, SmallPackMessageComposer* _composer, uint32_t _totalTime);

	// Create a client communication channel
	bool CreateClientCommunicationChannel(boost::asio::ip::address _senderAddress, uint32_t _port, uint32_t _authToken, uint32_t _answerPort);

protected:

	// Check if we have a given communication channel
	virtual bool CommunicationChannelExists(boost::asio::ip::address _senderAddress, uint32_t _port, uint32_t _answerPort, bool _createIfNeed);

	// Send a system message to the given communication channel
	virtual void SendSystemMessageToCommunicationChannel(SmallPackPacker* _packer, NetworkMessage* _systemMessage, uint32_t _totalTime, float _elapsedTime);

	// Process a ping message
	virtual void ProcessPingMessage(SmallPackPacker* _packer, NetworkMessage* _message, PingCommandType _type, uint32_t _totalTime);

///////////////
// VARIABLES //
private: //////

	// All the client connections
	std::vector<SmallPackServerCommunicationChannel*> m_ClientConnections;
};

// Short type
typedef SmallPackServerCommunicationCenter SmallPackCommunicationCenter;

// Server
SmallPackNamespaceEnd(Server)

// SmallPack
SmallPackNamespaceEnd(SmallPack)