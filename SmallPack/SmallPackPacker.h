////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackPacker.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "SmallPackConfig.h"

#include <cstdint>
#include <vector>
#include "SmallPackMessages.h"
#include "SmallPackMessagePackList.h"

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
	const uint32_t TotalReservedData = 1024 * 32;

	// The max pack data size
	const uint32_t MessagePackMaxData = 512;

public:
	SmallPackPacker();
	SmallPackPacker(const SmallPackPacker&);
	~SmallPackPacker();

public:

	// Initialize
	bool Initialize();

	// Request a new message pack
	MessagePack* RequestMessagePack();

	// Release a message pack
	void ReleaseMessagePack(MessagePack* _messagePack);

	// Pack a given message into a pack
	bool PackMessage(NetworkMessage& _message, MessagePack* _pack);

	// Unpack a message pack
	std::vector<NetworkMessage> UnpackMessagePack(MessagePack* _pack);

	// Reset the current frame
	void ResetFrame();

protected:

	// Request message data
	bool RequestReservedMessageData(MessageData* _messageData, uint32_t _size)
	{
		// Set the required size
		//uint32_t requiredSize = sizeof(ObjectType);
		uint32_t requiredSize = pow2roundup(_size);
		
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
		_messageData->dataPtr = selectedDataSlice;
		_messageData->dataSize = requiredSize;

		return true;
	}

private:

	// Get total messages inside pack
	uint32_t GetTotalPackMessages(MessagePack* _pack);

	// Get the total data used for a given pack
	uint32_t GetTotalPackData(MessagePack* _pack);

	// Set the total messages inside a pack
	void SetPackTotalMessages(MessagePack* _pack, uint32_t _totalNumberMessages);

	// Set the total data used for a given pack
	void SetPackTotalData(MessagePack* _pack, uint32_t _totalNewData);

///////////////
// VARIABLES //
private: //////

	// The messaged reserved data
	unsigned char* m_MessagedReservedData;

	// The total message reserved data used
	uint32_t m_TotalReservedMessageDataUsed;

	// All the message packs
	SmallPackMessagePackList m_MessagePackList;
};

// SmallPack
SmallPackNamespaceEnd(SmallPack)