#include "SFMLNetwork.hpp"
#include <SFML/Network.hpp>
#include <cstdlib>
#include <cstring>
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/aes.h>
#include <sstream>

#include <iomanip>
#include <ios>

// let's do some fucking datalogging up in here
#include <iostream>

using namespace Citizens;

/**
 * \brief constructs an SFMLNetwork object
 */
SFMLNetwork::SFMLNetwork() : disconnected(true) 
{
	socket = new sf::TcpSocket();
	socket->setBlocking(true);
}

/**
 * \brief deletes the local 'socket' representation
 * \note in future versions this destructor may be deleted as the socket
 * should not be a pointer, it was only changed to a pointer while I was
 * experimenting with something
 */
SFMLNetwork::~SFMLNetwork()
{
	delete socket;
}

/**
 * \brief abstraction for establishing connection
 * \details little more than a wrapper around sf::TcpSocket::connect()
 * in future versions it may send a command to the server to verify
 * things like version matching
 * \param[in] ip the ip of the server to attempt connection with
 * \return a boolean representing whether or not a connection was
 * established.
 * \note modifies the 'disconnected' status of the network
 */
bool SFMLNetwork::connect(const std::string& ip)
{
	bool success = false; // assume false unless proven otherwise
	
	sf::IpAddress server_ip(ip);
	success = (socket->connect(server_ip,Protocol::port) == sf::Socket::Done);
	disconnected = !success;
	if(!success) error = "connection attempt failed";
	
	return success;
}

/**
 * \brief abstraction for sending a packet across the network
 * \details sends a packet across the network, obviously.  Note that
 * in future versions the parameter list may be switched for a single
 * Packet& parameter
 * \param[in] c the NetworkCommand for the packet
 * \param[in] payload_length the length of the payload of the packet (expected to be correct)
 * \param[in] msg a std::string containin the payload (far from the best way of doing it, I know)
 * \return a bool indicating whether the packet was successfully sent or not
 * note that the return status does not mean to imply whether or not the packet 
 * was received or not, only whether it was sent.
 */
bool SFMLNetwork::send(NetworkCommand c,char payload_length,const std::string& msg)
{
	bool success = false;
	
	unsigned char* data = (unsigned char*)std::malloc(NetworkCommand::length + payload_length + 1);
	
	std::memcpy(data,c.str().c_str(),NetworkCommand::length);
	data[NetworkCommand::length] = payload_length;
	std::memcpy(data + NetworkCommand::length + 1,msg.c_str(),payload_length);
	
	sf::Socket::Status status;
	while((status = socket->send(data,NetworkCommand::length + payload_length + 1)) != sf::Socket::Done)
	{
		if(status == sf::Socket::Error)
		{
			error = "send error";
			break;
		}
		if(status == sf::Socket::Disconnected)
		{
			disconnected = true;
			error = "disconnected";
			break;
		}
	}
	success = (status == sf::Socket::Done);
	
	
	std::free(data);
	return success;
}

/**
 * \brief fetch a packet from the network (a lot more complex than that)
 * \details expects the passed-in Packet to have enough space allocated
 * for the payload*.  If not enough space is allocated, a buffer overrun is possible
 * This is a known bug, which will be fixed in a later version.
 * \warning buffer overrun
 * \param[out] packet that is completely overwritten by the function (all fields are modified)
 */
bool SFMLNetwork::receive(Packet& packet)
{
	bool success = true;
	
	size_t received;
	sf::Socket::Status status;
	
	char* command = (char*)std::malloc(NetworkCommand::length);
	
	while((status = socket->receive(command,NetworkCommand::length,received)) != sf::Socket::Done)
	{
		if(status == sf::Socket::Error)
		{
			error = "receive error";
			success = false;
			break;
		}
		else if(status == sf::Socket::Disconnected)
		{
			error = "disconnected";
			success = false;
			break;
		}
	}
	
	packet.command = (NetworkCommand)command;
	std::free(command);
	if(packet.command == ENC_INVALID)
	{
		error = "invalid command";
		return false;
	}
	
	unsigned char payload_size;
	while((status = socket->receive(&payload_size,1,received)) != sf::Socket::Done)
	{
		if(status == sf::Socket::Error)
		{
			error = "receive error";
			success = false;
			break;
		}
		else if(status == sf::Socket::Disconnected)
		{
			error = "disconnected";
			success = false;
			break;
		}
	}
	
	packet.payload_size = (unsigned int) payload_size;
	
	while((status = socket->receive(packet.payload,packet.payload_size,received)) != sf::Socket::Done)
	{
		if(status == sf::Socket::Error)
		{
			error = "receive error";
			success = false;
			break;
		}
		else if(status == sf::Socket::Disconnected)
		{
			error = "disconnected";
			success = false;
			break;
		}
	}
	
	return success;
}

