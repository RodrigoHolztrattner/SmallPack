////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackPacker.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "SmallPackConfig.h"

#include <cstdint>
#include "SmallPackMessages.h"

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
class SmallPackMessageComposer;

////////////////////////////////////////////////////////////////////////////////
// Class name: SmallPackPacker
////////////////////////////////////////////////////////////////////////////////
class SmallPackPacker
{
	// The SmallPackMessageComposer is a friend
	friend SmallPackMessageComposer;

private:

	// The total message reserved data
	const uint32_t TotalReservedData = 1024;

public:
	SmallPackPacker();
	SmallPackPacker(const SmallPackPacker&);
	~SmallPackPacker();

public:

	// Initialize
	bool Initialize();

	// Pack a given message and prepare it to be sent
	void PackMessage(NetworkMessage& _message /*, CommunicationChannel _channel*/);

protected:

	// Request message data
	template <typename ObjectType>
	bool RequestReservedMessageData(MessageData** _messageData)
	{
		// Set the required size
		//uint32_t requiredSize = sizeof(ObjectType);
		uint32_t requiredSize = pow2roundup(sizeof(ObjectType));
		
		// Check if we have enough remaining data
		if (m_TotalReservedMessageDataUsed + requiredSize >= TotalReservedData)
		{
			return false;
		}

		// Select the data
		unsigned char* selectedDataSlice = &m_MessagedReservedData[m_TotalReservedMessageDataUsed];

		// Increment the amount reserved data used
		m_TotalReservedMessageDataUsed += requiredSize;

		// Set the data location
		*_messageData->dataPtr = selectedDataSlice;
		*_messageData->dataSize = requiredSize;

		return true;
	}

public:

///////////////
// VARIABLES //
private: //////

	// The messaged reserved data
	unsigned char* m_MessagedReservedData;

	// The total message reserved data used
	uint32_t m_TotalReservedMessageDataUsed;
};

// SmallPack
SmallPackNamespaceEnd(SmallPack)