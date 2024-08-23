#include "chat_thread.h"





ChatThread::ChatThread()
{
	base = event_base_new();

	_thread = new std::thread(worker,this);
	_id = _thread->get_id();


}



ChatThread::~ChatThread()
{
	if(_thread)
	{
		delete _thread;
	}	

}




void ChatThread::start(ChatInfo *i,DataBase *d)
{
	this->info = i;
	this->db = d;

}






void ChatThread::worker(ChatThread *t)
{
	t->run();

}


void ChatThread::run()
{
	struct event timeout;
	struct timeval tv;
	
	event_assign(&timeout, base, -1, EV_PERSIST, timeout_cb, this);

	evutil_timerclear(&tv);
	tv.tv_sec = 3;
	event_add(&timeout, &tv);

	std::cout<<"--- thread "<<_id<<" start working..."<<std::endl;

	event_base_dispatch(base);

	event_base_free(base);	
}



void ChatThread::timeout_cb(evutil_socket_t fd, short event,void *arg)
{
	ChatThread *t = (ChatThread *)arg;
	//std::cout<<"--- thread"<<t->thread_get_id()<<"is listening ..."<<std::endl;
}



std::thread::id ChatThread::thread_get_id()
{
	return _id;

}

struct event_base *ChatThread::thread_get_base()
{
	return base;
}


void ChatThread::thread_readcb(struct bufferevent *bev, void *arg)
{
	ChatThread *t = (ChatThread *)arg;

	char buf[1024] = {0};
	if(!t->thread_read_data(bev, buf))
	{
		std::cout<<"thread_read_data error"<<std::endl;
		return;
	}
	
	std::cout<<"---thread "<< t->thread_get_id() <<" receive data ";
	std::cout<<buf<<std::endl;
	
	Json::Reader reader;  //json解析对象
	Json::Value val;

	//判断buf是不是json格式
	if(!reader.parse(buf,val))
	{
		std::cout<<"error buf is not json"<<std::endl;
		return;
	}
	
	if(val["cmd"] == "register")
	{
		t->thread_register(bev,val);
	}
	else if(val["cmd"] == "login")
	{
		t->thread_login(bev,val);	
	}
	else if(val["cmd"] == "addfriend")
	{
		t->thread_add_friend(bev,val);
	}
	else if(val["cmd"] == "private")
	{
		t->thread_private_chat(bev,val);
	}
	else if(val["cmd"] == "creategroup")
	{
		t->thread_create_group(bev,val);
	}
	else if(val["cmd"] == "joingroup")
	{
		t->thread_join_group(bev,val);
	}
	else if(val["cmd"] == "groupchat")
	{
		t->thread_group_chat(bev,val);
	}
	else if(val["cmd"] == "file")
	{
		t->thread_transfer_file(bev,val);
	}
	else if(val["cmd"] == "offline")
	{
		t->thread_client_offline(bev,val);
	}
	else if(val["cmd"] == "get_g_member")
	{
		t->thread_get_g_member(bev,val);
	}

}






void ChatThread::thread_eventcb(struct bufferevent *b, short s, void *a)
{
	if(s & BEV_EVENT_EOF)
	{
		std::cout<<"---disconnect client---"<<std::endl;
		bufferevent_free(b);
	}
	else
	{
		std::cout<<"---unkown error---"<<std::endl;
	}

}


bool ChatThread::thread_read_data(struct bufferevent *bev, char *s)
{
	int size;
	size_t count = 0;

	if(bufferevent_read(bev,&size, 4) != 4)
	{
		return false;
	}

	char buf[1024] = {0};
	while(1)
	{
		//count += bufferevent_read(bev, buf, 1024);
		count += bufferevent_read(bev, buf, size - count);
		strcat(s,buf);
		memset(buf,0,1024);

		if(count >= size)
		{
			break;
		}
	}

	return true;

}



