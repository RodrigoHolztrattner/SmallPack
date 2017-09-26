////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackClientCommunicationChannel.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SmallPackClientCommunicationChannel.h"
#include <cstdio>
#include <iostream>
#include <stdlib.h>     /* atoi */

using namespace boost::asio::ip;

SmallPack::Client::SmallPackClientCommunicationChannel::SmallPackClientCommunicationChannel(boost::asio::io_service& _ioService) : SmallPack::SmallPackCommunicationChannel(_ioService)
{
	// Set the initial data
	// ...
}

/*
SmallPack::SmallPackClientCommunicationChannel::SmallPackClientCommunicationChannel(const SmallPack::SmallPackClientCommunicationChannel& other)
{
}
*/

SmallPack::Client::SmallPackClientCommunicationChannel::~SmallPackClientCommunicationChannel()
{
}

bool SmallPack::Client::SmallPackClientCommunicationChannel::Initialize(const char* _host, const char* _port)
{
	/*
	// Resolve the ip version with the given host and port to get our iterator
	udp::resolver resolver(m_ChannelData.ioService);
	udp::resolver::query query(udp::v4(), _senderAddress.to_string().c_str(), std::to_string(_port).c_str());
	m_ChannelData.iterator = resolver.resolve(query);
	*/

	// Resolve the ip version with the given host and port to get our iterator
	udp::resolver resolver(m_ChannelData.ioService);
	udp::resolver::query query(udp::v4(), _host, _port);
	m_ChannelData.endpoint = *resolver.resolve(query);
	m_ChannelData.socket.open(udp::v4());

	// Set the channel data
	m_ChannelData.address.from_string(_host);
	m_ChannelData.port = atoi(_port);

	// Server config done
	std::cout << "Server connection config stabilished!" << std::endl;

	return true;
}

void SmallPack::Client::SmallPackClientCommunicationChannel::FrameUpdate(uint32_t _currentTime, float _timeElapsed)
{
	// Call the parent method
	SmallPack::SmallPackCommunicationChannel::FrameUpdate(_currentTime, _timeElapsed);
}

void SmallPack::Client::SmallPackClientCommunicationChannel::ProcessSystemMessage(SmallPackPacker* _packer, NetworkMessage* _message, uint32_t _currentTime)
{
	// Call the parent method
	SmallPack::SmallPackCommunicationChannel::ProcessSystemMessage(_packer, _message, _currentTime);

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