#pragma once
#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H
#include <event.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <event2/listener.h>
#include "chat_database.h"
#include "chat_list.h"
#include "chat_thread.h"

#define IP "172.20.95.223"
#define PORT 8002
class ChatServer
{
	private:
		struct event_base* base; //用于监听事件集合
		DataBase *db;     //数据库对象
		ChatInfo *info;	//在线用户信息，群信息
		ChatThread *pool; //线程池对象
		int thread_num;  //记录线程数量
		int cur_thread; //记录当前线程号


	public:
		ChatServer();
		~ChatServer();
		void listen(const char *,int);
		static void listener_cb(struct evconnlistener*, evutil_socket_t,struct sockaddr* , int , void* );
		void server_update_group_info();
		void server_alloc_event(int);
};









#endif

