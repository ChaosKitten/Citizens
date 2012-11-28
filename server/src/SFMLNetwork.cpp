#include "SFMLNetwork.hpp"
#include <SFML/Network.hpp>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cryptopp/sha.h>
#include <cryptopp/aes.h>
#include <cstdio>

#define COMMAND_SIZE 5

using namespace Citizens;

SFMLNetwork::SFMLNetwork(Logger& log,Database& db) : logger(log), database(db), last_command(NO_COMMAND)
{
	error = "Unknown error";
	expected_size = COMMAND_SIZE+1; // command + payload size byte
	command_mode = true;
	std::srand(std::time(0));
}

bool SFMLNetwork::listen(const unsigned short port)
{
	bool success = false;
	success = (socket.listen(port) == sf::Socket::Done);
	if(!success)
	{
		std::stringstream err_builder;
		err_builder << "Could not listen to port " << port;
		error = err_builder.str();
		logger.log(error,ERROR);
	}
	
	// make sure the socket doesn't block, because we want to support concurrent connections without threading
	socket.setBlocking(false);
	
	return success;
}

bool SFMLNetwork::send(const std::string& msg)
{
	bool success = false;
	
	return success;
}

bool SFMLNetwork::receive(std::string& result)
{
	bool success = false;
	
	return success;
}

bool SFMLNetwork::login(const std::string& username, const std::string& password)
{
	bool success = false;
	
	return success;
}

void SFMLNetwork::stopThread(void)
{
	running = false;
}

