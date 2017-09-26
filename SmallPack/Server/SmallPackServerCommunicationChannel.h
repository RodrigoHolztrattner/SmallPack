////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackServerCommunicationChannel.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "../SmallPackConfig.h"
#include "../SmallPackMessages.h"
#include "../SmallPackMessagePackList.h"
#include "../SmallPackCommunicationChannel.h"

#include <boost\asio.hpp>
#include <ctime>

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

////////////////////////////////////////////////////////////////////////////////
// Class name: SmallPackServerCommunicationChannel
////////////////////////////////////////////////////////////////////////////////
class SmallPackServerCommunicationChannel : public SmallPack::SmallPackCommunicationChannel
{

public:
	SmallPackServerCommunicationChannel(boost::asio::io_service& _ioService);
	//SmallPackServerCommunicationChannel(const SmallPackServerCommunicationChannel&);
	~SmallPackServerCommunicationChannel();

public:

	// Initialize this channel
	bool Initialize(boost::asio::ip::address _senderAddress, uint32_t _port);

	// Do the frame update for this communication channel
	void FrameUpdate(uint32_t _currentTime, float _timeElapsed);

	// Process a system message
	void ProcessSystemMessage(SmallPackPacker* _packer, NetworkMessage* _message, uint32_t _currentTime);

private:

///////////////
// VARIABLES //
protected: ////

	// The send queue
	std::vector<MessagePack*> m_SendQueue;
};

// Short type
typedef SmallPackServerCommunicationChannel SmallPackCommunicationChannel;

// Server
SmallPackNamespaceEnd(Server)

// SmallPack
SmallPackNamespaceEnd(SmallPack)