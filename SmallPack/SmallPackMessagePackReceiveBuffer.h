////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackMessagePackReceiveBuffer.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "SmallPackConfig.h"

#include "SmallPackMessagePackList.h"
#include <array>

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

// The message pack buffer list
struct MessagePackBufferList
{
	// The amount of message packs inside this buffer list
	uint32_t amountPacks;

	// The first message pack
	MessagePack* nodeMessagePack;
};

////////////////////////////////////////////////////////////////////////////////
// Class name: SmallPackMessagePackReceiveBuffer
////////////////////////////////////////////////////////////////////////////////
class SmallPackMessagePackReceiveBuffer
{
	// The buffering amount
	static const uint32_t BufferingAmount = 3;

public:
	SmallPackMessagePackReceiveBuffer();
//	SmallPackMessagePackReceiveBuffer(const SmallPackMessagePackReceiveBuffer&)
	~SmallPackMessagePackReceiveBuffer();

public:

	// Return the buffer list from the current reading buffer
	MessagePackBufferList GetPackList();

	// Insert a pack into the current writing buffer
	void InsertPack(MessagePack* _messagePack);

private:

///////////////
// VARIABLES //
private: //////

	// The pack buffers
	std::array<MessagePackBufferList, BufferingAmount> m_MessagePackBuffers;

	// The buffer we are writing and the one we are reading
	uint32_t m_WritingBuffer;
	uint32_t m_ReadingBuffer;

	// If the writing or reading buffer should advance
	bool m_WritingShouldAdvance;
	bool m_ReadingShouldAdvance;
};

// SmallPack
SmallPackNamespaceEnd(SmallPack)