#include "SFMLNetwork.hpp"
#include <SFML/Network.hpp>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cryptopp/sha.h>
#include <cryptopp/aes.h>
#include <cstdio>
#include <algorithm>

using namespace Citizens;

SFMLNetwork::SFMLNetwork(Logger& log,Database& db) : logger(log), database(db), last_command(ENC_NO_COMMAND)
{
	error = "Unknown error";
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
	
	// make sure this socket doesn't block, because we want to support concurrent connections without threading
	socket.setBlocking(false);
	
	return success;
}

bool SFMLNetwork::send_byte(ClientData client,const unsigned char& byte,bool& sent)
{
	sf::Socket::Status st = client.socket->send(&byte,1);
	if(st==sf::Socket::Disconnected)
	{
		client.disconnected = true;
	}
	sent = (st==sf::Socket::Done);
	return sent; // we should really remove the parameter and just use a return value
}

//! \note blocking call
bool SFMLNetwork::send_all(ClientData client,const unsigned char* data, const unsigned int size)
{
	unsigned int i = 0;
	while(i < size)
	{
		bool sent = false;
		send_byte(client,data[i],sent);
		if(sent) i++;
	}
	return (i == size);
}

bool SFMLNetwork::get_byte(ClientData& client,unsigned char& byte)
{
	if(client.packet_size == client.packet_expected) return true;
	size_t bytes_received = 0;
	sf::Socket::Status st = client.socket->receive(&byte,1,bytes_received);
	switch(st)
	{
		case sf::Socket::Disconnected:
			client.disconnected = true;
			break;
		case sf::Socket::Done:
		{
			client.packet_size += bytes_received;
			/*
			std::stringstream logmsg;
			logmsg << client.packet_size << "/" << client.packet_expected << " bytes received";
			logger.log(logmsg.str(),NET);
			*/
			break;
		}
		case sf::Socket::Error:
			logger.log("socket error in get_byte",NET);
			break;
		case sf::Socket::NotReady:
			break;
	}
	return false; // we expect there to be more data to receive later on
}

void SFMLNetwork::stopThread(void)
{
	running = false;
}

