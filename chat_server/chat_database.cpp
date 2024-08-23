#include "chat_database.h"



DataBase::DataBase()
{

}


DataBase::~DataBase()
{

}





bool DataBase::database_connect()
{
	//init database
	mysql = mysql_init(NULL);
	//connect database
	mysql = mysql_real_connect(mysql,"localhost","root","root","chat_database",0,NULL,0);

	if(NULL == mysql)
	{
		std::cout<<"mysql_real_connect  error"<<std::endl;
		return false;
	}
	
	//set utf-8
	if (mysql_query(mysql,"set names utf8;") != 0 )
	{
		std::cout<<"mysql_query  set names utf8 error"<<std::endl;
		return false;
	}
	
	return true;


}


void DataBase::database_disconnect()
{
	mysql_close(mysql);
}


bool DataBase::database_init_table()
{
	database_connect();

	const char *sql_1 = "create table if not exists chat_group(groupname varchar(128),groupowner varchar(128),groupmember varchar(4096))charset utf8;";
	
	if(mysql_query(mysql,sql_1) != 0)
	{
		return false;
	}

	const char *sql_2 = "create table if not exists chat_user(username varchar(128),password varchar(128),friendlist varchar(4096),grouplist varchar(4096))charset utf8;";
	
	if(mysql_query(mysql,sql_2) != 0)
        {
                return false;
        }


	database_disconnect();

	return true;
}






int DataBase::database_get_group_info(std::string *g)
{
        if(mysql_query(mysql,"select * from chat_group;") != 0)
        {
                std::cout<<"select error"<<std::endl;
                return -1;
        }

        //结果集
        MYSQL_RES *res = mysql_store_result(mysql);

        if (NULL == res)
        {
                std::cout<<"mysql_store_result error"<<std::endl;
                return -1;
        }

        MYSQL_ROW r;
        int idx = 0;

        while(r = mysql_fetch_row(res))
        {
                g[idx] += r[0];
                g[idx] += "|";
                g[idx] += r[2];
                idx++;
        }
        mysql_free_result(res);
        return idx;
}