bool SFMLNetwork::tick(void)
{
	bool new_connection = false;
	
	sf::TcpSocket* new_socket = NULL;
	unsigned int connection_count = active_connections.size();
	
	std::string username;
	
	do
	{
		new_socket = new sf::TcpSocket();
		new_socket->setBlocking(false);
		if(socket.accept(*new_socket) == sf::Socket::Done)
		{
			ClientData cd;
			cd.token = std::rand();
			cd.sock = new_socket;
			cd.bad_user = false; // before a username has been received, we can't know if it was bad
			cd.authorised = false;
			cd.username = "";
			active_connections.push_back(cd);
			std::stringstream msg;
			msg << "Connection received from " << new_socket->getRemoteAddress();
			logger.log(msg.str(),NET);
		}
		else
		{
			delete new_socket;
			break;
		}
	} while(true);
	
	new_connection = (active_connections.size() > connection_count);
	
	std::vector<std::vector<ClientData>::iterator> erasables;
	
	for(std::vector<ClientData>::iterator i = active_connections.begin(); i != active_connections.end(); ++i)
	{
		unsigned char* data = (unsigned char*)std::malloc(expected_size);
		sf::Socket::Status socket_stat;
		size_t bytes_read = 0;
		
		// this boolean stops the loop from trying to parse a disconnection
		bool cont = false;
		// wait for expected_size worth of data on this connection; once it's been read, move on to parsing
		// this is the reason why each client should spawn a new thread (or new process) on the server
		// but I'm currently unable to figure out threading enough to be able to implement this.
		while((socket_stat = i->sock->receive(data,expected_size,bytes_read)) != sf::Socket::Done)
		{
			if(socket_stat == sf::Socket::Disconnected)
			{
				std::stringstream msg;
				msg << "Client " << i->sock->getRemoteAddress() << " disconnected.";
				logger.log(msg.str(),NET);
				erasables.push_back(i);
				cont = true;
				database.disconnect_user(i->username);
				break;
			}
			if(socket_stat == sf::Socket::Error)
			{
				logger.log("Network error",NET);
				break;
			}
		}
		if(cont) continue;
		if(command_mode)
		{
			command_mode = false; // payload mode!
			expected_size = data[COMMAND_SIZE];
			if(std::memcmp(data,"LOGIN",COMMAND_SIZE) == 0)
			{
				last_command = LOGIN;
			}
			else if(std::memcmp(data,"AUTHR",COMMAND_SIZE) == 0)
			{
				last_command = AUTHORISATION;
				std::stringstream logmsg;
				logmsg << i->sock->getRemoteAddress() << " requests authorisation for user " << i->username;
				logger.log(logmsg.str(),NET);
			}
			else
			{
				last_command = NO_COMMAND;
			}
		}
		else
		{
			command_mode = true; // back into boring old command mode next packet
			expected_size = NetworkCommand::length+1; // oops!
			switch(last_command)
			{
				case LOGIN:
				{
					// send back the random byte for encrypting
					char response[7];
					std::memcpy(response,"LOGIN",5);
					response[5] = 1; // payload is 1 byte long
					response[6] = i->token;
					std::stringstream logmsg;
					sf::Socket::Status status;
					while((status = i->sock->send(response,7)) != sf::Socket::Done)
					{
						if(status == sf::Socket::Error || status == sf::Socket::Disconnected) break;
					}
					if(status != sf::Socket::Done)
					{
						logmsg << "Network error: could not send random byte";
					}
					else
					{
						logmsg << "Received LOGIN request from " << i->sock->getRemoteAddress();
						logmsg << " and sent back random byte: ";
						logmsg << std::hex << std::setfill('0') << std::setw(2) << std::nouppercase << (unsigned short)i->token;
					}
					
					i->username = (char*)data;
					bool online = false;
					if(!database.get_authinfo(i->username,i->auth,online) || online)
					{
						i->bad_user = true;
					}
					else
					{
						logger.log(i->auth,NET);
					}
					logmsg << " (user: " << i->username << ")";
					logger.log(logmsg.str(),NET);
					break;
				}
				case AUTHORISATION:
				{
					CryptoPP::SHA1 hash;
					unsigned char* hashbuf = (unsigned char*) i->auth.c_str();
					CryptoPP::BlockCipher* encryptor = new CryptoPP::AES::Encryption();
					encryptor->SetKey(hashbuf,CryptoPP::AES::MAX_KEYLENGTH);
					unsigned char* block = (unsigned char*)std::malloc(encryptor->BlockSize());
					std::memset(block,i->token,encryptor->BlockSize());
					encryptor->ProcessBlock(block);
					std::printf("%d bytes follow: ",encryptor->BlockSize());
					for(unsigned int j = 0;j < encryptor->BlockSize();j++)
					{
						std::printf("%.2X ",block[j]);
					}
					std::printf("\n");
					std::printf("%d bytes follow: ",encryptor->BlockSize());
					for(unsigned int j = 0;j < encryptor->BlockSize();j++)
					{
						std::printf("%.2X ",data[j]);
					}
					std::printf("\n");
					char response[NetworkCommand::length + 5];
					std::memcpy(response,"AUTHR",5);
					response[5] = 4; // payload is 4 bytes long
					if(std::memcmp(data,block,expected_size) == 0)
					{
						logger.log("User good",DEBUG);
						std::memcpy(response+6,"GOOD",4);
						i->authorised = true;
						sf::Socket::Status status;
						while((status = i->sock->send(response,NetworkCommand::length + 5)) != sf::Socket::Done)
						{
							if((status == sf::Socket::Error) || (status == sf::Socket::Disconnected)) break;
						}
						std::stringstream logmsg;
						if(status != sf::Socket::Done)
							logmsg << "Network Error: Could not return packet";
						else
						{
							logmsg << "Authorised user: " << i->username;
							database.connect_user(i->username);
						}
						logger.log(logmsg.str(),NET);
					}
					else
					{
						logger.log("User bad",DEBUG);
						std::memcpy(response+6,"BAD!",4);
						i->authorised = false;
						sf::Socket::Status status;
						while((status = i->sock->send(response,NetworkCommand::length + 5)) != sf::Socket::Done)
						{
							if((status == sf::Socket::Error) || (status == sf::Socket::Disconnected)) break;
						}
						std::stringstream logmsg;
						if(status != sf::Socket::Done)
							logmsg << "Network Error: Could not return packet";
						else
							logmsg << "Rejected user: " << i->username;
						logger.log(logmsg.str(),NET);
					}
					
					std::free(block);
					delete encryptor;
					break;
				}
				case NO_COMMAND:
				{
					logger.log("invalid command received",NET);
					break;
				}
			}
		}
		std::free(data);
	}
	
	for(unsigned int i = 0; i < erasables.size(); i++)
	{
		delete erasables[i]->sock;
		active_connections.erase(erasables[i]);
	}
	
	return new_connection;
}

SFMLNetwork::~SFMLNetwork()
{
	for(std::vector<ClientData>::iterator i = active_connections.begin(); i != active_connections.end(); i++)
	{
		delete i->sock;
	}
}

std::string SFMLNetwork::get_error(void)
{
	return error;
}
