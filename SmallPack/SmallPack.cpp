// SmallPack.cpp : Defines the entry point for the console application.
//

#include "SmallPackConfig.h"
#include "SmallPackMessages.h"
#include "SmallPackPacker.h"
#include "SmallPackMessageComposer.h"
#include "CommunicationChannel/SmallPackCommunicationChannel.h"
#include "CommunicationChannel\SmallPackCommunicationChannelNonReliable.h"
#include "CommunicationChannel\SmallPackCommunicationChannelReliable.h"
#include "SmallPackCommunicationCenter.h"
#include "SmallPackMessagePackList.h"

#include "Server\SmallPackServerCommunicationCenter.h"

#include <iostream>

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

	// Initialize the message packer
	bool result = messagePacker.Initialize();
	if (!result)
	{
		return false;
	}

	// Digite a opcao
	std::cout << "Digite a opcao: ";
	int op;
	std::cin >> op;
	std::cout << std::endl;

	if (!op)
	{
		// Create the communication center
		SmallPack::SmallPackCommunicationCenter communcationCenter(ioService);

		// Initialize the communication controller so we can accept outside requests from other players
		result = communcationCenter.Initialize("127.0.0.1", "2234", 2235);
		if (!result)
		{
			// Cannot initialize the communication controller
			exit(0);
		}

		// Compose a dummy message
		SmallPack::NetworkMessage newMessage;
		int a = 2;
		messageComposer.Compose(&messagePacker, SmallPack::Operator::System, 0, 0, a, newMessage);
		SmallPack::MessagePack* messagePack
		messagePacker.PackMessage(newMessage, )
		// Our game loop
		while (true)
		{
			// Do the update for our communication center
			communcationCenter.Update(&messagePackList, &messagePacker, 0, 0);
		}
	}
	else
	{
		// Create the communication center
		SmallPack::Server::SmallPackServerCommunicationCenter communcationCenter(ioService);

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
			communcationCenter.Update(&messagePackList, &messagePacker, 0, 0);
		}
	}


    return 0;
}

