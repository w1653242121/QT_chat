#ifndef CHAT_THREAD_H
#define CHAT_THREAD_H

#include <thread>
#include <event.h>
#include "chat_list.h"
#include "chat_database.h"
#include "string.h"
#include <jsoncpp/json/json.h>


class ChatThread
{
private:
	std::thread *_thread;
	std::thread::id _id;
	struct event_base *base;
	ChatInfo *info;
	DataBase *db;



public:
	ChatThread();
	~ChatThread();
	void start(ChatInfo *, DataBase *);
	void run();
	std::thread::id thread_get_id();
	struct event_base *thread_get_base();
	bool thread_read_data(struct bufferevent *, char *);
	void thread_register(struct bufferevent *,Json::Value &);
	void thread_login(struct bufferevent *,Json::Value &);
	void thread_write_data(struct bufferevent *,Json::Value &);	
	void thread_add_friend(struct bufferevent *,Json::Value &);
	void thread_create_group(struct bufferevent *,Json::Value &);
	int thread_compare_string(std::string &,std::string*);
	void thread_private_chat(struct bufferevent *,Json::Value &);
	void thread_join_group(struct bufferevent *,Json::Value &);
	void thread_group_chat(struct bufferevent *,Json::Value &);
	void thread_transfer_file(struct bufferevent *,Json::Value &);
	void thread_client_offline(struct bufferevent *,Json::Value &);
	void thread_get_g_member(struct bufferevent *,Json::Value &);
	static void worker(ChatThread *);
	static void timeout_cb(evutil_socket_t,short,void *);
	static void thread_readcb(struct bufferevent *, void *);
	static void thread_eventcb(struct bufferevent *, short , void *);
	
};








#endif
