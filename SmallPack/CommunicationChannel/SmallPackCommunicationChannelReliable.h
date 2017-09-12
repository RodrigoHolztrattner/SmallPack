////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackCommunicationChannelReliable.h
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

/*
	- Ao receber um ack do server nos tamb�m devemos receber a resposta do processamento da mensagem mesmo que ela seja vazia. Isso garante que se o server entregou um ack
	n�s tamb�m recebemos a mensagem em si, logo dessa forma n�s n�o precisamos usar um layer a mais do server para o cliente onde o server precisaria receber um ack do 
	cliente

*/

// We know the SmallPackPacker class
class SmallPackPacker;

////////////////////////////////////////////////////////////////////////////////
// Class name: SmallPackCommunicationChannelReliable
////////////////////////////////////////////////////////////////////////////////
class SmallPackCommunicationChannelReliable : public SmallPackCommunicationChannel
{
	// The reliable data structure we will use to hold the messages
	struct ReliableData
	{
		ReliableData() {}
		ReliableData(MessagePack* _messagePack, uint32_t _timeSent) : messagePack(_messagePack), timeSent(_timeSent) {}

		// The time we sent this message
		uint32_t timeSent;

		// The message pack
		MessagePack* messagePack;
	};

public:
	SmallPackCommunicationChannelReliable(boost::asio::io_service& _ioService);
	//SmallPackCommunicationChannelReliable(const SmallPackCommunicationChannelReliable&);
	~SmallPackCommunicationChannelReliable();

	// Do the frame update for this communication channel
	virtual void FrameUpdate(uint32_t _currentTime, float _timeElapsed);

	// Process a system message
	virtual void ProcessSystemMessage(SmallPackPacker* _packer, NetworkMessage* _message, uint32_t _currentTime);

protected:

	// Send all queued messages
	virtual void SendQueuedMessages(uint32_t _currentTime);

private:

	// Check if we need to re-send any expired messages
	void CheckForExpiredMessages(uint32_t _currentTime);

	// Process a delivery confirmation message
	void ProcessDeliveryConfirmation(SmallPackPacker* _packer, CommandDeliveryConfirmation _deliveryConfirmation);

///////////////
// VARIABLES //
protected: ////

	// The reliable data
	std::vector<ReliableData> m_ReliableMessageData;
};

// SmallPack
SmallPackNamespaceEnd(SmallPack)