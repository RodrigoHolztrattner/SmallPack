////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackClientCommunicationChannel.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SmallPackClientCommunicationChannel.h"
#include <cstdio>
#include <iostream>
#include <stdlib.h>     /* atoi */

using namespace boost::asio::ip;

SmallPack::Client::SmallPackClientCommunicationChannel::SmallPackClientCommunicationChannel(boost::asio::io_service& _ioService) : m_ChannelData(_ioService)
{
	// Set the initial data
	m_PingInfo.ping = PingInfo::InitialPing;
	m_PingInfo.expectingPing = false;
	m_PingInfo.lastPingElapsedTime = 0;
	m_PingInfo.pingMessageIdentifier = 0;
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
	// Check for ping re-send
	// ...
}

void SmallPack::Client::SmallPackClientCommunicationChannel::ProcessSystemMessage(SmallPackPacker* _packer, NetworkMessage* _message, uint32_t _currentTime)
{
	// Look for the ping controll message (and if we are expecting a ping message)
	if (_message->messageHeader.IsFromType(SystemCommands::Ping) && m_PingInfo.expectingPing)
	{
		// Get the ping data
		CommandPing pingData;
		_message->GetDataObject(pingData);

		// Process this ping command
		ProcessPingCommand(pingData, _currentTime);
	}
}

void SmallPack::Client::SmallPackClientCommunicationChannel::QueueMessage(SmallPack::MessagePack* _messagePack)
{
	// Insert the message pack into our send queue
	m_SendQueue.push_back(_messagePack);
}

void SmallPack::Client::SmallPackClientCommunicationChannel::SendMessagePack(SmallPack::MessagePack* _messagePack)
{
	const uint32_t bufferSize = 2048;
	unsigned char data[bufferSize];

	// Copy the message pack to our bytestream
	uint32_t length = _messagePack->CopyToByteStream(data);

	// Send the message pack through this channel
	m_ChannelData.socket.send_to(boost::asio::buffer(data, length), m_ChannelData.endpoint);

	// TODO: Precido tomar cuidado pois devo deixar o pack num queue de envio MAS lembrar que caso estejamos usando o reliable channel
	// não podemos liberar esse pack depois do envio já que ele ainda é armazenado para confirmacao
}

void SmallPack::Client::SmallPackClientCommunicationChannel::ProcessPingCommand(CommandPing _pingData, uint32_t _currentTime)
{
	// Check the ping identifier
	if (m_PingInfo.pingMessageIdentifier == _pingData.pingIdentifier)
	{
		// Set the ping data
		m_PingInfo.ping = _currentTime - m_PingInfo.lastPingElapsedTime;
		m_PingInfo.expectingPing = false;
	}
}

bool SmallPack::Client::SmallPackClientCommunicationChannel::IsHost(boost::asio::ip::address _address, uint32_t _port)
{
	if (m_ChannelData.address == _address && m_ChannelData.port == _port)
	{
		return true;
	}

	return false;
}