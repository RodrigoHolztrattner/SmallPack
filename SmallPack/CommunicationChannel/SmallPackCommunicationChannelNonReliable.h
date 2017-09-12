////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackCommunicationChannelNonReliable.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "../SmallPackConfig.h"
#include "SmallPackCommunicationChannel.h"

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

////////////////////////////////////////////////////////////////////////////////
// Class name: SmallPackCommunicationChannelNonReliable
////////////////////////////////////////////////////////////////////////////////
class SmallPackCommunicationChannelNonReliable : public SmallPackCommunicationChannel
{

public:
	SmallPackCommunicationChannelNonReliable(boost::asio::io_service& _ioService);
	//SmallPackCommunicationChannelNonReliable(const SmallPackCommunicationChannelNonReliable&);
	~SmallPackCommunicationChannelNonReliable();

public:

	// Do the frame update for this communication channel
	virtual void FrameUpdate(uint32_t _currentTime, float _timeElapsed);

	// Process a system message
	virtual void ProcessSystemMessage(SmallPackPacker* _packer, NetworkMessage* _message, uint32_t _currentTime);

protected:

	// Send all queued messages
	virtual void SendQueuedMessages(uint32_t _currentTime);

///////////////
// VARIABLES //
protected: ////

};

// SmallPack
SmallPackNamespaceEnd(SmallPack)