void ChatThread::thread_register(struct bufferevent *bev,Json::Value &v)
{
	db->database_connect();

	//用户是否存在	
	if(db->database_user_exists(v["username"].asString()))
	{
		Json::Value val;
		val["cmd"] = "register_reply";
		val["result"] = "user_exists";

		thread_write_data(bev,val);
	}
	else
	{
		db->database_insert_user_info(v);
		
		Json::Value val;
                val["cmd"] = "register_reply";
                val["result"] = "success";

                thread_write_data(bev,val);
	
	}

	db->database_disconnect();
}


void ChatThread::thread_write_data(struct bufferevent *bev,Json::Value &v)
{
	std::string s = Json::FastWriter().write(v);
	int len = s.size();
	char buf[1024] = {0};

	memcpy(buf,&len,4);
	memcpy(buf+4,s.c_str(),len);

	if(bufferevent_write(bev,buf,len+4) == -1)
	{
		std::cout<<"bufferevent_write error"<<std::endl;
	}
}


void ChatThread::thread_login(struct bufferevent *bev,Json::Value &v)
{
	//是否已在线
	 struct bufferevent *b = info->list_friend_online(v["username"].asString());
	 if(b)
	 {
		Json::Value val;
                val["cmd"] = "login_reply";
                val["result"] = "online";

                thread_write_data(bev,val);
	 	return;
	 }


	db->database_connect();

	if(!db->database_user_exists(v["username"].asString()))
	{	//用户不存在
		Json::Value val;
		val["cmd"] = "login_reply";
		val["result"] = "not_exist";

		thread_write_data(bev,val);

		db->database_disconnect();
		return;
	}

	//用户存在
	if(!db->database_password_check(v))
	{	//密码错误
		Json::Value val;
                val["cmd"] = "login_reply";
                val["result"] = "password_error";

                thread_write_data(bev,val);

                db->database_disconnect();
                return;
	
	}
	
	//get friend list and group list
	std::string friendlist,grouplist;
	if(!db->database_get_friend_and_group(v,friendlist,grouplist))
	{
		std::cout<<"db->database_get_friend_and_group error"<<std::endl;
		return;
	}
	db->database_disconnect();


	//回复客户端
	Json::Value val;
	val["cmd"] = "login_reply";
	val["result"] = "success";
	val["friendlist"] = friendlist;
	val["grouplist"] = grouplist;
	val["username"] = v["username"];
	
	thread_write_data(bev,val);

	std::cout<<"登录成功"<<val << std::endl;

	//更新在线用户
	info->list_update_list(v,bev);

	//通知好友
	if(friendlist.empty())
	{
		return;
	}

	int idx = friendlist.find('|');
	int start =0;

	while(idx != -1)
	{
		std::string name = friendlist.substr(start,idx-start);
		
		//send online info to friend
		struct bufferevent *b = info->list_friend_online(name);
		if(NULL != b)
		{
			Json::Value val;
                	val["cmd"] = "online";
                	val["username"] = v["username"];

                	thread_write_data(b,val);
		}
		
		start = idx+1;
		idx = friendlist.find('|',idx+1);


	}
	std::string name = friendlist.substr(start,idx-start);

	b = info->list_friend_online(name);
                if(NULL != b)
                {
                        val.clear();
        		val["cmd"] = "online";
        		val["username"] = v["username"];

        		thread_write_data(b,val);
                }
}


