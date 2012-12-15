/*
 * MySQL version of Database.cpp
 * Note that this implementation file should be easy enough to swap out
 * if you want to use a non-MySQL backend in the future
 * none of the Database class methods or members care much about
 * the back-end.
 */
#ifndef WITH_MYSQL
#error Needs mysql shit, man, you total noob and stuff
#endif
#include "Database.hpp"

using namespace Citizens;

Database::Database()
{
	db_config c;
	c.host = "";
	c.user = "";
	c.password = "";
	c.schema = "";
	c.port = 0;
	config = c;
	
	// if this isn't overwritten before Database::get_error() is called
	// we're in for some big trouble...
	error = "unknown error";
}

Database::Database(db_config c)
{
	config = c;
}

bool Database::connect()
{
	bool rv = true;
	try {
		mysql_connection.connect(config.schema.c_str(),config.host.c_str(),config.user.c_str(),config.password.c_str(),config.port);
	} catch (mysqlpp::ConnectionFailed ec) {
		error = ec.what();
		rv = false;
	}
	return (rv && mysql_connection.connected());
}

bool Database::connect(db_config c)
{
	config = c;
	return connect();
}

std::string Database::get_error(void)
{
	return error;
}

bool Database::get_userlist(std::map<std::string,bool>& list)
{
	mysqlpp::StoreQueryResult result;
	try
	{
		mysqlpp::Query query = mysql_connection.query("SELECT username,online FROM `citizens`");
		result = query.store();
	} catch(mysqlpp::BadQuery e) {
		error = e.what();
		return false;
	}
	
	std::stringstream to_bool;
	for(size_t i = 0; i < result.num_rows(); ++i)
	{
		std::stringstream to_bool(static_cast<std::string>(result[i][1]));
		to_bool >> list[static_cast<std::string>(result[i][0])];
	}
	return true;
}

bool Database::get_accountlist(std::vector<std::string>& list)
{
	mysqlpp::StoreQueryResult result;
	try
	{
		mysqlpp::Query query = mysql_connection.query("SELECT `username` FROM `accounts`");
		result = query.store();
	} catch(mysqlpp::BadQuery e) {
		error = e.what();
		return false;
	}
	
	for(size_t i = 0; i < result.num_rows(); ++i)
	{
		list.push_back(static_cast<std::string>(result[i][0]));
	}
	return true;
}

bool Database::add_account(const std::string& username, const std::string& password)
{
	try
	{
		mysqlpp::Query query = mysql_connection.query("SELECT `username` FROM `accounts` WHERE `username` = '"+username+"'");
		if(query.store().num_rows() != 0)
		{
			error = "duplicate user";
			return false;
		}
	} catch (mysqlpp::BadQuery e) {
		// we don't want to run the risk of duplicating the user in this case
		return false;
	}
	std::stringstream query_stream;
	query_stream 
	<< "INSERT INTO `citizens`.`accounts`(`username`,`auth`) VALUES ('"
	<< username
	<< "',SHA1('"
	<< username
	<< password
	<< "'))";
	try
	{
		mysqlpp::Query query = mysql_connection.query(query_stream.str());
		return query.exec();
	} catch (mysqlpp::BadQuery e) {
		error = e.what();
		return false;
	}
}

bool Database::get_authinfo(const std::string& username,std::string& auth,bool& online)
{
	try
	{
		mysqlpp::Query query = mysql_connection.query("SELECT `auth`,`online` FROM `accounts` WHERE `username` = '"+username+"'");
		mysqlpp::StoreQueryResult result = query.store();
		if(result.num_rows() == 0) { return false; }
		// mysqlpp is a bit lacking, it sends back out 2D arrays of [col][row]:
		auth = static_cast<std::string>(result[0][0]);
		online = (result[0][1] != "0");
		return true;
	} catch (mysqlpp::BadQuery e) {
		error = e.what();
		return false;
	}
}

bool Database::disconnect_user(const std::string& username)
{
	try
	{
		mysqlpp::Query query = mysql_connection.query("UPDATE `accounts` SET `online` = '0' WHERE `username` = '"+username+"'");
		return query.exec();
	} catch (mysqlpp::BadQuery e) {
		error = e.what();
		return false;
	}
}

bool Database::connect_user(const std::string& username)
{
	try
	{
		mysqlpp::Query query = mysql_connection.query("UPDATE `accounts` SET `online` = '1' WHERE `username` = '"+username+"'");
		return query.exec();
	} catch (mysqlpp::BadQuery e) {
		error = e.what();
		return false;
	}
}
