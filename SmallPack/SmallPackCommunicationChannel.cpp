////////////////////////////////////////////////////////////////////////////////
// Filename: SmallPackCommunicationChannel.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SmallPackCommunicationChannel.h"

using namespace boost::asio::ip;

SmallPack::SmallPackCommunicationChannel::SmallPackCommunicationChannel(boost::asio::io_service& _ioService) : m_ioService(_ioService), m_Socket(_ioService)
{
}

/*
SmallPack::SmallPackCommunicationChannel::SmallPackCommunicationChannel(const SmallPack::SmallPackCommunicationChannel& other)
{
}
*/

SmallPack::SmallPackCommunicationChannel::~SmallPackCommunicationChannel()
{
}

bool SmallPack::SmallPackCommunicationChannel::Initialize(const char* _host, const char* _port)
{
	udp::resolver resolver(m_ioService);
	udp::resolver::query query(udp::v4(), _host, _port);
	m_Iterator = resolver.resolve(query);


	/*
	using namespace std; // For strlen.
	std::cout << "Enter message: ";
	char request[max_length];
	std::cin.getline(request, max_length);
	size_t request_length = strlen(request);
	s.send_to(boost::asio::buffer(request, request_length), *iterator);

	char reply[max_length];
	udp::endpoint sender_endpoint;
	size_t reply_length = s.receive_from(
		boost::asio::buffer(reply, max_length), sender_endpoint);
	std::cout << "Reply is: ";
	std::cout.write(reply, reply_length);
	std::cout << "\n";
	*/

	return true;
}

/*
void SmallPack::SmallPackCommunicationChannel::Send(SmallPack::NetworkMessage& _message)
{
	m_Socket.send_to(boost::asio::buffer(request, request_length), *m_Iterator);
}

Preciso dar um jeito dë "serializar" uma mensagem e empacotar elas
o ideal é ter um tamanho maximo pra cada mensagem e criar uma lista onde posso ficar pegando e devolvendo data
preciso ajustar o empacotador para juntar multiplas mensagens caso a gente queira, o ideal é que a mensagem enviada ja seja do tipo "empacotada" e
que comecemos empacotando apenas 1 mensagem, caso possível vamos aumentando esse tamanho até que consigamos descobrir o tamanho ideal, entao 
SEMPRE devemos enviar uma mensagem do tipo "PackedMessage"
*/