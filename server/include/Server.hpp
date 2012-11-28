#ifndef SERVER_HPP
#define SERVER_HPP

#include "Configuration.hpp"
#include "Options.hpp"
#include "Database.hpp"
#include "Network.hpp"
#include "Logger.hpp"
#include <string>
#include <map>

namespace Citizens
{
	enum errnum_t
	{
		NONE,
		BAD_CONFIG,
		KILLED,
		FORCED_ERROR,
		BAD_DATABASE,
		BAD_NETWORK,
	};
	
	/**
	 * \brief Main class for the server implementation
	 * \details 
	 * \author Chris Browne <yoda2031@gmail.com>
	 */
	class Server
	{
	public:
		Server(Options& o,Network& n,Logger& l,Database& d);
		/**
		 * \brief Initialises the server
		 * \return a Configuration object representing the read-in config
		 */
		Configuration init();
		/**
		 * \brief Returns the server's current error number
		 * \return an int representing the server's current error status
		 */
		int get_errnum(void);
		/**
		 * \brief Fetch a user-readable error status
		 * \return a string representing the server's current error status
		 */
		std::string get_error(void);
		/**
		 * \brief Thread-helping function
		 */
		static void run(void* s);
		/**
		 * \brief Start the server running
		 */
		void start(void);
		/**
		 * \brief Stop the server
		 */
		void stop(void);
		/**
		 * \brief Cast to bool
		 * \details Useful for shortening if() statements
		 * \return true if errnum == 0, false otherwise
		 */
		operator bool(void);
	private:
		/**
		 * \brief enter the main server loop
		 */
		void loop(void);
		bool running;
		errnum_t errnum;
		Configuration config;
		std::string prompt;
		Options& opt;
		Database& db;
		Logger& logger;
		Network& net;
	};
};

#endif
