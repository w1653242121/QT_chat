#include "chatlist.h"
#include "ui_chatlist.h"
#include "addfriend.h"
#include "chatprivate.h"
#include "groupcreate.h"
#include "addgroup.h"
#include "chatgroup.h"

#pragma execution_character_set("utf-8")

Chatlist::Chatlist(QTcpSocket *s,QString &flist,QString &glist,QString &uname) :
    ui(new Ui::Chatlist)
{
    ui->setupUi(this);

    socket = s;
    friendlist = flist;
    grouplist = glist;
    username = uname;

    qDebug() << username << friendlist << grouplist;


    QFont qf("宋体",13);
    QSize qs(200,27);
    //群信息
    if(grouplist.isEmpty())
    {

    }
    else if(!grouplist.contains('|'))
    {
        ui->grouplistWidget->addItem(grouplist);
    }
    else
    {
        QStringList Glist = grouplist.split('|');

        for(int x = 0; x<Glist.size(); x++)
        {
            if("" != Glist.at(x))
            {
                ui->grouplistWidget->addItem(Glist.at(x));
            }

        }
    }
    ui->grouplistWidget->setFont(qf);
    ui->grouplistWidget->setGridSize(qs);


    //好友信息
    if(friendlist.isEmpty())
    {

    }
    else if(!friendlist.contains('|'))
    {
        ui->friendlistWidget->addItem(friendlist);
    }
    else
    {
        QStringList Flist = friendlist.split('|');

        for(int x = 0; x<Flist.size(); x++)
        {
            if("" != Flist.at(x))
            {
                ui->friendlistWidget->addItem(Flist.at(x));
            }

        }
    }
    ui->friendlistWidget->setFont(qf);
    ui->friendlistWidget->setGridSize(qs);





    connect(socket,&QTcpSocket::disconnected,this,&Chatlist::chat_server_disconnect);
    connect(socket,&QTcpSocket::readyRead,this,&Chatlist::chat_server_reply);

    connect(ui->friendlistWidget,&QListWidget::itemDoubleClicked,this,&Chatlist::chat_private_chat);
    connect(ui->grouplistWidget,&QListWidget::itemDoubleClicked,this,&Chatlist::chat_group_chat);


}

Chatlist::~Chatlist()
{
    delete ui;
}



void Chatlist::chat_server_reply()
{
    QByteArray ba;
    chat_read_data(ba);

    QJsonObject js_obj = QJsonDocument::fromJson(ba).object();

    QString cmd = js_obj.value("cmd").toString();

    if(cmd == "addfriend_reply")
    {
        chat_addfriend_reply(js_obj);
    }
    else if(cmd == "online")
    {
        chat_online_reply(js_obj);
    }
    else if(cmd == "be_addfriend")
    {
        chat_be_friend_reply(js_obj);
    }
    else if(cmd == "private_reply")
    {
        chat_private_reply(js_obj);
    }
    else if(cmd == "private")
    {
        chat_friend_launch_chat_reply(js_obj);
    }
    else if(cmd == "creategroup_reply")
    {
        chat_creategroup_reply(js_obj);
    }
    else if(cmd == "joingroup_reply")
    {
        chat_addgroup_reply(js_obj);
    }
    else if(cmd == "new_member_join")
    {
        chat_new_member_join_reply(js_obj);
    }
    else if(cmd == "get_g_member_reply")
    {
        emit signal_retuen_member(js_obj);
    }
    else if(cmd == "groupchat_reply")
    {
        chat_groupchat_reply(js_obj);
    }
    else if(cmd == "friend_offline")
    {
        chat_friend_offline(js_obj);
    }
    else if(cmd == "file_reply")
    {
        chat_file_reply(js_obj);
    }
    else if(cmd == "file_name")
    {
        chat_file_name_reply(js_obj);
    }
    else if(cmd == "file_transfer")
    {
        chat_file_transfer_reply(js_obj);
    }
    else if(cmd == "file_end")
    {
        chat_file_end_reply(js_obj);
    }
}




void Chatlist::chat_read_data(QByteArray &ba)
{
    char buf[512] = {0};
    int len;
    int sum = 0;
    int i =1;

    socket->read(buf,4);
    memcpy(&len,buf,4);

    qDebug() <<"data len is : "<<len;

    while(i)
    {
        memset(buf,0,512);
        sum += socket->read(buf,len-sum);
        ba.append(buf);

        if(sum>=len)
        {
            i = 0;
        }


    }
    qDebug() <<"data is : "<<ba;
}

