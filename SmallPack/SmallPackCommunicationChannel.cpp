////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackCommunicationChannel.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SmallPackCommunicationChannel.h"
#include <cstdio>
#include <iostream>
#include <stdlib.h>     /* atoi */
#include "SmallPackPacker.h"
#include "SmallPackMessageComposer.h"

using namespace boost::asio::ip;

SmallPack::SmallPackCommunicationChannel::SmallPackCommunicationChannel(boost::asio::io_service& _ioService, bool _canExpire) : m_ChannelData(_ioService)
{
	// Set the initial data
	m_PingInfo.ping = PingInfo::InitialPing;
	m_PingInfo.expectingPing = false;
	m_PingInfo.receivedRequestedPing = false;
	m_PingInfo.lastPingTime = 0;
	m_PingInfo.pingExpectedIdentifier = 0;
	m_MessagePackSendList = nullptr;
	m_AuthenticationToken = 0;
	m_CanExpire = _canExpire;
}

/*
SmallPack::SmallPackCommunicationChannel::SmallPackCommunicationChannel(const SmallPack::SmallPackCommunicationChannel& other)
{
}
*/

SmallPack::SmallPackCommunicationChannel::~SmallPackCommunicationChannel()
{
}

bool SmallPack::SmallPackCommunicationChannel::Initialize(const char* _host, const char* _port, uint32_t _answerPort, uint32_t _authenticationToken)
{
	// Resolve the ip version with the given host and port to get our iterator
	udp::resolver resolver(m_ChannelData.ioService);
	udp::resolver::query query(udp::v4(), _host, _port);
	m_ChannelData.endpoint = *resolver.resolve(query);
	m_ChannelData.socket.open(udp::v4());

	// Set the channel data
	m_ChannelData.address = boost::asio::ip::address::from_string(_host);;
	m_ChannelData.port = atoi(_port);

	// Set the answer port
	SetAnswerPort(_answerPort);

	// Set the authentication token
	SetAuthenticationToken(_authenticationToken);

	return true;
}

void SmallPack::SmallPackCommunicationChannel::FrameUpdate(uint32_t _currentTime, float _timeElapsed)
{
	// Check for ping request
	if (!m_PingInfo.expectingPing && _currentTime - m_PingInfo.lastPingTime >= PingRequestInterval)
	{
		// Set that we need a new ping message
		m_PingInfo.expectingPing = true;

		// Log
		// std::cout << "We are now expecting a ping message!" << std::endl;
	}

	// Check if our ping request timed out
	// ...
}

void SmallPack::SmallPackCommunicationChannel::ProcessChannelInternalMessage(SmallPackPacker* _packer, NetworkMessage* _message, uint32_t _currentTime)
{
	// Do nothing
}

void SmallPack::SmallPackCommunicationChannel::ProcessPingAnswer(SmallPackPacker* _packer, NetworkMessage* _message, uint32_t _currentTime)
{
	// Check if we are expecting a ping message
	if (m_PingInfo.expectingPing)
	{
		// Set the ping
		m_PingInfo.ping = _currentTime - m_PingInfo.lastRequestTime;

		// Set the last ping time
		m_PingInfo.lastPingTime = _currentTime;

		// Ok we found our ping
		m_PingInfo.expectingPing = false;

		// Increment our expected identifier
		m_PingInfo.pingExpectedIdentifier++;

		// Log
		std::cout << " - Received ping answer from port: " << GetPort() << " - ping: " << m_PingInfo.ping << std::endl;
	}
}

void SmallPack::SmallPackCommunicationChannel::QueueMessage(SmallPack::NetworkMessage* _message)
{
	// Reset the message flags
	_message->messageHeader.messageFlags = 0;

	// Insert the message into our send queue
	m_SendQueue.push_back(_message);
}

void SmallPack::SmallPackCommunicationChannel::CommitQueueMessage(SmallPackPacker* _packer, uint32_t _currentTime)
{
	// Process the ping functionality
	ProcessPingFunctionality(_packer, _currentTime);

	// For each queued message pack
	for (int i = 0; i < m_SendQueue.size(); i++)
	{
		// Pack this message
		PackMessage(*m_SendQueue[i], _packer);
	}

	// Clear the message pack array
	m_SendQueue.clear();

	// For each message pack inside our send list
	while (m_MessagePackSendList != nullptr)
	{
		// Send this message pack
		SendMessagePack(m_MessagePackSendList);

		// Get the next message pack
		MessagePack* nextMessagePack = m_MessagePackSendList->nextPack;

		// Process the sent message pack
		ProcessSentMessagePack(_packer, m_MessagePackSendList, _currentTime);

		// Set the next message pack
		m_MessagePackSendList = nextMessagePack;
	}
}

void SmallPack::SmallPackCommunicationChannel::ProcessSentMessagePack(SmallPackPacker* _packer, MessagePack* _messagePack, uint32_t _currentTime)
{
	// Release this message pack
	_packer->ReleaseMessagePack(m_MessagePackSendList);
}

void SmallPack::SmallPackCommunicationChannel::RequestPing()
{
	// Log
	// std::cout << " - Received ping request!" << std::endl;

	// Set the ping info
	m_PingInfo.receivedRequestedPing = true;
}

