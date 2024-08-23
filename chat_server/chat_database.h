#ifndef CHAT_DATABASE_H
#define CHAT_DATABASE_H


#include <mysql/mysql.h>
#include <mutex>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <cstring>


class DataBase
{
private:
	MYSQL *mysql;
	std::mutex _mutex;
public:
	DataBase();
	~DataBase();
	bool database_connect();
	void database_disconnect();
	int database_get_group_info(std::string *);
	bool database_init_table();
	bool database_user_exists(std::string );
	bool database_password_check(Json::Value &);
	void database_insert_user_info(Json::Value &v);
	bool database_get_friend_and_group(Json::Value &,std::string &,std::string &);
	void database_add_friend(Json::Value &);	
	void database_update_friendlist(std::string &,std::string &);
	void database_add_new_group(std::string ,std::string );
	void database_updata_group_member(std::string ,std::string );
	void database_get_g_member(std::string ,std::string &);



};




#endif
