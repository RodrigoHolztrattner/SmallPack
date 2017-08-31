// SmallPack.cpp : Defines the entry point for the console application.
//

#include "SmallPackConfig.h"
#include "SmallPackMessages.h"
#include "SmallPackPacker.h"
#include "SmallPackMessageComposer.h"
#include "SmallPackCommunicationChannel.h"
#include "SmallPackCommunicationController.h"

struct Vector
{
	Vector() {}
	Vector(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
	float x, y, z, w;
};

int main()
{
	SmallPack::SmallPackMessageComposer messageComposer;
	SmallPack::SmallPackPacker messagePacker;
	boost::asio::io_service ioService;

	std::vector<SmallPack::NetworkMessage> uunpackedMessages;
	uunpackedMessages.resize(2);

	// Initialize the IO service
	ioService.run();

	// Create the communication controller
	SmallPack::SmallPackCommunicationController communcationController(ioService);

	// Initialize the message packer
	bool result = messagePacker.Initialize();
	if (!result)
	{
		return false;
	}

	//
	//
	//

	// Request a new message pack
	SmallPack::MessagePack* newMessagePack = messagePacker.RequestMessagePack();
	if (newMessagePack == nullptr)
	{
		return false;
	}

	// Create a dummy vector
	Vector vec1(0.0, 0.5, 6.0, 1.0);
	Vector vec2(102323.9, 1.0, 0.0, 99.98);

	// Compose some messages
	SmallPack::NetworkMessage newMessage1;
	SmallPack::NetworkMessage newMessage2;

	// Create the first message
	result = messageComposer.Compose<Vector>(&messagePacker, SmallPack::Operation::Command, 0, vec1, newMessage1);
	if (!result)
	{
		return false;
	}

	// Create the second message
	result = messageComposer.Compose<Vector>(&messagePacker, SmallPack::Operation::StayAlive, 5, vec2, newMessage2);
	if (!result)
	{
		return false;
	}

	// Pack the messages
	result = messagePacker.PackMessage(newMessage1, newMessagePack);
	if (!result)
	{
		return false;
	}
	result = messagePacker.PackMessage(newMessage2, newMessagePack);
	if (!result)
	{
		return false;
	}

	// Unpack all messages
	std::vector<SmallPack::NetworkMessage> unpackedMessages = messagePacker.UnpackMessagePack(newMessagePack);

	// Get the unpacked object
	Vector unpackedVector1;
	Vector unpackedVector2;
	result = unpackedMessages[0].GetDataObject(unpackedVector1);
	if (!result)
	{
		return false;
	}
	result = unpackedMessages[1].GetDataObject(unpackedVector2);
	if (!result)
	{
		return false;
	}

	//
	//
	//

	// Initialize the communication controller so we can accept outside requests from other players
	result = communcationController.Initialize(2234);
	if (!result)
	{
		// Cannot initialize the communication controller
		exit(0);
	}

	// Start accepting incomming requests
	communcationController.Start();

    return 0;
}

