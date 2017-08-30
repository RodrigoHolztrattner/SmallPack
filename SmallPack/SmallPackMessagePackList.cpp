////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackMessagePackList.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SmallPackMessagePackList.h"

#include <memory>

SmallPack::SmallPackMessagePackList::SmallPackMessagePackList()
{
	// Set the initial data
	m_PackCounter = 0;
	m_MessagePackFreelist = nullptr;
}

SmallPack::SmallPackMessagePackList::SmallPackMessagePackList(const SmallPack::SmallPackMessagePackList& other)
{
}

SmallPack::SmallPackMessagePackList::~SmallPackMessagePackList()
{
}

bool SmallPack::SmallPackMessagePackList::Initialize()
{
	return true;
}

void SmallPack::SmallPackMessagePackList::OptmizeUsedMemory()
{
	// For each pack inside the freelist
	MessagePack* currentNode = m_MessagePackFreelist;
	while (currentNode != nullptr)
	{
		// Get the next node
		MessagePack* nextNode = GetNextPackNode(currentNode);

		// Delete the pack data
		DeallocPack(currentNode);

		// Set the new current node
		currentNode = nextNode;
	}
}

SmallPack::MessagePack* SmallPack::SmallPackMessagePackList::RequestPack()
{
	// Check if we have any free pack
	if (m_MessagePackFreelist != nullptr)
	{
		// Get the current node
		MessagePack* currentNode = m_MessagePackFreelist;

		// Set the new freelist root
		m_MessagePackFreelist = GetNextPackNode(m_MessagePackFreelist);

		return currentNode;
	}

	// Alloc a new node
	MessagePack* newNode = AllocNewPack();

	return newNode;
}

void SmallPack::SmallPackMessagePackList::ReleasePack(MessagePack* _pack)
{
	// Make the released pack points to the current freelist top node
	SetPackNodePtr(_pack, m_MessagePackFreelist);

	// Set the new freelist top node
	m_MessagePackFreelist = _pack;
}

SmallPack::MessagePack* SmallPack::SmallPackMessagePackList::GetNextPackNode(MessagePack* _node)
{
	// Cast to pointer type
	MessagePack* packPtr = (MessagePack*)_node->data;

	return &packPtr[-1];
}

void SmallPack::SmallPackMessagePackList::SetPackNodePtr(MessagePack* _thisNode, MessagePack* _pointsToThisNode)
{
	// Cast to pointer type
	MessagePack* packPtr = (MessagePack*)_thisNode->data;

	// Get the node ptr
	MessagePack** nodePtr = (MessagePack**)&packPtr[-1];

	// Set the new ptr
	*nodePtr = _pointsToThisNode;
}

SmallPack::MessagePack* SmallPack::SmallPackMessagePackList::AllocNewPack()
{
	// Increment the pack counter
	m_PackCounter++;

	// Alloc new data
	unsigned char* newData = new unsigned char[GetAllocSize()];

	// Cast to pack ptr
	MessagePack* packPtr = (MessagePack*)newData;

	return &packPtr[1];
}

void SmallPack::SmallPackMessagePackList::DeallocPack(MessagePack* _node)
{
	// Cast to pointer type
	MessagePack* packPtr = (MessagePack*)_node->data;

	// Get the real ptr
	unsigned char* realPtr = (unsigned char*)&packPtr[-1];

	// Delete the data
	delete[] realPtr;

	// Subtract one from the pack counter
	m_PackCounter--;
}

uint32_t SmallPack::SmallPackMessagePackList::GetAllocSize()
{
	return MessagePack::MessagePackSize + sizeof(MessagePack*);
}