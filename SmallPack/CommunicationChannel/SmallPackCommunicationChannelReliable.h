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
	- Enviamos uma mensagem nossa para o servidor, esperamos que ele responda algo
	- Enquanto a resposta não chega nos guardamos o message pack num vetor de espera
	- Caso tenhamos enviado (por algum motivo que não se encontra aqui na especificacao) uma mensagem que nao precise de uma resposta, o servidor ainda sim deve enviar um "ok" (ack) e
	no caso do servidor enviar uma resposta consistente, podemos confirmar o recebimento daquele pack



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