/**
 * \brief This function is in charge of a lot, so I'm going to do my best to document it but forgive me if I don't catch everything.
 * \details
 * First, the client sends a "LOGIN" packet with the username in it.
 * Next, the server replies with a "LOGIN" packet that contains a single byte of random data
 * Next, the client is expected to encrypt the packet using a hash of the password (salted with the username - note that we may change the salt later if security becomes a problem)
 * The resultant 16 bytes of encrypted data is sent back to the server in an AUTHR packet
 * The server then compares the encrypted data with its own version of the same procedure, using a hash stored in the database as the encryption key
 * The server will then respond with an AUTHR packet that has the payload "GOOD" or "BAD!" to indicate success or failure.
 * There is no way for the client to tell whether it is the username or password which is wrong, so don't even try to program in that security hole
 * \param[in] username pretty obvious really, preserves constness
 * \param[in] password also obvious, also preserves constness
 * \return a boolean indicating whether the login attempt was successful or not
 */
bool SFMLNetwork::login(const std::string& username, const std::string& password)
{
	bool success = false;
	std::cout << "Attempting login" << std::endl;
	if(username.size() > 0xFF)
	{
		std::cout << "Bad login: username length > 0xFF" << std::endl;
		return false;
	}
	std::cout << "creating hash for encryption..." << std::endl;
	CryptoPP::SHA1 sha1;
	std::stringstream source;
	std::string hash;
	source << username;
	source << password;
	CryptoPP::StringSource(source.str(), true, new CryptoPP::HashFilter(sha1, new CryptoPP::HexEncoder(new CryptoPP::StringSink(hash),false)));
	

	std::cout << "hash: " << hash << std::endl;
	
	std::cout << "sending username..." << std::endl;
	success = send(ENC_LOGIN,username.size(),username);
	if(!success)
	{
		std::cout << "Error sending username" << std::endl;
		return success;
	}
	std::cout << "receiving random byte..." << std::endl;
	
	Packet p;
	success = receive(p);
	std::cout << "COMMAND: " << p.command << std::endl;
	std::cout << "Payload: " << p.payload << std::endl;
	
	if(!success)
	{
		std::cout << "Error receiving random byte" << std::endl;
		return success;
	}
	if(success)
	{
		std::cout << "random byte received: ";
		std::cout << std::hex << std::setfill('0') << std::setw(2) << std::nouppercase << (unsigned short)p.payload[NetworkCommand::length+1];
		std::cout << std::endl;
		CryptoPP::BlockCipher* encryptor = new CryptoPP::AES::Encryption();
		encryptor->SetKey((unsigned char*)hash.c_str(),CryptoPP::AES::MAX_KEYLENGTH);
		unsigned char* block = (unsigned char*)std::malloc(encryptor->BlockSize());
		std::memset(block,p.payload[NetworkCommand::length+1],encryptor->BlockSize());
		encryptor->ProcessBlock(block);
		std::cout << "sending back the random byte, encrypted (" << encryptor->BlockSize() << " bytes)..." << std::endl;
		success = send(ENC_AUTHORISATION,encryptor->BlockSize(),(char*)block);
		if(!success)
		{
			std::cout << "failed to return the random byte" << std::endl;
		}
		else
		{
			std::cout << "random byte sent" << std::endl;
		}
		std::free(block);
		delete encryptor;
	}
	// if we got THAT far...
	if(success)
	{
		std::cout << "receiving server confirmation on good/badness" << std::endl;
		//Packet p;
		success = receive(p);
		if(success)
		{
			p.payload[NetworkCommand::length+5] = 0; // insert nul-terminator into the stream
			std::cout << "Server replied: " << p.payload+NetworkCommand::length+1 << std::endl;
		}
		else
		{
			std::cout << "Could not retrieve server response: " << error << std::endl;
		}
	}
	return success;
}

/**
 * \brief returns whether the network has been disconnected
 * \return a boolean indicating whether or not the network is disconnected
 */
bool SFMLNetwork::is_disconnected(void)
{
	return disconnected;
}

/**
 * \brief get a string representation of the error status of the network
 * \return a string representation of the error status of the network
 */
std::string SFMLNetwork::get_error(void)
{
	return error;
}

/**
 * \brief fetch an XML resource over the network
 * \return an XML reader resource if it could be fetched, or NULL
 * \todo implement this method
 */
irr::io::IXMLReader* SFMLNetwork::get_XML(const std::string& resource_name)
{
	return NULL;
}
