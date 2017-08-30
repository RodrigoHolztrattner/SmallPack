////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackPacker.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SmallPackPacker.h"

SmallPack::SmallPackPacker::SmallPackPacker()
{
	// Set the initial data
	m_TotalReservedMessageDataUsed = 0;
	m_MessagedReservedData = nullptr;
}

SmallPack::SmallPackPacker::SmallPackPacker(const SmallPack::SmallPackPacker& other)
{
}

SmallPack::SmallPackPacker::~SmallPackPacker()
{
}

bool SmallPack::SmallPackPacker::Initialize()
{
	// Alloc the reserved data
	m_MessagedReservedData = new unsigned char[NumberReservedDataMessages];
	if (m_MessagedReservedData == nullptr)
	{
		// Allocation failed
		return false;
	}

	// Initialize the message pack list
	bool result = m_MessagePackList.Initialize();
	if (!result)
	{
		return false;
	}

	return true;
}

SmallPack::MessagePack* SmallPack::SmallPackPacker::RequestMessagePack()
{
	// Get a new message pack from our list
	MessagePack* newMessagePack = m_MessagePackList.RequestPack();

	// Update the total messages and the total pack data
	SetPackTotalMessages(*newMessagePack, 0);
	SetPackTotalData(*newMessagePack, sizeof(uint32_t) * 2); // total messages + total packet data used

	return newMessagePack;
}

void SmallPack::SmallPackPacker::ReleaseMessagePack(MessagePack* _messagePack)
{
	// Release this message pack to our list
	m_MessagePackList.ReleasePack(_messagePack);
}

void SmallPack::SmallPackPacker::ResetFrame()
{
	// Reset the current reserved data used
	m_TotalReservedMessageDataUsed = 0;
}

bool SmallPack::SmallPackPacker::PackMessage(NetworkMessage& _message, MessagePack& _pack)
{
	// Get the total number of messages inside this pack
	uint32_t totalMessages = GetTotalPackMessages(_pack);

	// Get the total data used for this pack
	uint32_t totalPackData = GetTotalPackData(_pack);

	// Check if we can add this message to this pack
	if (totalPackData + _message.messageTotalSize >= MessagePackMaxData)
	{
		// No more space inside this pack
		return false;
	}

	// Serialize the new message
	_message.Serialize(_pack.GetData(), totalPackData);

	// Update the total messages and the total pack data
	SetPackTotalMessages(_pack, totalMessages + 1);
	SetPackTotalData(_pack, totalPackData);

	return true;
}

std::vector<SmallPack::NetworkMessage> SmallPack::SmallPackPacker::UnpackMessagePack(MessagePack& _pack)
{
	// Get the total number of messages inside this pack
	uint32_t totalMessages = GetTotalPackMessages(_pack);

	// Get the total data used for this pack
	uint32_t totalPackData = GetTotalPackData(_pack);

	// The vector we are going to use to store all messages
	std::vector<NetworkMessage> unpackedMessages;

	// The current data location
	uint32_t currentDataLocation = sizeof(uint32_t) * 2; // total messages + total packet data used

	// For each message
	for (int i = 0; i < totalMessages; i++)
	{
		NetworkMessage newMessage;

		// Request data for the message
		RequestReservedMessageData(&newMessage.messageData);

		// Deserialize the message
		newMessage.Deserialize(_pack.GetData(), currentDataLocation);

		// Add it to our unpacked array
		unpackedMessages.push_back(newMessage);
	}

	return unpackedMessages;
}

uint32_t SmallPack::SmallPackPacker::GetTotalPackMessages(MessagePack& _pack)
{
	// Get the pack data
	unsigned char* packData = _pack.GetData();

	// Cast to uint32_t array
	uint32_t* uintArray = (uint32_t*)packData;

	// Check the total number of messages inside this pack (this is the first uint32_t data)
	return uintArray[0];
}

uint32_t SmallPack::SmallPackPacker::GetTotalPackData(MessagePack& _pack)
{
	// Get the pack data
	unsigned char* packData = _pack.GetData();

	// Cast to uint32_t array
	uint32_t* uintArray = (uint32_t*)packData;

	// Check the total data used by this pack (this is the second uint32_t data)
	return uintArray[1];
}

void SmallPack::SmallPackPacker::SetPackTotalMessages(MessagePack& _pack, uint32_t _totalNumberMessages)
{
	// Get the pack data
	unsigned char* packData = _pack.GetData();

	// Cast to uint32_t array
	uint32_t* uintArray = (uint32_t*)packData;

	// Increment the total number of messages
	uintArray[0] += _totalNumberMessages;
}

void SmallPack::SmallPackPacker::SetPackTotalData(MessagePack& _pack, uint32_t _totalNewData)
{
	// Get the pack data
	unsigned char* packData = _pack.GetData();

	// Cast to uint32_t array
	uint32_t* uintArray = (uint32_t*)packData;

	// Set the total data used
	uintArray[1] = _totalNewData;
}