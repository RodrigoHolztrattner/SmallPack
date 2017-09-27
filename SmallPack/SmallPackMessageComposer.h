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

	// Compose a new message
	template <typename ObjectType>
	bool Compose(SmallPackPacker* _packer, Operator _messageOperator, uint32_t _messageCommand, uint32_t _messageId, ObjectType& _object, NetworkMessage& _networkMessage)
	{
		NetworkMessage newMessage;

		// Set the message data
		newMessage.messageHeader.messageOperator = _messageOperator;
		newMessage.messageHeader.messageId = _messageId;
		newMessage.messageHeader.messageCommand = _messageCommand;
		newMessage.messageHeader.messageFlags = 0;

		// Request the message data
		bool result = _packer->RequestReservedMessageData(&newMessage.messageData, sizeof(ObjectType));
		if (!result)
		{
			// Could not allocate data for the message
			return false;
		}

		// Copy the message object
		memcpy(newMessage.messageData.dataPtr, &_object, sizeof(ObjectType));

		// Compute the message total size
		newMessage.ComputeMessageTotalSize();

		// Set the network message
		_networkMessage = newMessage;

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