void ChatThread::thread_add_friend(struct bufferevent *bev,Json::Value &v)
{
	//自己加自己
	if(v["friend"] == v["username"])
	{
		return;
	}
	db->database_connect();
	//判断好友是否存在
	if(!db->database_user_exists(v["friend"].asString()))
	{
		Json::Value val;
		val["cmd"] = "addfriend_reply";
		val["result"] = "not_exist";

		thread_write_data(bev,val);
		db->database_disconnect();
		return;
	} 
	//判断是否是好友关系
	std::string friendlist,grouplist;
	if(db->database_get_friend_and_group(v,friendlist,grouplist))
	{
		std::string str[1024];
		int num = thread_compare_string(friendlist,str);	
		
		for(int i = 0; i < num; i++)
		{
			if(str[i] == v["friend"].asString())
			{
				Json::Value val;
				val["cmd"] = "addfriend_reply";
				val["result"] = "already_friend";

				thread_write_data(bev,val);

				db->database_disconnect();
				return;
			}
		}

	}

	
	
	//在数据库中相互添加
	db->database_add_friend(v);	
	
	db->database_disconnect();
	//回复好友
	Json::Value val;
	val["cmd"] = "be_addfriend";
	val["friend"] = v["username"];
	
	struct bufferevent *b = info->list_friend_online(v["friend"].asString());
	if(NULL != b)
	{
		thread_write_data(b, val);
	}


	//回复自己
	val.clear();
	val["cmd"] = "addfriend_reply";
	val["result"] = "success";
	val["friend"] = v["friend"];

	thread_write_data(bev, val);



}





int ChatThread::thread_compare_string(std::string &f,std::string *s)
{
	int count = 0;
	int start = 0;
	int idx = f.find('|');

	while(idx != -1)
	{
		s[count++] = f.substr(start,idx - start);


		start = idx + 1;
		idx = f.find('|',start);
	}

	s[count++] = f.substr(start);

	return count;
}


void ChatThread::thread_private_chat(struct bufferevent *bev,Json::Value &v)
{
	//判断对方是否在线
	std::string name = v["tofriend"].asString();
	struct bufferevent *b = info->list_friend_online(name);
	if(NULL == b)
	{
		Json::Value val;
		val["cmd"] = "private_reply";
		val["result"] = "offline";

		thread_write_data(bev,val);
		return;
	}

	//转发内容
	
	Json::Value val;
	val["cmd"] = "private";
	val["fromfriend"] = v["username"];
	val["text"] = v["text"];

	thread_write_data(b,val);

}


void ChatThread::thread_create_group(struct bufferevent *bev,Json::Value &v)
{
	//判断群是否存在
	std::string groupname = v["groupname"].asString();
	if(info->list_group_exist(groupname))
	{
		Json::Value val;
		val["cmd"] = "creategroup_reply";
		val["result"] = "exist";
		val["groupname"] = v["groupname"];

		thread_write_data(bev,val);
		return;	
	}
	
	//修改数据库
	db->database_connect();

	db->database_add_new_group(groupname,v["owner"].asString());

	db->database_disconnect();

	//修改map
	info->list_add_new_group(groupname,v["owner"].asString());	
	
	//回复客户端
	Json::Value val;
	val["cmd"] = "creategroup_reply";
	val["result"] = "success";
	val["groupname"] = v["groupname"];

	thread_write_data(bev,val);
}


void ChatThread::thread_join_group(struct bufferevent *bev,Json::Value &v)
{
	//群是否存在
	std::string groupname = v["groupname"].asString();
	std::string username = v["username"].asString();
        if(!info->list_group_exist(groupname))
        {
                Json::Value val;
                val["cmd"] = "joingroup_reply";
                val["result"] = "not_exist";

                thread_write_data(bev,val);
                return;
        }
	//是否已经在群中
	std::cout<<info->list_member_is_group(groupname,username)<<std::endl;
	if(info->list_member_is_group(groupname,username))
	{
		Json::Value val;
                val["cmd"] = "joingroup_reply";
                val["result"] = "already";

                thread_write_data(bev,val);
                return;	
	}
	
	//修改数据库
	db->database_connect();

	db->database_updata_group_member(groupname,username);

	db->database_disconnect();
	
	//修改map
	info->list_update_group_member(groupname,username);	
	
	//通知群成员
	std::list<std::string> l = info->list_get_list(groupname);
	struct bufferevent *b;
	std::string member;
	for(auto it = l.begin(); it != l.end(); it++)
	{
		if(*it == username)
		{
			continue;
		}
		
		member += *it;
		member += "|";

		b = info->list_friend_online(*it);
		if(NULL != b)
		{
			Json::Value val;
			val["cmd"] = "new_member_join";
			val["groupname"] = groupname;
			val["username"] = username;

			thread_write_data(b,val);
		}
	
	}
	
	member.erase(member.size() - 1);

	//通知本人
	Json::Value val;
	val["cmd"] = "joingroup_reply";
	val["result"] = "success";
	val["member"] = member;
	val["groupname"] = groupname;
	thread_write_data(bev,val);
}



