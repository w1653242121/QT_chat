#include "chat_server.h"

ChatServer::ChatServer()
{
	//init event set  "base"
	this->base = event_base_new();
	
	//init database object
	db = new DataBase();

	//初始化数据库表 chat_user   chat_group
	if(!db->database_init_table())
	{
		std::cout<<"database_init_table() error"<<std::endl;
		exit(-1);
	}

	//初始化在线用户信息、群信息数据结构
	info = new ChatInfo();

	//初始化群信息，把群信息从数据库中读取出来，放入map中
	server_update_group_info();

	//初始化线程池
	thread_num = 3;
	cur_thread = 0;
	pool = new ChatThread[thread_num];

	for(int i = 0; i < thread_num; i++)
	{
		pool[i].start(info,db);
	}



	
}

void ChatServer::server_update_group_info()
{
        if(!db->database_connect())
        {
                exit(-1);

        }
        std::string groupinfo[1024];   //群最大数
        int num = db->database_get_group_info(groupinfo);
        std::cout<<"group number is :"<<num<<std::endl;

        db->database_disconnect();
	
	info->list_update_group(groupinfo,num);

	//test
	//info->list_show_group();

}





ChatServer::~ChatServer()
{
	if(db)
	{
		delete db;
	}
}


//create listen object
void ChatServer::listen(const char *ip,int port)
{
	struct sockaddr_in server_info;
	memset(&server_info, 0, sizeof(server_info));
	server_info.sin_family = AF_INET;
	server_info.sin_addr.s_addr = inet_addr(ip);
	server_info.sin_port = htons(port);


	struct evconnlistener* listener = evconnlistener_new_bind(base, listener_cb, this, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, 
		5,(struct sockaddr*)&server_info, sizeof(server_info));

	if (NULL == listener)
	{
		std::cout << "evconnlistener_new_bind error" << std::endl;
		exit(-1);
	}

	//listen set
	event_base_dispatch(base);

	//relese
	evconnlistener_free(listener);
	event_base_free(base);

}

void ChatServer::listener_cb(struct evconnlistener* listener, evutil_socket_t fd,
	struct sockaddr* c, int socklen, void* arg)
{
	ChatServer *ser = (ChatServer *)arg;
	struct sockaddr_in* client_info = (struct sockaddr_in *)c;

	std::cout << "client send connect" << std::endl;
	std::cout << "ip:" << inet_ntoa(client_info->sin_addr);
	std::cout << "      port:" << client_info->sin_port << std::endl;

	//create event , add to Thread Pool
	ser->server_alloc_event(fd);
}


void ChatServer::server_alloc_event(int fd)
{
	struct event_base *t_base = pool[cur_thread].thread_get_base();
	
	struct bufferevent *bev = bufferevent_socket_new(t_base,fd,BEV_OPT_CLOSE_ON_FREE);

	if(NULL == bev)
	{
		std::cout<<"bufferevent_socket_new error"<<std::endl;
		return;
	}

	bufferevent_setcb(bev,ChatThread::thread_readcb,NULL,ChatThread::thread_eventcb,&pool[cur_thread]);
	bufferevent_enable(bev,EV_READ);
	
	cur_thread = (cur_thread+1) % thread_num;

}














