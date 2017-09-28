////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackMessageComposer.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "SmallPackConfig.h"

#include "SmallPackMessages.h"
#include "SmallPackPacker.h"

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

// We know th SmallPackPacker
//class SmallPackPacker;

////////////////////////////////////////////////////////////////////////////////
// Class name: SmallPackMessageComposer
////////////////////////////////////////////////////////////////////////////////
class SmallPackMessageComposer
{
	// The SmallPackPacker is a friend
	friend SmallPackPacker;

public:
	SmallPackMessageComposer();
	SmallPackMessageComposer(const SmallPackMessageComposer&);
	~SmallPackMessageComposer();

public:

	/*
		- Adicionar um modo de criação (compose) dummy para mensagens sem objeto
		- Verificar se a adição de multiplos objetos está funcionando
		- Permitir receber multiplos objetos de uma mensagem
	*/

	// Compose a new message
	template <typename ObjectType>
	static bool Compose(SmallPackPacker* _packer, NetworkMessage& _networkMessage, Operator _messageOperator, uint32_t _messageCommand, uint32_t _messageId, ObjectType _object, uint32_t _totalObjects = 1)
	{
		NetworkMessage newMessage;

		// Set the message data
		newMessage.messageHeader.messageOperator = _messageOperator;
		newMessage.messageHeader.messageId = _messageId;
		newMessage.messageHeader.messageCommand = _messageCommand;
		newMessage.messageHeader.messageFlags = 0;

		// Request the message data
		bool result = _packer->RequestReservedMessageData(&newMessage.messageData, sizeof(ObjectType) * _totalObjects);
		if (!result)
		{
			// Could not allocate the data for the message
			return false;
		}

		// Copy the message object
		memcpy(newMessage.messageData.dataPtr, _object, sizeof(ObjectType) * _totalObjects);

		// Compute the message total size
		newMessage.ComputeMessageTotalSize();

		// Set the network message
		_networkMessage = newMessage;

		return true;
	}

	// Return the message data (cast to the given object type)
	template <typename ObjectType>
	static bool GetDataObject(NetworkMessage* _message, ObjectType& _object)
	{
		// Check if the sizes are equal
		if (_message->messageData.dataSize != sizeof(ObjectType))
		{
			return false;
		}

		// Set the object
		_object = *(ObjectType*)_message->messageData.dataPtr;

		return true;
	}

private:




public:

///////////////
// VARIABLES //
private: //////

};

// SmallPack
SmallPackNamespaceEnd(SmallPack)