void Chatlist::chat_server_disconnect()
{
    QMessageBox::warning(this,"警告","连接断开");

}

void Chatlist::chat_send_data(QJsonObject &j)
{
    QByteArray data;
    QByteArray ba = QJsonDocument(j).toJson();

    int len = ba.size();

    data.insert(0,(char *)&len,4);
    data.append(ba);

    socket->write(data);
    socket->flush();

}


void Chatlist::on_add_friend_Button_clicked()
{
    Addfriend *a = new Addfriend(username,this);
    a->setWindowTitle("AddFriend");
    a->show();
}


void Chatlist::chat_online_reply(QJsonObject &js_obj)
{
    QString str = QString("[%1]上线").arg(js_obj.value("username").toString());
    QMessageBox::information(this,"好友在线",str);
}

void Chatlist::chat_addfriend_reply(QJsonObject &js_obj)
{
    QString res = js_obj.value("result").toString();
    if(res == "not_exist")
    {
        QMessageBox::warning(this,"添加通知","该用户不存在");
    }
    else if(res == "already_friend")
    {
        QMessageBox::warning(this,"添加通知","和该用户已是好友");
    }
    else if(res == "success")
    {
        QMessageBox::information(this,"添加通知","添加成功");
        ui->friendlistWidget->addItem(js_obj.value("friend").toString());
    }
}


void Chatlist::chat_be_friend_reply(QJsonObject &js_obj)
{
    QString str = QString("[%1]添加你为好友").arg(js_obj.value("friend").toString());
    QMessageBox::information(this,"添加信息",str);

    ui->friendlistWidget->addItem(js_obj.value("friend").toString());
}


void Chatlist::chat_private_chat()
{
    QString fname = ui->friendlistWidget->currentItem()->text();

    for(int x = 0; x < plist.size(); x++)
    {
        if(plist.at(x).friendname == fname)
        {
            plist.at(x).c->activateWindow();
            return;
        }
    }

    //启动聊天窗口
    ChatPrivate *chat_window = new ChatPrivate(&plist,username,fname,this);
    chat_window->setWindowTitle(fname);
    chat_window->show();

    //struct private_chat_window p = {chat_window,fname};
    //plist.push_back(p);

    struct private_chat_window p;
    p.c = chat_window;
    p.friendname = fname;
    plist.push_back(p);

}


void Chatlist::chat_group_chat()
{
    QString gname = ui->grouplistWidget->currentItem()->text();
    QString str = QString("群聊：%1").arg(gname);

    for(int x = 0; x < glist.size(); x++)
    {
        if(glist.at(x).groupname == gname)
        {
            glist.at(x).g->activateWindow();
            return;
        }
    }

    //启动群聊天窗口
    QJsonObject js_obj;
    js_obj.insert("cmd","get_g_member");
    js_obj.insert("groupname",gname);
    chat_send_data(js_obj);

    ChatGroup *group_window = new ChatGroup(&glist,username,gname,this);
    group_window->setWindowTitle(str);
    group_window->show();



    struct group_chat_window g;
    g.g = group_window;
    g.groupname = gname;
    glist.push_back(g);


}

void Chatlist::chat_private_reply(QJsonObject &j)
{
    if(j.value("result").toString() == "offline")
    {
        QMessageBox::warning(this,"提示信息","对方不在线");
    }


}

void Chatlist::chat_friend_launch_chat_reply(QJsonObject &j)
{
    QString fname = j.value("fromfriend").toString();

    for(int x = 0; x < plist.size(); x++)
    {
        if(plist.at(x).friendname == fname)
        {
            plist.at(x).c->activateWindow();
            emit signal_friend_launch_chat(j);
            return;
        }
    }

    //启动聊天窗口
    ChatPrivate *chat_window = new ChatPrivate(&plist,username,fname,this);
    chat_window->setWindowTitle(fname);
    chat_window->show();

    //struct private_chat_window p = {chat_window,fname};
    //plist.push_back(p);

    struct private_chat_window p;
    p.c = chat_window;
    p.friendname = fname;
    plist.push_back(p);

    emit signal_friend_launch_chat(j);
}




void Chatlist::on_create_group_Button_clicked()
{
    GroupCreate *newGroup = new GroupCreate(username,this);
    newGroup->setWindowTitle("create group");
    newGroup->show();
}



