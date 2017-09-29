////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackCommunicationCenter.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "SmallPackConfig.h"

#include "SmallPackCommunicationChannel.h"
#include "Client\SmallPackClientCommunicationChannelNonReliable.h"
#include "Client\SmallPackClientCommunicationChannelReliable.h"
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
protected:

	// The server authentication token
	static const uint32_t ServerAuthenticationToken = 0;

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

		// Our authentication token
		uint32_t authenticationToken;
	};

public:
	SmallPackCommunicationCenter(boost::asio::io_service& _ioService);
	~SmallPackCommunicationCenter();

	// Do the update for our communication center
	virtual std::vector<NetworkMessage> Update(SmallPackMessagePackList* _messagePackList, SmallPackPacker* _packer, uint32_t _totalTime, float _elapsedTime);

	// Commit all messages inside this communication center
	virtual void CommitMessages(SmallPackPacker* _packer, uint32_t _totalTime);

protected:

	// Return all messaged from a given message pack
	void GetMessagesFromPack(SmallPackPacker* _packer, SmallPack::MessagePack* _messagePack, boost::asio::ip::udp::endpoint& _senderEndpoint, std::vector<NetworkMessage>& _messageVector, bool _createNewCommunicationChannels = false);

	// Check for messages
	SmallPack::MessagePack* CheckForNewMessages(SmallPackMessagePackList* _messagePackList, boost::asio::ip::udp::endpoint& _endpoint);

	// Verify a message vector for system messages
	void CheckForSystemMessages(SmallPackPacker* _packer, std::vector<NetworkMessage>& _messageVector, uint32_t _totalTime, float _elapsedTime);

	// Process a ping message
	virtual void ProcessPingMessage(SmallPackPacker* _packer, NetworkMessage* _message, PingCommandType _type, uint32_t _totalTime);
	
	// Check if a sender has a communication channel
	virtual SmallPackCommunicationChannel* GetSenderCommunicationChannel(boost::asio::ip::address _senderAddress, uint32_t _port, bool _createIfNeed);

	// Create a new communication channel
	virtual SmallPackCommunicationChannel* CreateCommunicationChannel(boost::asio::ip::address _senderAddress, uint32_t _port) = 0;

///////////////
// VARIABLES //
protected: ////

	// Our controller data
	ControllerData m_ControllerData;

// private:

	// All the p2p connections
	std::vector<SmallPackCommunicationChannel*> m_ClientConnections;
};

// SmallPack
SmallPackNamespaceEnd(SmallPack)