// SmallPack.cpp : Defines the entry point for the console application.
//

#include "SmallPackConfig.h"
#include "SmallPackMessages.h"
#include "SmallPackPacker.h"
#include "SmallPackMessageComposer.h"
#include "SmallPackCommunicationChannel.h"
#include "SmallPackCommunicationCenter.h"
#include "SmallPackMessagePackList.h"

struct Vector
{
	Vector() {}
	Vector(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
	float x, y, z, w;
};

int main()
{
	SmallPack::SmallPackMessageComposer messageComposer;
	SmallPack::SmallPackMessagePackList messagePackList;
	SmallPack::SmallPackPacker messagePacker(messagePackList);
	boost::asio::io_service ioService;

	// Initialize the IO service
	ioService.run();

	// Create the communication center
	SmallPack::SmallPackCommunicationCenter communcationCenter(ioService);

	// Initialize the message packer
	bool result = messagePacker.Initialize();
	if (!result)
	{
		return false;
	}

	// Initialize the communication controller so we can accept outside requests from other players
	result = communcationCenter.Initialize(2234);
	if (!result)
	{
		// Cannot initialize the communication controller
		exit(0);
	}

	// Our game loop
	while (true)
	{
		// Do the update for our communication center
		communcationCenter.Update(&messagePackList,& messagePacker, 0);
	}

    return 0;
}