void SmallPack::SmallPackCommunicationChannel::SetAuthenticationToken(uint32_t _token)
{
	m_AuthenticationToken = _token;
}

void SmallPack::SmallPackCommunicationChannel::SetCanExpire()
{
	m_CanExpire = true;
}

void SmallPack::SmallPackCommunicationChannel::ResetExpireTimer()
{
	m_ExpireTime = ExpireTimeAmount;
}

bool SmallPack::SmallPackCommunicationChannel::ShouldDelete(uint32_t _currentTime, float _timeElapsed)
{
	// If this channel can expire
	if (m_CanExpire)
	{
		// Reduce the expire time
		m_ExpireTime -= _timeElapsed;

		// Check if we expired
		if (m_ExpireTime <= 0)
		{
			// We expired
			return true;
		}
	}

	// Check if we timed out
	if (_currentTime - m_PingInfo.lastPingTime >= TimeOutAmount)
	{
		// We timed out
		// ...
		// std::cout << "Connection TimedOut" << std::endl;
	}

	return false;
}

void SmallPack::SmallPackCommunicationChannel::SetAnswerPort(uint32_t _port)
{
	// Set the answer port
	m_AnswerPort = _port;

	// Create the answer endpoint
	udp::resolver resolver(m_ChannelData.ioService);
	udp::resolver::query query(udp::v4(), m_ChannelData.address.to_string().c_str(), std::to_string(_port).c_str());
	m_ChannelData.answerEndpoint = *resolver.resolve(query);
}

void SmallPack::SmallPackCommunicationChannel::SendMessagePack(SmallPack::MessagePack* _messagePack)
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

void SmallPack::SmallPackCommunicationChannel::ProcessPingFunctionality(SmallPackPacker* _packer, uint32_t _currentTime)
{
	// Check if we have at last one message inside our send queue
	if (!m_SendQueue.size())
	{
		// Check if the other point wants a ping message
		if (m_PingInfo.receivedRequestedPing)
		{
			// Prepare a ping answer
			SmallPack::NetworkMessage newMessage; int dummyData = 0;
			SmallPackMessageComposer::Compose(_packer, newMessage, SmallPack::Operator::System, SmallPack::SystemCommands::None, 0, &dummyData);
			newMessage.messageHeader.messageFlags = SetFlag(newMessage.messageHeader.messageFlags, CommandFlags::Answer);
			PackMessage(newMessage, _packer);

			// Log
			// std::cout << " - Sending ping answer!" << std::endl;

			// Set that we sent the ping message
			m_PingInfo.receivedRequestedPing = false;
		}

		// We cant check for expecting ping messages here //
		/*
			Se verificarmos a necessidade de um ping aqui acabaremos
			enviando uma mensagem nova todo frame, o ideal é que se 
			queremos um ping, em algum momento enviaremos uma mensagem
			e dai sim colocaremos a flag de ping, por isso essa
			verificação ocorre apenas na parte abaixo.
		*/
	}
	// Ok we have at last one message to be sent
	else
	{
		// Check if the other point wants a ping message
		if (m_PingInfo.receivedRequestedPing)
		{
			// Adjust the request ping flag for the first message
			m_SendQueue[0]->messageHeader.messageFlags = SetFlag(m_SendQueue[0]->messageHeader.messageFlags, CommandFlags::Answer);

			// Set that we sent the ping message
			m_PingInfo.receivedRequestedPing = false;
		}

		// Check if we expect a ping message
		if (m_PingInfo.expectingPing)
		{
			// Adjust the expect ping flag for the first message
			m_SendQueue[0]->messageHeader.messageFlags = SetFlag(m_SendQueue[0]->messageHeader.messageFlags, CommandFlags::Request);

			// Set the last request time
			m_PingInfo.lastRequestTime = _currentTime;
		}
	}
}

void SmallPack::SmallPackCommunicationChannel::PackMessage(NetworkMessage& _message, SmallPackPacker* _packer)
{
	// Check if the current message pack is valid
	if (m_MessagePackSendList == nullptr)
	{
		// Request a new message pack
		m_MessagePackSendList = _packer->RequestMessagePack();
		m_MessagePackSendList->nextPack = nullptr;
	}

	// Set the message answer port and authentication token
	_message.messageHeader.answerPort = m_AnswerPort;
	_message.messageHeader.authToken = m_AuthenticationToken;

	// Pack this message
	if (!_packer->PackMessage(_message, m_MessagePackSendList))
	{
		// Request a new message pack
		MessagePack* newMessagePack = _packer->RequestMessagePack();

		// Insert this pack into the send list
		newMessagePack->nextPack = m_MessagePackSendList;
		m_MessagePackSendList = newMessagePack;

		// Try to pack again
		if (!_packer->PackMessage(_message, m_MessagePackSendList))
		{
			// Error!
			// ...
		}
	}
}

bool SmallPack::SmallPackCommunicationChannel::IsHost(boost::asio::ip::address _address, uint32_t _port)
{
	if (m_ChannelData.address == _address && m_ChannelData.port == _port)
	{
		return true;
	}

	return false;
}