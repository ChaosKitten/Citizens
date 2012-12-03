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

SFMLNetwork::SFMLNetwork(const Protocol& p) : protocol(p), disconnected(true) {}

bool SFMLNetwork::connect(const std::string& ip)
{
	bool success = false; // assume false unless proven otherwise
	
	sf::IpAddress server_ip(ip);
	success = (socket.connect(server_ip,protocol.port) == sf::Socket::Done);
	disconnected = !success;
	if(!success) error = "connection attempt failed";
	
	return success;
}

bool SFMLNetwork::send(NetworkCommand c,char payload_length,const std::string& msg)
{
	bool success = false;
	
	unsigned char* data = (unsigned char*)std::malloc(NetworkCommand::length + payload_length + 1);
	
	std::memcpy(data,c.str().c_str(),NetworkCommand::length);
	data[NetworkCommand::length] = payload_length;
	std::memcpy(data + NetworkCommand::length + 1,msg.c_str(),payload_length);
	
	sf::Socket::Status status;
	while((status = socket.send(data,NetworkCommand::length + payload_length + 1)) != sf::Socket::Done)
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

bool SFMLNetwork::receive(unsigned char* buf,unsigned int size)
{
	bool success = true;
	size_t received_bytes;
	size_t total_received;
	sf::Socket::Status status;
	while((status = socket.receive(buf,size,received_bytes)) != sf::Socket::Done)
	{
		if(status == sf::Socket::Error)
		{
			error = "receive error";
			success = false;
			break;
		}
		else if(status == sf::Socket::Disconnected)
		{
			std::stringstream errs;
			errs << "disconnected after " << received_bytes << " bytes";
			error = errs.str();
			success = false;
			break;
		}
	}
	disconnected = (status == sf::Socket::Disconnected);
	
	return success;
}

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
	
	unsigned int bufsize = NetworkCommand::length + 1 + sha1.DigestSize();
	unsigned char* buf = (unsigned char*)std::malloc(bufsize);
	std::cout << "sending username..." << std::endl;
	success = send(ENC_LOGIN,username.size(),username);
	if(!success)
	{
		std::cout << "Error sending username" << std::endl;
		return success;
	}
	std::cout << "receiving random byte..." << std::endl;
	
	success = receive(buf,NetworkCommand::length + 2);
	std::cout << "DEBUG: " << buf << std::endl;
	if(!success)
	{
		std::cout << "Error receiving random byte" << std::endl;
		return success;
	}
	if(success)
	{
		std::cout << "random byte received: ";
		std::cout << std::hex << std::setfill('0') << std::setw(2) << std::nouppercase << (unsigned short)buf[NetworkCommand::length+1];
		std::cout << std::endl;
		CryptoPP::BlockCipher* encryptor = new CryptoPP::AES::Encryption();
		encryptor->SetKey((unsigned char*)hash.c_str(),CryptoPP::AES::MAX_KEYLENGTH);
		unsigned char* block = (unsigned char*)std::malloc(encryptor->BlockSize());
		std::memset(block,buf[NetworkCommand::length+1],encryptor->BlockSize());
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
		success = receive(buf,NetworkCommand::length + 5);
		if(success)
		{
			buf[NetworkCommand::length+5] = 0; // insert nul-terminator into the stream
			std::cout << "Server replied: " << buf+NetworkCommand::length+1 << std::endl;
		}
		else
		{
			std::cout << "Could not retrieve server response: " << error << std::endl;
		}
	}
	std::free(buf);
	return success;
}

bool SFMLNetwork::is_disconnected(void)
{
	return disconnected;
}

std::string SFMLNetwork::get_error(void)
{
	return error;
}
