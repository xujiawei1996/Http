#ifndef __SQL_H__
#define __SQL_H__

#include<iostream>
#include<mysql.h>
#include<string>
#include<stdlib.h>
#include<string.h>

using namespace std;

class sql
{
public:
	sql(const std::string &_user,\
		const std::string &_ip,\
		const std::string &_passwd,\
		const std::string &_db,\
		const int &_port);
	~sql();
	int connect();
	int insert(const std::string &name,\
	const std::string &sex,\
	const std::string &school,\
	const std::string &hobby);
	int select();
private:
	MYSQL *conn;
	std::string user;
	std::string ip;
	std::string passwd;
	std::string db;
	int port;
};


#endif