bool DataBase:: database_user_exists(std::string u)
{
	char sql[512] = {0};

	sprintf(sql, "select * from chat_user where username='%s';",u.c_str());

	std::unique_lock<std::mutex> lck(_mutex);

	if(mysql_query(mysql,sql)!=0)
	{
		std::cout<<"mysql select error"<<std::endl;
		return true;
	}
	
	MYSQL_RES *res = mysql_store_result(mysql);
	if(NULL == res)
	{
		std::cout<<"mysql_store_resule error"<<std::endl;
		return true;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if(NULL == row)
	{
		
		return false;
	}
	else
	{
		return true;
	}


}

void DataBase::database_insert_user_info(Json::Value &v)
{
	std::string username = v["username"].asString();
	std::string password = v["password"].asString();

	char sql[512] = {0};
	sprintf(sql,"insert into chat_user (username,password) values('%s','%s');",username.c_str(),password.c_str());
	
	std::unique_lock<std::mutex> lck(_mutex);

	if(mysql_query(mysql,sql) != 0)
	{
		std::cout<<"DataBase::database_insert_user_info error"<<std::endl;
	
	}
}


bool DataBase::database_password_check(Json::Value &v)
{
	std::string username = v["username"].asString();
        std::string password = v["password"].asString();
	
	char sql[512] = {0};
	sprintf(sql, "select password from chat_user where username='%s';",username.c_str());

	 std::unique_lock<std::mutex> lck(_mutex);

	if(mysql_query(mysql,sql) != 0)
        {
                std::cout<<"database_password_check error "<<std::endl;
		return false;
        }

	MYSQL_RES *res = mysql_store_result(mysql);
	if(NULL == res)
	{
		std::cout<<"mysql store_result  error"<<std::endl;
	       return false;	
	}
	
	MYSQL_ROW row = mysql_fetch_row(res);
	if(NULL == row)
	{
		std::cout<<"mysql_fetch_row error"<<std::endl;
		return false;
	}
	
	if(!strcmp(row[0],password.c_str()))
	{
		return true;
	}
	else
	{
		return false;
	}

}



bool DataBase::database_get_friend_and_group(Json::Value &v,std::string &fList,std::string &gList)
{
	std::string username = v["username"].asString();
	char sql[1024] = {0};

	sprintf(sql,"select * from chat_user where username='%s';",username.c_str());

	 std::unique_lock<std::mutex> lck(_mutex);

	if(mysql_query(mysql,sql) !=0)
	{
		std::cout<<"get friend and group select * error"<<std::endl;
		return false;
	}

	MYSQL_RES *res = mysql_store_result(mysql);
	if(NULL == res)
	{
		std::cout<<"get friend and group res error"<<std::endl;
		return false;
	}
	
	MYSQL_ROW row = mysql_fetch_row(res);
	if(NULL == row)
        {
                std::cout<<"get friend and group row error"<<std::endl;
                return false;
        }

	if(row[2])
	{
	fList = std::string(row[2]);
	}

	if(row[3])
	{
	gList = std::string(row[3]);
	}

	return true;
}



void DataBase::database_add_friend(Json::Value &v)
{
	std::string username = v["username"].asString();
	std::string friendname = v["friend"].asString();

	database_update_friendlist(username,friendname);
	database_update_friendlist(friendname,username);


}


void DataBase::database_update_friendlist(std::string &u,std::string &f)
{
	std::unique_lock<std::mutex> lck(_mutex);

	char sql[1024] = {0};
	sprintf(sql,"select friendlist from chat_user where username='%s';",u.c_str());

	if(mysql_query(mysql,sql) != 0)
	{
		std::cout<<"database_update_friendlist mysql_query error"<<std::endl;
		return;
	}
	
	MYSQL_RES *res = mysql_store_result(mysql);
	if(NULL == res)
	{
		std::cout<<"database_update_friendlist res error"<<std::endl;
		return;
	
	}

	std::string friendlist;
	MYSQL_ROW row = mysql_fetch_row(res);
	if(NULL == row[0])
	{

		friendlist.append(f);
	
	}
	else
	{
		friendlist.append(row[0]);
		friendlist.append("|");
		friendlist.append(f);
	}

	memset(sql,0,sizeof(sql));
	sprintf(sql,"update chat_user set friendlist = '%s' where username = '%s';",friendlist.c_str(),u.c_str());
	
	if(mysql_query(mysql,sql) != 0)
	{
		std::cout<<"update friendlist error"<<std::endl;
	}

}


void DataBase::database_add_new_group(std::string groupname,std::string owner)
{
	std::unique_lock<std::mutex> lck(_mutex);
	//修改group表
	char sql[512] = {0};
	sprintf(sql,"insert into chat_group values ('%s','%s','%s');",groupname.c_str(),owner.c_str(),owner.c_str());

	if(mysql_query(mysql,sql) != 0)
	{
		std::cout<<"insert group error"<<std::endl;
		return;
	}

	//修改user表	
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select grouplist from chat_user where username = '%s';",owner.c_str());

	if(mysql_query(mysql,sql) != 0)
	{
		std::cout<<" mysql_query error"<<std::endl;
		return;
	}
	
	MYSQL_RES *res = mysql_store_result(mysql);
	if(NULL == res)
	{
		std::cout<<" res error"<<std::endl;
		return;
	
	}

	std::string grouplist;
	MYSQL_ROW row = mysql_fetch_row(res);
	if(NULL == row[0])
	{

		grouplist.append(groupname);
	
	}
	else
	{
		grouplist.append(row[0]);
		grouplist.append("|");
		grouplist.append(groupname);
	}

	memset(sql,0,sizeof(sql));
	sprintf(sql,"update chat_user set grouplist = '%s' where username = '%s';",grouplist.c_str(),owner.c_str());
	
	if(mysql_query(mysql,sql) != 0)
	{
		std::cout<<"update friendlist error"<<std::endl;
	}

	
	

}


void DataBase::database_updata_group_member(std::string gname,std::string uname)
{
	std::unique_lock<std::mutex> lck(_mutex);

        char sql[1024] = {0};
        sprintf(sql,"select groupmember from chat_group where groupname='%s';",gname.c_str());

        if(mysql_query(mysql,sql) != 0)
        {
                std::cout<<"database_updata_group_member mysql_query error"<<std::endl;
                return;
        }

        MYSQL_RES *res = mysql_store_result(mysql);
        if(NULL == res)
        {
                std::cout<<"database_updata_group_member res error"<<std::endl;
                return;

        }

        std::string memberlist;
        MYSQL_ROW row = mysql_fetch_row(res);
        if(NULL == row[0])
	{
	
	}
	else
	{
		memberlist.append(row[0]);
		memberlist.append("|");
		memberlist.append(uname);
	}
	
	mysql_free_result(res);

	//更新
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update chat_group set groupmember='%s' where groupname='%s';",memberlist.c_str(),gname.c_str());

	if(mysql_query(mysql,sql) != 0)
	{
		std::cout<<"update chat_group error"<<std::endl;
	}

	//更新user表
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select grouplist from chat_user where username='%s';",uname.c_str());

        if(mysql_query(mysql,sql) != 0)
        {
                std::cout<<"database_updata_group_member mysql_query error"<<std::endl;
                return;
        }

        res = mysql_store_result(mysql);
        if(NULL == res)
        {
                std::cout<<"database_updata_group_member res error"<<std::endl;
                return;

        }

	std::string newGroupName;
        row = mysql_fetch_row(res);
        if(NULL == row[0])
        {
		newGroupName.append(gname);
        }
        else
        {
                newGroupName.append(row[0]);
                newGroupName.append("|");
                newGroupName.append(gname);
        }
	
	memset(sql,0,sizeof(sql));
        sprintf(sql,"update chat_user set grouplist='%s' where username='%s';",newGroupName.c_str(),uname.c_str());

        if(mysql_query(mysql,sql) != 0)
        {
                std::cout<<"update chat_group error"<<std::endl;
        }


}

void DataBase::database_get_g_member(std::string gname,std::string &member)
{
	std::unique_lock<std::mutex> lck(_mutex);

        char sql[1024] = {0};
        sprintf(sql,"select groupmember from chat_group where groupname='%s';",gname.c_str());

        if(mysql_query(mysql,sql) != 0)
        {
                std::cout<<"database_update_friendlist mysql_query error"<<std::endl;
                return;
        }

        MYSQL_RES *res = mysql_store_result(mysql);
        if(NULL == res)
        {
                std::cout<<"database_update_friendlist res error"<<std::endl;
                return;

        }

        MYSQL_ROW row = mysql_fetch_row(res);

	member = row[0];



}

