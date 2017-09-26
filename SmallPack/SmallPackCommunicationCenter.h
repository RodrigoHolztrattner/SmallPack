////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackCommunicationCenter.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "SmallPackConfig.h"

#include "Client\SmallPackClientCommunicationChannel.h"
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

	// Do the update for our communication center
	virtual std::vector<NetworkMessage> Update(SmallPackMessagePackList* _messagePackList, SmallPackPacker* _packer, uint32_t _totalTime, float _elapsedTime) = 0;

	// Commit all messages inside this communication center
	virtual void CommitMessages(SmallPackPacker* _packer, SmallPackMessageComposer* _composer, uint32_t _totalTime) = 0;

protected:

	// Return all messaged from a given message pack
	void GetMessagesFromPack(SmallPackPacker* _packer, SmallPack::MessagePack* _messagePack, boost::asio::ip::udp::endpoint& _senderEndpoint, std::vector<NetworkMessage>& _messageVector, bool _createNewCommunicationChannels = false);

	// Check for messages
	SmallPack::MessagePack* CheckForNewMessages(SmallPackMessagePackList* _messagePackList, boost::asio::ip::udp::endpoint& _endpoint);

	// Verify a message vector for system messages
	void CheckForSystemMessages(SmallPackPacker* _packer, std::vector<NetworkMessage>& _messageVector, uint32_t _totalTime, float _elapsedTime);

	// Process a ping message
	virtual void ProcessPingMessage(SmallPackPacker* _packer, NetworkMessage* _message, PingCommandType _type, uint32_t _totalTime) = 0;
	
	// Check if we have a given communication channel
	virtual bool CommunicationChannelExists(boost::asio::ip::address _senderAddress, uint32_t _port, bool _createIfNeed) = 0;

	// Send a system message to the given communication channel
	virtual void SendSystemMessageToCommunicationChannel(SmallPackPacker* _packer, NetworkMessage* _systemMessage, uint32_t _totalTime, float _elapsedTime) = 0;

///////////////
// VARIABLES //
protected: ////

	// Our controller data
	ControllerData m_ControllerData;
};

// SmallPack
SmallPackNamespaceEnd(SmallPack)