void Chatlist::chat_creategroup_reply(QJsonObject &j)
{
    QString res = j.value("result").toString();
    QString gname = j.value("groupname").toString();

    if(res == "exist")
    {
        QMessageBox::warning(this,"建群信息","失败，该群已存在");
    }
    else if(res == "success")
    {
        QMessageBox::information(this,"建群信息","新建群聊成功");
        ui->grouplistWidget->addItem(gname);
    }
}



void Chatlist::on_add_group_Button_clicked()
{
    addgroup *AddGro = new addgroup(username,this);
    AddGro->setWindowTitle("加入群聊");
    AddGro->show();

}

void Chatlist::chat_addgroup_reply(QJsonObject &j)
{
    QString res = j.value("result").toString();
    QString gname = j.value("groupname").toString();

    if(res == "not_exist")
    {
        QMessageBox::warning(this,"加群提示","该群不存在");
    }
    else if(res == "already")
    {
        QMessageBox::warning(this,"加群提示","已经在该群中");
    }else if(res == "success")
    {
        QMessageBox::information(this,"加群提示","加群成功");
    ui->grouplistWidget->addItem(gname);
    }


}


void Chatlist::chat_new_member_join_reply(QJsonObject &j)
{
    QString newMember = j.value("username").toString();
    QString gname = j.value("groupname").toString();

    for(int x = 0; x < glist.size(); x++)
    {
        if(glist.at(x).groupname == gname)
        {
            glist.at(x).g->new_group_member(newMember);
            QString str = QString("[%1]已加入[%2]群聊").arg(newMember).arg(gname);
            QMessageBox::information(this,"群聊新成员",str);
            break;
        }
    }
}



void Chatlist::chat_groupchat_reply(QJsonObject &j)
{
    QString gname = j.value("groupname").toString();

    for(int x = 0; x < glist.size(); x++)
    {
        if(glist.at(x).groupname == gname)
        {
            glist.at(x).g->activateWindow();
            emit signal_group_launch_chat(j);
            return;
        }
    }

    //new window
    QJsonObject js_obj;
    js_obj.insert("cmd","get_g_member");
    js_obj.insert("groupname",gname);
    chat_send_data(js_obj);

    ChatGroup *group_window = new ChatGroup(&glist,username,gname,this);
    QString str = QString("群聊：%1").arg(gname);
    group_window->setWindowTitle(str);
    group_window->show();



    struct group_chat_window g;
    g.g = group_window;
    g.groupname = gname;
    glist.push_back(g);
    emit signal_group_launch_chat(j);


}

void Chatlist::chat_friend_offline(QJsonObject &j)
{
    QString fname = j.value("username").toString();
    QString str = QString("[%1]下线").arg(fname);
    QMessageBox::information(this,"好友下线",str);

}


void Chatlist::closeEvent(QCloseEvent *CLO)
{
    QJsonObject js_obj;
    js_obj.insert("cmd","offline");
    js_obj.insert("username",username);
    chat_send_data(js_obj);

    socket->disconnect(SIGNAL(disconnected()));
    socket->close();

    this->close();


}

void Chatlist::chat_file_reply(QJsonObject &j)
{
    QString res = j.value("result").toString();

    if(res == "offline")
    {
        emit signal_file_offline();
    }
    else if(res == "online")
    {
        emit signal_file_online();
    }
}

void Chatlist::chat_file_name_reply(QJsonObject &j)
{
    QString tem = j.value("filename").toString();
    int i = tem.lastIndexOf('/');
    chat_file.filename = tem.right(tem.length() - i - 1);
    chat_file.size = j.value("filelength").toInt();
    chat_file.fromuser = j.value("fromuser").toString();
    chat_file.myFile = new QFile(chat_file.filename);

    chat_file.myFile->open(QIODevice::WriteOnly);
}

void Chatlist::chat_file_transfer_reply(QJsonObject &j)
{
    QString message = j.value("text").toString();
    QByteArray ba;
    ba.append(message);
    chat_file.myFile->write(ba);
}
void Chatlist::chat_file_end_reply (QJsonObject &j)
{
    chat_file.myFile->close();
    delete chat_file.myFile;

    QString tem = QString("[%1]给您传输了一个文件").arg(chat_file.fromuser);
    QMessageBox::information(this,"接收文件",tem);
}













