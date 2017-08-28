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
	m_MessagedReservedData = new unsigned char[TotalReservedData];
	if (!m_MessagedReservedData)
	{
		// Allocation failed
		return false;
	}

	return true;
}