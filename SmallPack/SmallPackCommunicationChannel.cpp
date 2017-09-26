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

SmallPack::SmallPackCommunicationChannel::SmallPackCommunicationChannel(boost::asio::io_service& _ioService) : m_ChannelData(_ioService)
{
	// Set the initial data
	m_PingInfo.ping = PingInfo::InitialPing;
	m_PingInfo.expectingPing = false;
	m_PingInfo.receivedRequestedPing = false;
	m_PingInfo.lastPingTime = 0;
	m_PingInfo.pingExpectedIdentifier = 0;
	m_MessagePackSendList = nullptr;
	m_AuthenticationToken = 0;
}

/*
SmallPack::SmallPackCommunicationChannel::SmallPackCommunicationChannel(const SmallPack::SmallPackCommunicationChannel& other)
{
}
*/

SmallPack::SmallPackCommunicationChannel::~SmallPackCommunicationChannel()
{
}


void SmallPack::SmallPackCommunicationChannel::FrameUpdate(uint32_t _currentTime, float _timeElapsed)
{
	// Check for ping re-send
	if (!m_PingInfo.expectingPing && _currentTime - m_PingInfo.lastPingTime >= m_PingInfo.ping * 2.0f + m_PingInfo.ping * 0.5f)
	{
		// Set that we need a new ping message
		m_PingInfo.expectingPing = true;

		// Log
		std::cout << "We are now expecting a ping message!" << std::endl;
	}

	// Check if our ping request timed out
	// ...
}

void SmallPack::SmallPackCommunicationChannel::ProcessSystemMessage(SmallPackPacker* _packer, NetworkMessage* _message, uint32_t _currentTime)
{

}

void SmallPack::SmallPackCommunicationChannel::ProcessPingAnswer(SmallPackPacker* _packer, NetworkMessage* _message, uint32_t _currentTime)
{
	/*
	// Check if we are expecting a ping message
	if (m_PingInfo.expectingPing && CheckFlag(_message->messageHeader.messageFlags, PingCommandType::Answer))
	{
		// Get the ping data
		CommandPing pingData;
		_message->GetDataObject(pingData);

		// Check the ping identifier
		if (pingData.pingIdentifier == m_PingInfo.pingExpectedIdentifier)
		{
			// Ok we found our ping
			m_PingInfo.expectingPing = false;

			// Increment our expected identifier
			m_PingInfo.pingExpectedIdentifier++;
		}
	}
	*/

	// Check if we are expecting a ping message
	if (m_PingInfo.expectingPing)
	{
		// Log
		std::cout << " - Received ping answer!" << std::endl;

		// Ok we found our ping
		m_PingInfo.expectingPing = false;

		// Increment our expected identifier
		m_PingInfo.pingExpectedIdentifier++;
	}
}

void SmallPack::SmallPackCommunicationChannel::QueueMessage(SmallPack::NetworkMessage* _message)
{
	// Reset the message flags
	_message->messageHeader.messageFlags = 0;

	// Insert the message into our send queue
	m_SendQueue.push_back(_message);
}

void SmallPack::SmallPackCommunicationChannel::CommitQueueMessage(SmallPackPacker* _packer, SmallPackMessageComposer* _composer, uint32_t _originPort, uint32_t _authToken, uint32_t _currentTime)
{
	// Process the ping functionality
	ProcessPingFunctionality(_packer, _composer, _originPort, _authToken, _currentTime);

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
	std::cout << " - Received ping request!" << std::endl;

	// Set the ping info
	m_PingInfo.receivedRequestedPing = true;
}

void SmallPack::SmallPackCommunicationChannel::SetAuthenticationToken(uint32_t _token)
{
	m_AuthenticationToken = _token;
}

void SmallPack::SmallPackCommunicationChannel::SetAnswerPort(uint32_t _port)
{
	// Set the answer port
	m_AnswerPort = _port;
	nao ta chamando aqui no cliente 
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
	m_ChannelData.socket.send_to(boost::asio::buffer(data, length), m_ChannelData.answerEndpoint);

	// TODO: Precido tomar cuidado pois devo deixar o pack num queue de envio MAS lembrar que caso estejamos usando o reliable channel
	// não podemos liberar esse pack depois do envio já que ele ainda é armazenado para confirmacao
}

void SmallPack::SmallPackCommunicationChannel::ProcessPingFunctionality(SmallPackPacker* _packer, SmallPackMessageComposer* _composer, uint32_t _originPort, uint32_t _authToken, uint32_t _currentTime)
{
	// Check if we have at last one message inside our send queue
	if (!m_SendQueue.size())
	{
		// Check if the other point wants a ping message
		if (m_PingInfo.receivedRequestedPing)
		{
			// Prepare a ping answer
			SmallPack::NetworkMessage newMessage; int dummyData = 0;
			_composer->Compose(_packer, SmallPack::Operator::System, 0, 0, _originPort, _authToken, dummyData, newMessage);
			newMessage.messageHeader.messageFlags = SetFlag(newMessage.messageHeader.messageFlags, PingCommandType::Answer);
			PackMessage(newMessage, _packer);

			// Log
			std::cout << " - Sending ping answer!" << std::endl;

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
			m_SendQueue[0]->messageHeader.messageFlags = SetFlag(m_SendQueue[0]->messageHeader.messageFlags, PingCommandType::Answer);

			// Set that we sent the ping message
			m_PingInfo.receivedRequestedPing = false;
		}

		// Check if we expect a ping message
		if (m_PingInfo.expectingPing)
		{
			// Adjust the expect ping flag for the first message
			m_SendQueue[0]->messageHeader.messageFlags = SetFlag(m_SendQueue[0]->messageHeader.messageFlags, PingCommandType::Request);
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