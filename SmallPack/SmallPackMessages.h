////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackMessages.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "SmallPackConfig.h"

#include <boost\asio.hpp>
#include <cstdint>
#include <memory>

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

//////////////////////
// MAIN DEFINITIONS //
//////////////////////

enum class Endpoint
{
	Client,
	Server
};

enum class Operator
{
	System,
	Application
};

enum class SystemCommands
{
	Ping,
	DeliveryConfirmation,
	ClientConnectInfo
};

enum PingCommandType
{
	Request			= 1 << 0,
	Answer			= 1 << 1
};

struct CommandDeliveryConfirmation
{
	// The confirmation identifier
	uint32_t confirmationIdentifier;
};

/////////////////////
// NETWORK MESSAGE //
/////////////////////

struct MessageData
{
	unsigned char* dataPtr;
	uint32_t dataSize;
};

struct MessageHeader
{
	// The message flags <not used>
	uint32_t messageFlags;

	// The message operator <who owns this message>
	Operator messageOperator;

	// The message command <what this message is about (ping, custom message, etc)>
	uint32_t messageCommand;

	// The message id <internal identification of this message, used only by the CommunicationChannel>
	uint32_t messageId;

	// The answer port
	uint32_t answerPort;

	// The authentication token <0 in case of server>
	uint32_t authToken;

	// Return the ID
	template <typename Type>
	Type GetType() { return (Type)messageCommand; }

	// Set the type
	template <typename Type>
	void SetType(Type _type) { messageCommand = (uint32_t)_type; }

	// Check if this message is from the given type
	template <typename Type>
	bool IsFromType(Type _type) { return (Type)messageCommand == _type; }
};

struct SenderInfo
{
	// The sender address
	boost::asio::ip::address address;

	// The sender port
	uint32_t port;
};

struct NetworkMessage
{
	// The message total size <serializable>
	uint32_t messageTotalSize;

	// The message header <serializable>
	MessageHeader messageHeader;

	// The message data <serializable>
	MessageData messageData;

	// The sender info (only used when a message was received, ignored when composing a new one)
	SenderInfo senderInfo;

	// Serialize this message
	void Serialize(unsigned char* _to, uint32_t& _currentPosition)
	{
		// Get to the message total size (so we can update the total size)
		uint32_t* totalSizePtr = (uint32_t*)&_to[_currentPosition];

		// Save the current position inside the data
		uint32_t savedPosition = _currentPosition;

		// Copy the data
		memcpy(&_to[_currentPosition], &messageTotalSize, sizeof(uint32_t)); _currentPosition += sizeof(uint32_t);
		memcpy(&_to[_currentPosition], &messageHeader, sizeof(MessageHeader)); _currentPosition += sizeof(MessageHeader);
		memcpy(&_to[_currentPosition], &messageData.dataSize, sizeof(uint32_t)); _currentPosition += sizeof(uint32_t);
		memcpy(&_to[_currentPosition], messageData.dataPtr, sizeof(unsigned char) * messageData.dataSize); _currentPosition += sizeof(unsigned char) * messageData.dataSize;

		// Update the total size
		*totalSizePtr = _currentPosition - savedPosition;
	}

	// Deserialize data into this message
	void Deserialize(unsigned char* _data, uint32_t& _currentPosition)
	{
		memcpy(&messageTotalSize, &_data[_currentPosition], sizeof(uint32_t)); _currentPosition += sizeof(uint32_t);
		memcpy(&messageHeader, &_data[_currentPosition], sizeof(MessageHeader)); _currentPosition += sizeof(MessageHeader);
		memcpy(&messageData.dataSize, &_data[_currentPosition], sizeof(uint32_t)); _currentPosition += sizeof(uint32_t);
		memcpy(messageData.dataPtr, &_data[_currentPosition], sizeof(unsigned char) * messageData.dataSize); _currentPosition += sizeof(unsigned char) * messageData.dataSize;
	}

	// Compute the message total size
	void ComputeMessageTotalSize()
	{
		messageTotalSize = sizeof(uint32_t) + sizeof(MessageHeader) + sizeof(uint32_t) + sizeof(unsigned char) * messageData.dataSize;
	}
};

// SmallPack
SmallPackNamespaceEnd(SmallPack)