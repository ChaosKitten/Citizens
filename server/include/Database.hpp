#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "Configuration.hpp"
#ifdef WITH_MYSQL
#include <mysql++.h>
#endif

namespace Citizens
{
	class Database
	{
	public:
		Database();
		Database(db_config config);
		bool connect();
		bool connect(db_config config);
		bool authorise_user(const std::string& username,const std::string& auth_hash);
		bool get_userlist(std::map<std::string,bool>& list);
		bool get_accountlist(std::vector<std::string>& list);
		bool get_authinfo(const std::string& username,std::string& auth,bool& online);
		bool add_account(const std::string& username, const std::string& password);
		bool disconnect_user(const std::string& username);
		bool connect_user(const std::string& username);
		std::string get_error(void);
	private:
		db_config config;
		std::string error;
		// I'm trying desperately to separate implementation from interface here, 
		// but it's not easy without creating a second class for this crap
		#ifdef WITH_MYSQL
		mysqlpp::Connection mysql_connection;
		#endif
	};
};

#endif
