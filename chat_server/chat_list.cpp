#include "chat_list.h"

ChatInfo::ChatInfo()
{
	online_user = new std::list<User>;
	group_info = new std::map<std::string, std::list<std::string> >;
}



ChatInfo::~ChatInfo()
{
	if(online_user)
	{
		delete online_user;
	}
	if(group_info)
	{
		delete group_info;
	}
}

void ChatInfo::list_update_group(std::string *g,int size)
{
	int idx;
	int start=0;
	std::string groupname;
	std::string membername;
	std::list<std::string> l;

	for(int i=0; i<size; i++)
	{
		idx = g[i].find('|');
		groupname = g[i].substr(0,idx);
		//std::cout<<groupname<<std::endl;
		start = idx +1;

		while(1)
		{
			idx = g[i].find('|',idx+1);
			if(idx == -1) break;
			membername = g[i].substr(start, idx-start);
			l.push_back(membername);
			start = idx +1;
		}
		
		membername = g[i].substr(start, idx-start);
                l.push_back(membername);
	
		group_info->insert(std::pair<std::string, std::list<std::string> >(groupname,l));
		l.clear();

	}

	

}

void ChatInfo::list_show_group()
{
	for(auto it=group_info->begin(); it!=group_info->end(); it++)
	{
		std::cout<<it->first<<"  ";
		for(auto i =it->second.begin();i!=it->second.end();i++)
		{
			std::cout<< *i << " ";
		}
		std::cout<<std::endl;
	}

}


bool ChatInfo::list_update_list(Json::Value &v,struct bufferevent *bev)
{
	struct User u = {v["username"].asString(),bev};

	std::unique_lock<std::mutex> lck(list_mutex);

	online_user->push_back(u);
	return true;
}

struct bufferevent * ChatInfo::list_friend_online(std::string n)
{
	std::unique_lock<std::mutex> lck(list_mutex);
	
	for(auto it = online_user->begin(); it != online_user->end();it++)
	{
		if(it->name == n)
		{
			return it->bev;
		}
	
	}
	return NULL;


}


bool ChatInfo::list_group_exist(std::string name)
{
	std::unique_lock<std::mutex> lck(map_mutex);

	for(auto it = group_info->begin(); it != group_info->end(); it++)
	{
		if(it->first == name)
		{
			return true;
		}
	
	}
	
	return false;
}


void ChatInfo::list_add_new_group(std::string gname,std::string owner)
{
	std::list<std::string> l;
	l.push_back(owner);

	std::unique_lock<std::mutex> lck(map_mutex);
	group_info->insert(make_pair(gname,l));
}


bool ChatInfo::list_member_is_group(std::string gname,std::string uname)
{
	std::unique_lock<std::mutex> lck(map_mutex);	
	for(auto it = group_info->begin(); it != group_info->end(); it++)
        {
                if(it->first == gname)
                {
                        for(auto i = it->second.begin(); i != it->second.end(); i++)
			{
				if(*i == uname)
				{
					return true;
				}
			}
			return false;
                }

        }
	return false;
}


void ChatInfo::list_update_group_member(std::string gname,std::string uname)
{
	std::unique_lock<std::mutex> lck(map_mutex);

	for(auto it = group_info->begin(); it != group_info->end(); it++)
	{
		if(it->first == gname)
		{
			it->second.push_back(uname);
		}
	
	}

}

std::list<std::string> &ChatInfo::list_get_list(std::string gname)
{
	auto it = group_info->begin();
	
	std::unique_lock<std::mutex> lck(map_mutex);

        for(; it != group_info->end(); it++)
	{
		if(it->first == gname)
		{
			break;
		}
	
	}	
	return it->second;

}


 void ChatInfo::list_delete_user(std::string uname)
{
	std::unique_lock<std::mutex> lck(list_mutex);

	for(auto it = online_user->begin(); it != online_user->end(); it++)
	{
		if(it->name == uname)
		{
			online_user->erase(it);
			return;
		}
	}	

}










