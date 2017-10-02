////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackClientCommunicationChannelNonReliable.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "../SmallPackConfig.h"
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
// Class name: SmallPackClientCommunicationChannelNonReliable
////////////////////////////////////////////////////////////////////////////////
class SmallPackClientCommunicationChannelNonReliable : public SmallPack::SmallPackCommunicationChannel
{

public:
	SmallPackClientCommunicationChannelNonReliable(boost::asio::io_service& _ioService, bool _canExpire);
	//SmallPackClientCommunicationChannelNonReliable(const SmallPackClientCommunicationChannelNonReliable&);
	~SmallPackClientCommunicationChannelNonReliable();

public:

	// Do the frame update for this communication channel
	virtual void FrameUpdate(uint32_t _currentTime, float _timeElapsed);

	// Process a channel internal message
	virtual void ProcessChannelInternalMessage(SmallPackPacker* _packer, NetworkMessage* _message, uint32_t _currentTime);

///////////////
// VARIABLES //
protected: ////

};

// Short type
typedef SmallPackClientCommunicationChannelNonReliable SmallPackCommunicationChannelNonReliable;

// Client
SmallPackNamespaceEnd(Client)

// SmallPack
SmallPackNamespaceEnd(SmallPack)