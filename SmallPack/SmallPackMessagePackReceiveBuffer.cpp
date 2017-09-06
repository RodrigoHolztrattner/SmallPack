////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackMessagePackReceiveBuffer.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SmallPackMessagePackReceiveBuffer.h"

#include <memory>

SmallPack::SmallPackMessagePackReceiveBuffer::SmallPackMessagePackReceiveBuffer()
{
	// Set the initial data
	m_WritingShouldAdvance = false;
	m_ReadingShouldAdvance = true;

	// Set the initial writing and reading buffers
	m_WritingBuffer = 0;
	m_ReadingBuffer = BufferingAmount - 1;
}

/*
SmallPack::SmallPackMessagePackReceiveBuffer::SmallPackMessagePackReceiveBuffer(const SmallPack::SmallPackMessagePackReceiveBuffer& other)
{
}
*/

SmallPack::SmallPackMessagePackReceiveBuffer::~SmallPackMessagePackReceiveBuffer()
{
}

SmallPack::MessagePackBufferList SmallPack::SmallPackMessagePackReceiveBuffer::GetPackList()
{
	// Check if we should advance
	if (m_ReadingShouldAdvance)
	{
		// Advance the reading buffer
		m_ReadingBuffer = (m_ReadingBuffer++)% BufferingAmount;
	}

	// Get the pack list
	SmallPack::MessagePackBufferList packList = m_MessagePackBuffers[m_ReadingBuffer];

	// Set the writing buffer to advance
	m_WritingShouldAdvance = true;

	return packList;
}

void SmallPack::SmallPackMessagePackReceiveBuffer::InsertPack(MessagePack* _messagePack)
{
	// Check if we should advance
	if (m_WritingShouldAdvance)
	{
		// Advance the writing buffer
		m_WritingShouldAdvance = (m_WritingShouldAdvance++) % BufferingAmount;
	}

	// Insert the pack itself
	MessagePack* oldNode = m_MessagePackBuffers[m_WritingBuffer].nodeMessagePack;
	m_MessagePackBuffers[m_WritingBuffer].nodeMessagePack = _messagePack;
	m_MessagePackBuffers[m_WritingBuffer].nodeMessagePack->nextPack = oldNode;

	// Set the reading buffer to advance
	m_ReadingShouldAdvance = true;
}