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
	return _node->nextPack;
}

void SmallPack::SmallPackMessagePackList::SetPackNodePtr(MessagePack* _thisNode, MessagePack* _pointsToThisNode)
{
	// Set the new ptr
	_thisNode->nextPack = _pointsToThisNode;
}

SmallPack::MessagePack* SmallPack::SmallPackMessagePackList::AllocNewPack()
{
	// Increment the pack counter
	m_PackCounter++;

	// Create the new pack
	MessagePack* packPtr = new MessagePack();
	packPtr->nextPack = nullptr;

	return packPtr;
}

void SmallPack::SmallPackMessagePackList::DeallocPack(MessagePack* _node)
{
	// Delete the pack
	delete[] _node;

	// Subtract one from the pack counter
	m_PackCounter--;
}