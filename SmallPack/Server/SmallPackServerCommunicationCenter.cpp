////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackServerCommunicationCenter.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SmallPackServerCommunicationCenter.h"

#include "../SmallPackPacker.h"
#include <iostream>

using namespace boost::asio::ip;

SmallPack::Server::SmallPackServerCommunicationCenter::SmallPackServerCommunicationCenter(boost::asio::io_service& _ioService) : SmallPack::SmallPackCommunicationCenter(_ioService)
{
	// Set the initial data
	// ...
}

SmallPack::Server::SmallPackServerCommunicationCenter::~SmallPackServerCommunicationCenter()
{
}

bool SmallPack::Server::SmallPackServerCommunicationCenter::Initialize(uint16_t _selfPort)
{
	// Set the current port
	m_ControllerData.currentPort = _selfPort;

	// Set our authentication token
	m_ControllerData.authenticationToken = ServerAuthenticationToken;

	// Prepare our socket
	m_ControllerData.connectionSocket = udp::socket(m_ControllerData.ioService, udp::endpoint(udp::v4(), m_ControllerData.currentPort));

	// Log
	std::cout << "Listenning on port: " << _selfPort << std::endl;

	//
	//
	//

	CreateClientCommunicationChannel(boost::asio::ip::address::from_string("127.0.0.1"), 22223);

	return true;
}

bool SmallPack::Server::SmallPackServerCommunicationCenter::CreateClientCommunicationChannel(boost::asio::ip::address _senderAddress, uint32_t _port)
{
	// Get the sender communication channel
	SmallPackCommunicationChannel* communicationChannel = GetSenderCommunicationChannel(_senderAddress, _port, true);
	if (communicationChannel == nullptr)
	{
		return false;
	}

	return true;
}

SmallPack::SmallPackCommunicationChannel* SmallPack::Server::SmallPackServerCommunicationCenter::CreateCommunicationChannel(boost::asio::ip::address _senderAddress, uint32_t _port)
{
	return new SmallPackCommunicationChannel(m_ControllerData.ioService);
}