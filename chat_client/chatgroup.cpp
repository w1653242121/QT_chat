#include "chatgroup.h"
#include "ui_chatgroup.h"
#include "chatlist.h"

ChatGroup::ChatGroup(QList<struct group_chat_window> *g ,QString uname,QString gname,Chatlist *c) :
    ui(new Ui::ChatGroup)
{
    ui->setupUi(this);

    glist = g;
    username = uname;
    groupname = gname;
    this->c = c;


    connect(c,&Chatlist::signal_retuen_member,this,&ChatGroup::return_member_reply);
    connect(c,&Chatlist::signal_group_launch_chat,this,&ChatGroup::signal_group_launch_chat_reply);
}


ChatGroup::~ChatGroup()
{
    delete ui;
}


void ChatGroup::return_member_reply(QJsonObject &j)
{
    QString g_member = j.value("member").toString();

    if(g_member.isEmpty())
    {

    }
    else if(!g_member.contains('|'))
    {
        ui->group_chat_listWidget->addItem(g_member);
    }
    else
    {
        QStringList memberlist = g_member.split('|');

        for(int x = 0; x<memberlist.size(); x++)
        {
            if("" != memberlist.at(x))
            {
                ui->group_chat_listWidget->addItem(memberlist.at(x));
            }

        }
    }

}


void ChatGroup::new_group_member(QString newman)
{
    ui->group_chat_listWidget->addItem(newman);
}


void ChatGroup::on_send_infoButton_clicked()
{
    QString info = ui->group_chat_lineEdit->text();

    QJsonObject js_obj;
    js_obj.insert("cmd","groupchat");
    js_obj.insert("groupname",groupname);
    js_obj.insert("username",username);
    js_obj.insert("text",info);
    c->chat_send_data(js_obj);

    QString head = QString("%1:").arg(username);
    info.insert(0,head);

    all_text.append(info);
    all_text.append("\n\n");

    ui->group_chat_textEdit->setText(all_text);
    ui->group_chat_lineEdit->clear();
}







void ChatGroup::on_group_chat_lineEdit_returnPressed()
{
    QString info = ui->group_chat_lineEdit->text();

    QJsonObject js_obj;
    js_obj.insert("cmd","groupchat");
    js_obj.insert("groupname",groupname);
    js_obj.insert("username",username);
    js_obj.insert("text",info);
    c->chat_send_data(js_obj);

    QString head = QString("%1:").arg(username);
    info.insert(0,head);

    all_text.append(info);
    all_text.append("\n\n");

    ui->group_chat_textEdit->setText(all_text);
    ui->group_chat_lineEdit->clear();
}

void ChatGroup::closeEvent(QCloseEvent *CLO)
{
    for(int x = 0; x < glist->size(); x++)
    {
        if(glist->at(x).g == this)
        {
            glist->removeAt(x);
            break;
        }
    }

    this->close();//CLO->accept();
}

void ChatGroup::signal_group_launch_chat_reply(QJsonObject &j)
{
    QString fromwho =  j.value("from").toString();
    QString info = j.value("text").toString();

    QString head = QString("%1:").arg(fromwho);
    info.insert(0,head);

    all_text.append(info);
    all_text.append("\n\n");

    ui->group_chat_textEdit->setText(all_text);

}







