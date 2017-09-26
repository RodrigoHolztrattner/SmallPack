////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackClientCommunicationChannelReliable.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "../SmallPackConfig.h"
#include "SmallPackClientCommunicationChannel.h"

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

// We know the SmallPackPacker class
class SmallPackPacker;

// Client
SmallPackamespaceBegin(Client)

/*
	- Ao receber um ack do server nos também devemos receber a resposta do processamento da mensagem mesmo que ela seja vazia. Isso garante que se o server entregou um ack
	nós também recebemos a mensagem em si, logo dessa forma nós não precisamos usar um layer a mais do server para o cliente onde o server precisaria receber um ack do 
	cliente

*/

////////////////////////////////////////////////////////////////////////////////
// Class name: SmallPackClientCommunicationChannelReliable
////////////////////////////////////////////////////////////////////////////////
class SmallPackClientCommunicationChannelReliable : public SmallPackClientCommunicationChannel
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
	SmallPackClientCommunicationChannelReliable(boost::asio::io_service& _ioService);
	//SmallPackClientCommunicationChannelReliable(const SmallPackClientCommunicationChannelReliable&);
	~SmallPackClientCommunicationChannelReliable();

	// Do the frame update for this communication channel
	virtual void FrameUpdate(uint32_t _currentTime, float _timeElapsed);

	// Process a system message
	virtual void ProcessSystemMessage(SmallPackPacker* _packer, NetworkMessage* _message, uint32_t _currentTime);

protected:

	// Process a sent message pack
	virtual void ProcessSentMessagePack(SmallPackPacker* _packer, MessagePack* _messagePack, uint32_t _currentTime);

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

// Short type
typedef SmallPackClientCommunicationChannelReliable SmallPackCommunicationChannelReliable;

// Client
SmallPackNamespaceEnd(Client)

// SmallPack
SmallPackNamespaceEnd(SmallPack)