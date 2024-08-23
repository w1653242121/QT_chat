#ifndef CHAT_LIST_H
#define CHAT_LIST_H

#include <iostream>
#include <mutex>
#include <list>
#include <map>
#include <event.h>
#include <jsoncpp/json/json.h>

struct User
{
	std::string name;	//user name
	struct bufferevent *bev;  //client event

};


class ChatInfo
{
	private:
		std::list<User> *online_user;  //save online user info
		std::map<std::string, std::list<std::string> > *group_info;    //save all group info
		
		//lock mutex
		std::mutex list_mutex;
		std::mutex map_mutex;


	public:
		ChatInfo();
		~ChatInfo();
		void list_update_group(std::string *,int);
		void list_show_group();	
		bool list_update_list(Json::Value &,struct bufferevent *);
		bool list_group_exist(std::string);
		struct bufferevent *list_friend_online(std::string );
		void list_add_new_group(std::string ,std::string );
		bool list_member_is_group(std::string ,std::string );
		void list_update_group_member(std::string ,std::string );
		std::list<std::string> &list_get_list(std::string);
		void list_delete_user(std::string );
};
















#endif
