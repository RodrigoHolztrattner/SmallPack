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

// We know the SmallPackMessagePackList class
class SmallPackMessagePackList;

// The message pack
struct MessagePack
{
private:

	// The SmallPackMessagePackList is a friend
	friend SmallPackMessagePackList;

protected:

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

private:

	// The data itself
	unsigned char* data;
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

	// Calculate the total size that we should alloc for a new pack
	uint32_t GetAllocSize();

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