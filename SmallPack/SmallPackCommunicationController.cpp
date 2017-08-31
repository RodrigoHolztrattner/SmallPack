////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackCommunicationController.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SmallPackCommunicationController.h"

using namespace boost::asio::ip;

SmallPack::SmallPackCommunicationController::SmallPackCommunicationController(boost::asio::io_service& _ioService) : m_ioService(_ioService), m_ConnectionSocket(_ioService)
{
	// Set the initial data
	m_ListenForClientRequests = false;
}

/*
SmallPack::SmallPackCommunicationController::SmallPackCommunicationController(const SmallPack::SmallPackCommunicationController& other) : m_ioService(other.m_ioService), m_ConnectionSocket(other.m_ioService)
{
}
*/

SmallPack::SmallPackCommunicationController::~SmallPackCommunicationController()
{
}

bool SmallPack::SmallPackCommunicationController::Initialize(uint16_t _port)
{
	// Set the current port
	m_CurrentPort = _port;

	return true;
}

void SmallPack::SmallPackCommunicationController::Start()
{
	// Set that we should list for incomming connections
	m_ListenForClientRequests = true;

	// Start our side server
	//m_RequestListener = std::thread(&SmallPackCommunicationController::ListeningForRequests, this);
	ListeningForRequests();

}

void SmallPack::SmallPackCommunicationController::Stop()
{
	//
	m_ListenForClientRequests = false;
}

void SmallPack::SmallPackCommunicationController::ListeningForRequests()
{
	udp::socket sock(m_ioService, udp::endpoint(udp::v4(), m_CurrentPort));

	while(m_ListenForClientRequests)
	{
		#define max_length 1024
		char data[max_length];
		udp::endpoint sender_endpoint;
		size_t length = sock.receive_from(boost::asio::buffer(data, max_length), sender_endpoint);
		sock.send_to(boost::asio::buffer(data, length), sender_endpoint);


	}
}

bool SmallPack::SmallPackCommunicationController::BroadcastMessageToAllClients(NetworkMessage& _message)
{
	return true;
}