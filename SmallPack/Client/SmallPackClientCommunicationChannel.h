////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackClientCommunicationChannel.h
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

// Client
SmallPackamespaceBegin(Client)

////////////////////////////////////////////////////////////////////////////////
// Class name: SmallPackClientCommunicationChannel
////////////////////////////////////////////////////////////////////////////////
class SmallPackClientCommunicationChannel : public SmallPack::SmallPackCommunicationChannel
{

public:
	SmallPackClientCommunicationChannel(boost::asio::io_service& _ioService);
	//SmallPackClientCommunicationChannel(const SmallPackClientCommunicationChannel&);
	~SmallPackClientCommunicationChannel();

public:

	// Initialize this channel
	bool Initialize(const char* _host, const char* _port);

	// Do the frame update for this communication channel
	virtual void FrameUpdate(uint32_t _currentTime, float _timeElapsed);

	// Process a system message
	virtual void ProcessSystemMessage(SmallPackPacker* _packer, NetworkMessage* _message, uint32_t _currentTime);

protected:

	// Process a sent message pack
	// virtual void ProcessSentMessagePack(SmallPackPacker* _packer, MessagePack* _messagePack);

///////////////
// VARIABLES //
protected: ////

};

// Short type
typedef SmallPackClientCommunicationChannel SmallPackCommunicationChannel;

// Client
SmallPackNamespaceEnd(Client)

// SmallPack
SmallPackNamespaceEnd(SmallPack)