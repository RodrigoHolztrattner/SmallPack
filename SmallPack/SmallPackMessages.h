////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackMessages.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "SmallPackConfig.h"
#include <cstdint>

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

enum class Endpoint
{
	Client,
	Server
};

enum class Operation
{
	StayAlive,
	Command,
	Update
};

struct MessageData
{
	unsigned char* dataPtr;
	uint32_t dataSize;
};

struct NetworkMessage
{
	// The message flags
	uint32_t messageFlags;

	// The message operation
	uint32_t messageOperation;

	// The message id
	uint32_t messageId;

	// The message data
	MessageData messageData;
};

// SmallPack
SmallPackNamespaceEnd(SmallPack)