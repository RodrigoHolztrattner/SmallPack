////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackMessagePackList.h
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

// We know the SmallPackMessagePackList, the SmallPackPacker and the SmallPackMessagePackReceiveBuffer classes
class SmallPackMessagePackList;
class SmallPackPacker;
class SmallPackMessagePackReceiveBuffer;

// The message pack
struct MessagePack
{
public:

private:

	// The SmallPackMessagePackList is a friend
	friend SmallPackMessagePackList;

	// The SmallPackPacker is a friend
	friend SmallPackPacker;

	// The SmallPackMessagePackReceiveBuffer is a friend
	friend SmallPackMessagePackReceiveBuffer;

public:

	// The message pack size
	static const uint32_t MessagePackSize = 1024;

public:

	// Copy data into this message pack
	void FillWithData(unsigned char* _data, uint32_t _size)
	{
		memcpy(data, _data, _size);
	}

	// Return this message pack data
	unsigned char* GetData()
	{
		return data;
	}

	// Return the total number of messages
	uint32_t GetTotalMessages()
	{
		return totalNumberMessages;
	}

	// Return the amount of data used
	uint32_t GetTotalDataUsed()
	{
		return totalDataUsed;
	}

	// Copy to bytestream
	uint32_t CopyToByteStream(unsigned char* _byteStream)
	{
		uint32_t position = 0;
		memcpy(_byteStream, &totalNumberMessages, sizeof(uint32_t)); position += sizeof(uint32_t);
		memcpy(_byteStream, &totalDataUsed, sizeof(uint32_t)); position += sizeof(uint32_t);
		memcpy(_byteStream, data, sizeof(unsigned char) * totalDataUsed); position += sizeof(unsigned char) * totalDataUsed;

		return position;
	}

	// Copy from bytestream
	void CopyFromByteStream(unsigned char* _byteStream, uint32_t _totalSize)
	{
		uint32_t position = 0;
		memcpy(&totalNumberMessages, _byteStream, sizeof(uint32_t)); position += sizeof(uint32_t);
		memcpy(&totalDataUsed, _byteStream, sizeof(uint32_t)); position += sizeof(uint32_t);
		memcpy(data, _byteStream, sizeof(unsigned char) * (_totalSize - position)); position += sizeof(unsigned char) * (_totalSize - position);
	}

protected:

	// The next pack ptr
	MessagePack* nextPack;

	// The tota number of messages
	uint32_t totalNumberMessages;

	// The total data used
	uint32_t totalDataUsed;

private:

	// The data itself
	unsigned char data[MessagePackSize];
};

////////////////////////////////////////////////////////////////////////////////
// Class name: SmallPackMessagePackList
////////////////////////////////////////////////////////////////////////////////
class SmallPackMessagePackList
{

public:
	SmallPackMessagePackList();
	SmallPackMessagePackList(const SmallPackMessagePackList&);
	~SmallPackMessagePackList();

public:

	// Initialize
	bool Initialize();

	// Perform a cleaning operation to optmize used memory
	void OptmizeUsedMemory();

	// Request a message pack
	MessagePack* RequestPack();

	// Release a message pack
	void ReleasePack(MessagePack* _pack);

private:

	// Get the next message pack from the given node
	MessagePack* GetNextPackNode(MessagePack* _node);

	// Set the message pack list ptr
	void SetPackNodePtr(MessagePack* _thisNode, MessagePack* _pointsToThisNode);

	// Alloc a new node
	MessagePack* AllocNewPack();

	// Dealloc a given node
	void DeallocPack(MessagePack* _node);

///////////////
// VARIABLES //
private: //////

	// The pack freelist
	MessagePack* m_MessagePackFreelist;

	// The pack counter (to keep track how many packs we have)
	uint32_t m_PackCounter;
};

// SmallPack
SmallPackNamespaceEnd(SmallPack)