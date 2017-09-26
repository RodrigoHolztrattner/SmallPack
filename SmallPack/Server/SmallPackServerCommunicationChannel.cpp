////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackServerCommunicationChannel.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SmallPackServerCommunicationChannel.h"
#include <cstdio>
#include <iostream>
#include <stdlib.h>     /* atoi */

using namespace boost::asio::ip;

SmallPack::Server::SmallPackServerCommunicationChannel::SmallPackServerCommunicationChannel(boost::asio::io_service& _ioService) : SmallPack::SmallPackCommunicationChannel(_ioService)
{
	// Set the initial data
	// ...
}

/*
SmallPack::SmallPackServerCommunicationChannel::SmallPackServerCommunicationChannel(const SmallPack::SmallPackServerCommunicationChannel& other)
{
}
*/

SmallPack::Server::SmallPackServerCommunicationChannel::~SmallPackServerCommunicationChannel()
{
}

bool SmallPack::Server::SmallPackServerCommunicationChannel::Initialize(boost::asio::ip::address _senderAddress, uint32_t _port)
{
	/*
	// Resolve the ip version with the given host and port to get our iterator
	udp::resolver resolver(m_ChannelData.ioService);
	udp::resolver::query query(udp::v4(), _senderAddress.to_string().c_str(), std::to_string(_port).c_str());
	m_ChannelData.iterator = resolver.resolve(query);
	*/
	
	// Resolve the ip version with the given host and port to get our iterator
	udp::resolver resolver(m_ChannelData.ioService);
	udp::resolver::query query(udp::v4(), _senderAddress.to_string().c_str(), std::to_string(_port).c_str());
	m_ChannelData.endpoint = *resolver.resolve(query);
	m_ChannelData.socket.open(udp::v4());

	a parte de recebimento de mensagem do cliente não esta funcionando
		OU
		a parte de envio de mensagem do server não está funcionando

	// Set the channel data
	m_ChannelData.address = _senderAddress;
	m_ChannelData.port = _port;
	
	return true;
}

void SmallPack::Server::SmallPackServerCommunicationChannel::FrameUpdate(uint32_t _currentTime, float _timeElapsed)
{
	// Call the parent method
	SmallPack::SmallPackCommunicationChannel::FrameUpdate(_currentTime, _timeElapsed);
}

void SmallPack::Server::SmallPackServerCommunicationChannel::ProcessSystemMessage(SmallPackPacker* _packer, NetworkMessage* _message, uint32_t _currentTime)
{
	// Call the parent method
	SmallPack::SmallPackCommunicationChannel::ProcessSystemMessage(_packer, _message, _currentTime);

	// Log
	std::cout << "Received client message!" << std::endl;

	/*
	// Look for the ping controll message (and if we are expecting a ping message)
	if (_message->messageHeader.IsFromType(SystemCommands::Ping) && m_PingInfo.expectingPing)
	{
		// Get the ping data
		CommandPing pingData;
		_message->GetDataObject(pingData);

		// Process this ping command
		ProcessPingCommand(pingData, _currentTime);
	}
	*/
}