//! \note I forget why this routine returns a bool representing whether a new connection was received or not
bool SFMLNetwork::tick(void)
{
	bool new_connection = false;
	
	sf::TcpSocket* new_socket = NULL;
	unsigned int connection_count = active_connections.size();
	
	
	do
	{
		new_socket = new sf::TcpSocket();
		new_socket->setBlocking(false);
		if(socket.accept(*new_socket) == sf::Socket::Done)
		{
			ClientData cd;
			cd.token = std::rand();
			cd.socket = new_socket;
			cd.bad_user = false;
			cd.authorised = false; 
			cd.username = "";
			cd.packet_expected = NetworkCommand::length + 1;
			
			// assign enough for the biggest possible packet, then never reallocate (trade space for time)
			cd.packet = (unsigned char*)std::malloc(NetworkCommand::length + 0x100);
			cd.packet_size = 0;
			cd.disconnected = false;
			cd.online = false;
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
		
		// get_byte() returns true only if the 'packet' has been completely received
		bool packet_complete = get_byte(
			*i,
			i->packet[i->packet_size]
		);
		
	
		if(i->disconnected)
		{
			std::stringstream msg;
			msg << "Client " << i->socket->getRemoteAddress() << " disconnected.";
			logger.log(msg.str(),NET);
			erasables.push_back(i);
			database.disconnect_user(i->username);
			break;
		}
		
		if(!packet_complete) continue;
		
		if(command_mode)
		{
			//! \note maybe ugly, maybe elegant.  my tired, ill and uneducated brain can't decide:
			last_command = NetworkCommand(std::string((char*)i->packet).substr(0,NetworkCommand::length));
			command_mode = false; // payload mode!
			i->packet_expected = i->packet[NetworkCommand::length];
			i->packet_size = 0;
		}
		else
		{
			switch(last_command)
			{
				case ENC_LOGIN:
				{
					// send back the random byte for encrypting
					unsigned char* response = (unsigned char*) std::malloc(7);
					std::memcpy(response,NetworkCommand(ENC_LOGIN).str().c_str(),NetworkCommand::length);
					response[NetworkCommand::length] = 1; // payload is 1 byte long
					response[NetworkCommand::length+1] = i->token;
					std::stringstream logmsg;
					if(!send_all(*i,response,NetworkCommand::length+2))
					{
						logmsg << "Network error: could not send random byte";
					}
					else
					{
						logmsg << "Received login request from " << i->socket->getRemoteAddress();
						logmsg << " and sent back random byte: ";
						logmsg << std::hex << std::setfill('0') << std::setw(2) << std::nouppercase << (unsigned short)response[6];
						logmsg << " (should be ";
						logmsg << std::hex << std::setfill('0') << std::setw(2) << std::nouppercase << (unsigned short)i->token;
						logmsg << ")";
					}
					std::free(response);
					
					// null terminate the username
					i->packet[i->packet_expected] = 0;
					
					i->username = (char*)i->packet;
					
					bool online = false;
					if(!database.get_authinfo(i->username,i->auth,online) || online)
					{
						std::stringstream lm;
						i->bad_user = true;
						if(online)
						{
							lm << "User " << i->username << " is already online";
						}
						else
						{
							lm << "User " << i->username << " rejected: could not find username in database";
						}
						logger.log(lm.str(),NET);
					}
					else
					{
						logger.log(i->auth,NET);
					}
					logmsg << " (user: " << i->username << ")";
					logger.log(logmsg.str(),NET);
					break;
				}
				case ENC_AUTHORISATION:
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
						std::printf("%.2X ",i->packet[j]);
					}
					std::printf("\n");
					unsigned char response[NetworkCommand::length + 5];
					std::memcpy(response,NetworkCommand(ENC_AUTHORISATION).str().c_str(),NetworkCommand::length);
					response[NetworkCommand::length] = 0x04; // payload is 4 bytes long
					
					//! \note We need std::memcmp for this line, so we might as well take advantage of memcpy and so on while we've got them
					if(!i->bad_user && std::memcmp(i->packet,block,i->packet_expected) == 0)
					{
						logger.log("User good",DEBUG);
						std::memcpy(response+NetworkCommand::length+1,"GOOD",4);
						i->authorised = true;
						
						std::stringstream logmsg;
						if(!send_all(*i,response,NetworkCommand::length +5))
							logmsg << "Network Error: Could not return packet";
						else
						{
							logmsg << "Authorised user: " << i->username;
							database.connect_user(i->username);
							i->online = true;
						}
						logger.log(logmsg.str(),NET);
					}
					else
					{
						logger.log("User bad",DEBUG);
						std::memcpy(response+6,"BAD!",4);
						i->authorised = false;
						
						std::stringstream logmsg;
						if(!send_all(*i,response,NetworkCommand::length + 5))
							logmsg << "Network Error: Could not return packet";
						else
							logmsg << "Rejected user: " << i->username;
						logger.log(logmsg.str(),NET);
						if(!i->bad_user)
						{
							database.disconnect_user(i->username);
							i->online = false;
						}
					}
					
					std::free(block);
					delete encryptor;
					break;
				}
				case ENC_NO_COMMAND:
				{
					logger.log("invalid command received",NET);
					break;
				}
			} // switch(last_command)
			command_mode = true; // back into boring old command mode next packet
			i->packet_expected = NetworkCommand::length+1; // oops!
			i->packet_size = 0;
		} // else // if(command_mode)
	} //for (std::vector<ClientData>::iterator i = active_connections.begin(); i != active_connections.end(); ++i)
	
	for(unsigned int i = 0; i < erasables.size(); i++)
	{
		// only disconnect online users from the database
		if(erasables[i]->online)
			database.disconnect_user(erasables[i]->username);
		std::free(erasables[i]->packet);
		delete erasables[i]->socket;
		active_connections.erase(erasables[i]);
	}
	
	return new_connection;
}

SFMLNetwork::~SFMLNetwork()
{
	for(std::vector<ClientData>::iterator i = active_connections.begin(); i != active_connections.end(); i++)
	{
		std::free(i->packet);
		delete i->socket;
	}
}

std::string SFMLNetwork::get_error(void)
{
	return error;
}