void ChatThread::thread_group_chat(struct bufferevent *bev,Json::Value &v)
{
	std::string groupname = v["groupname"].asString();
	std::string username = v["username"].asString();
	struct bufferevent *b;
	std::list<std::string> l = info->list_get_list(groupname); //获取群成员
	

        for(auto it = l.begin(); it != l.end(); it++)        	//转发信息
        {
		if(*it == username)
                {
                        continue;
                }
                b = info->list_friend_online(*it);
                
		if(NULL == b)
		{
			continue;
		}
		
		Json::Value val;
            	val["cmd"] = "groupchat_reply";
            	val["groupname"] = groupname;
		val["from"] = v["username"];
            	val["text"] = v["text"];

            	thread_write_data(b,val);
        }
}


void ChatThread::thread_transfer_file(struct bufferevent *bev,Json::Value &v)
{
	std::string friendname = v["friendname"].asString();
	Json::Value val;
	
	struct bufferevent *b = info->list_friend_online(friendname);
	if(!b)
	{
		val["cmd"] = "file_reply";
		val["result"] = "offline";

		thread_write_data(bev,val);
		return;
	}

	val["cmd"] = "file_reply";
      	val["result"] = "online";

        thread_write_data(bev,val);
	
	if(v["step"] == "1")
	{
		//传输文件的属性
		val["cmd"] = "file_name";
		val["filename"] = v["filename"];
		val["filelength"] = v["filelength"];
		val["fromuser"] = v["username"];
	}
	else if(v["step"] == "2")
	{
		//传输文件
		val["cmd"] = "file_transfer";
		val["text"] = v["text"];

	}
	else if(v["step"] == "3")
	{
		//结束
		val["cmd"] = "file_end";
		bufferevent_free(bev);
	}

	thread_write_data(b,val);
}


void ChatThread::thread_client_offline(struct bufferevent *bev,Json::Value &v)
{
	//删除在线用户链表
	std::string username = v["username"].asString();
	info->list_delete_user(username);
	
	//free bufferevent
	bufferevent_free(bev);	
	
	//通知好友
	db->database_connect();
	
	std::string friendlist,grouplist;

	db->database_get_friend_and_group(v,friendlist,grouplist);

	db->database_disconnect();

	if(friendlist.empty())
	{
		return;
	}

	std::string str[1024];
        int num = thread_compare_string(friendlist,str);
 	struct bufferevent *b;

	for(int i = 0; i < num; i++)
        {
		b = info->list_friend_online(str[i]);
		
		Json::Value val;
   	        val["cmd"] = "friend_offline";
               	val["username"] = v["username"];
	
		if(b)
		{
			thread_write_data(b,val);
		}
		
	}	

	std::cout<<"---disconnect client---"<<std::endl;


}

void ChatThread::thread_get_g_member(struct bufferevent *bev,Json::Value &v)
{
	std::string gname = v["groupname"].asString();
	std::string groupmember;

	db->database_connect();

	db->database_get_g_member(gname,groupmember);

	db->database_disconnect();
	
	Json::Value val;
	val["cmd"] = "get_g_member_reply";
	val["member"] = groupmember;
	thread_write_data(bev,val);

}








