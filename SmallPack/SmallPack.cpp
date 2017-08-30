// SmallPack.cpp : Defines the entry point for the console application.
//

#include "SmallPackConfig.h"
#include "SmallPackMessages.h"
#include "SmallPackPacker.h"
#include "SmallPackMessageComposer.h"
#include "SmallPackCommunicationChannel.h"
#include "SmallPackCommunicationController.h"

int main()
{
	boost::asio::io_service ioService;
	ioService.run();

	SmallPack::SmallPackCommunicationController communcationController(ioService);
	SmallPack::SmallPackPacker messagePacker;

	// Initialize the communication controller so we can accept outside requests from other players
	bool result = communcationController.Initialize(2234);
	if (!result)
	{
		// Cannot initialize the communication controller
		exit(0);
	}

	// Initialize the message packer
	result = messagePacker.Initialize();
	if (!result)
	{
		// ...
	}

	// Start accepting incomming requests
	communcationController.Start();

    return 0;
}

