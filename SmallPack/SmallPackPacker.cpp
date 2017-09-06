////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackPacker.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SmallPackPacker.h"

SmallPack::SmallPackPacker::SmallPackPacker(SmallPackMessagePackList& _messagePackList) : m_MessagePackList(_messagePackList)
{
	// Set the initial data
	m_TotalReservedMessageDataUsed = 0;
	m_MessagedReservedData = nullptr;
}

/*
SmallPack::SmallPackPacker::SmallPackPacker(const SmallPack::SmallPackPacker& other)
{
}
*/

SmallPack::SmallPackPacker::~SmallPackPacker()
{
}

bool SmallPack::SmallPackPacker::Initialize()
{
	// Alloc the reserved data
	m_MessagedReservedData = new unsigned char[TotalReservedData];
	if (m_MessagedReservedData == nullptr)
	{
		// Allocation failed
		return false;
	}

	return true;
}

SmallPack::MessagePack* SmallPack::SmallPackPacker::RequestMessagePack()
{
	// Get a new message pack from our list
	MessagePack* newMessagePack = m_MessagePackList.RequestPack();

	// Update the total messages and the total pack data
	SetPackTotalMessages(newMessagePack, 0);
	SetPackTotalData(newMessagePack, 0); // total messages + total packet data used

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

bool SmallPack::SmallPackPacker::PackMessage(NetworkMessage& _message, MessagePack* _pack)
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
	_message.Serialize(_pack->GetData(), totalPackData);

	// Update the total messages and the total pack data
	SetPackTotalMessages(_pack, totalMessages + 1);
	SetPackTotalData(_pack, totalPackData);

	return true;
}

std::vector<SmallPack::NetworkMessage> SmallPack::SmallPackPacker::UnpackMessagePack(MessagePack* _pack)
{
	// Get the total number of messages inside this pack
	uint32_t totalMessages = GetTotalPackMessages(_pack);

	// Get the total data used for this pack
	uint32_t totalPackData = GetTotalPackData(_pack);

	// The vector we are going to use to store all messages
	std::vector<SmallPack::NetworkMessage> unpackedMessages;

	// The current data location
	uint32_t currentDataLocation = 0;

	// For each message
	for (int i = 0; i < totalMessages; i++)
	{
		NetworkMessage newMessage;

		// Request data for the message
		RequestReservedMessageData(&newMessage.messageData, MessagePackMaxData);

		// Deserialize the message
		newMessage.Deserialize(_pack->GetData(), currentDataLocation);

		// Add it to our unpacked array
		unpackedMessages.push_back(newMessage);
	}

	return unpackedMessages;
}

uint32_t SmallPack::SmallPackPacker::GetTotalPackMessages(MessagePack* _pack)
{
	return _pack->totalNumberMessages;
}

uint32_t SmallPack::SmallPackPacker::GetTotalPackData(MessagePack* _pack)
{
	return _pack->totalDataUsed;
}

void SmallPack::SmallPackPacker::SetPackTotalMessages(MessagePack* _pack, uint32_t _totalNumberMessages)
{
	// Set the total number of messages
	_pack->totalNumberMessages = _totalNumberMessages;
}

void SmallPack::SmallPackPacker::SetPackTotalData(MessagePack* _pack, uint32_t _totalNewData)
{
	// Set the total data
	_pack->totalDataUsed = _totalNewData;
}