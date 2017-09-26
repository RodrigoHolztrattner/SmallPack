// SmallPack.cpp : Defines the entry point for the console application.
//

#include "SmallPackConfig.h"
#include "SmallPackMessages.h"
#include "SmallPackPacker.h"
#include "SmallPackMessageComposer.h"
#include "Client\SmallPackClientCommunicationChannel.h"
#include "Client\SmallPackClientCommunicationChannelNonReliable.h"
#include "Client\SmallPackClientCommunicationChannelReliable.h"
#include "Client\SmallPackClientCommunicationCenter.h"
#include "SmallPackMessagePackList.h"

#include "Server\SmallPackServerCommunicationCenter.h"

#include <iostream>
#include <ctime>

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
	clock_t initialTime = clock();

	// Initialize the IO service
	ioService.run();

	// Initialize the message packer
	bool result = messagePacker.Initialize();
	if (!result)
	{
		return false;
	}

	// Escolha entre server e cliente
	std::cout << "Digite 0 para executar como cliente e 1 para executar como server: ";
	int op;
	std::cin >> op;
	std::cout << std::endl;

	// Verifica qual opção foi escolhida
	if (op == 0)
	{
		std::string serverIp;
		std::string serverPort;
		uint32_t selfPort;

		// Create the communication center
		SmallPack::Client::SmallPackCommunicationCenter communcationCenter(ioService);

		// Digite o ip do servidor
		std::cout << "    - Digite o ip do servidor: ";
		std::cin >> serverIp;

		// Digite a porta do servidor
		std::cout << std::endl << "    - Digite a porta do servidor: ";
		std::cin >> serverPort;

		// Digite a porta usada pelo proprio cliente
		std::cout << std::endl << "    - Digite a porta a ser utilizada por este cliente: ";
		std::cin >> selfPort;
		std::cout << std::endl << std::endl;

		// Initialize the communication controller so we can accept outside requests from other players
		result = communcationCenter.Initialize(serverIp.c_str(), serverPort.c_str() , selfPort);
		if (!result)
		{
			// Cannot initialize the communication controller
			exit(0);
		}

		// Compose a dummy message
		SmallPack::NetworkMessage newMessage;
		int dummyData = 5;
		messageComposer.Compose(&messagePacker, SmallPack::Operator::System, 0, 0, 22223, 1, dummyData, newMessage);

		// Queue the message to be sent
		communcationCenter.GetServerCommunicationChannel()->QueueMessage(&newMessage);

		// Get the current time
		clock_t currentTime = clock();
		float acc = 0;

		// Our game loop
		while (true)
		{
			// Time
			clock_t end = clock();

			// Update the elapsed time
			double elapsed_secs = double(end - currentTime) / CLOCKS_PER_SEC;
			currentTime = end;

			// Do the update for our communication center
			std::vector<SmallPack::NetworkMessage> messages = communcationCenter.Update(&messagePackList, &messagePacker, end - initialTime, elapsed_secs);

			//
			//
			//
			acc += elapsed_secs;
			if (acc > 1)
			{
				acc = 0;

				// Queue the message to be sent
				communcationCenter.GetServerCommunicationChannel()->QueueMessage(&newMessage);

				std::cout << "Message sent to the server" << std::endl;
			}

			// Commit
			communcationCenter.CommitMessages(&messagePacker, &messageComposer, end - initialTime);

			// Reset the message packer frame
			messagePacker.ResetFrame();		
		}
	}
	else
	{
		uint32_t selfPort;

		// Digite a porta usada pelo proprio server
		std::cout << std::endl << "    - Digite a porta a ser utilizada por este server: ";
		std::cin >> selfPort;
		std::cout << std::endl << std::endl;

		// Create the communication center
		SmallPack::Server::SmallPackCommunicationCenter communcationCenter(ioService);

		// Initialize the communication controller so we can accept outside requests from other players
		result = communcationCenter.Initialize(selfPort);
		if (!result)
		{
			// Cannot initialize the communication controller
			exit(0);
		}

		// Get the current time
		clock_t currentTime = clock();

		// Our game loop
		while (true)
		{
			// Time
			clock_t end = clock();

			// Update the elapsed time
			double elapsed_secs = double(end - currentTime) / CLOCKS_PER_SEC;
			currentTime = end;

			// Do the update for our communication center
			std::vector<SmallPack::NetworkMessage> messages = communcationCenter.Update(&messagePackList, &messagePacker, end - initialTime, elapsed_secs);

			// Commit
			communcationCenter.CommitMessages(&messagePacker, &messageComposer, end - initialTime);

			// Reset the message packer frame
			messagePacker.ResetFrame();
		}
	}

    return 0;
}

