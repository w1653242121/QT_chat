#include "widget.h"
#include "ui_widget.h"
#include "chatlist.h"

#pragma execution_character_set("utf-8")

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    //init
    socket = new QTcpSocket();

    //输入密码时，密码隐藏
    ui->passwordEdit->setEchoMode(QLineEdit::Password);

    //connect
    socket->connectToHost(QHostAddress(IP),PORT);

    connect(socket,&QTcpSocket::disconnected,this,&Widget::client_disconnect);
    connect(socket,&QTcpSocket::connected,this,&Widget::client_connect_success);
    connect(socket,&QTcpSocket::readyRead,this,&Widget::client_reply_info);

}

Widget::~Widget()
{
    delete ui;
}


void Widget::client_connect_success()
{
    QMessageBox::information(this,"通知","连接成功");

}


void Widget::client_disconnect()
{
    QMessageBox::warning(this,"通知","断开连接");

}


//处理服务器返回的消息
void Widget::client_reply_info()
{
    QByteArray ba;
    client_read_data(ba);

    QJsonObject js_obj = QJsonDocument::fromJson(ba).object();

    QString cmd = js_obj.value("cmd").toString();

    if(cmd == "register_reply")
    {
        client_register_manage(js_obj);
    }
    else if(cmd == "login_reply")
    {
        client_login_manage(js_obj);
    }







}





void Widget::on_registerButton_clicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();

    QJsonObject js_obj;
    js_obj.insert("cmd","register");
    js_obj.insert("username",username);
    js_obj.insert("password",password);

    client_send_data(js_obj);
}


void Widget::client_send_data(QJsonObject &j)
{
    QByteArray data;
    QByteArray ba = QJsonDocument(j).toJson();

    int len = ba.size();

    data.insert(0,(char *)&len,4);
    data.append(ba);

    socket->write(data);

}


void Widget::client_read_data(QByteArray &ba)
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


void Widget::client_register_manage(QJsonObject &js_obj)
{
    QString result = js_obj.value("result").toString();

    if("user_exists" == result)
    {
        QMessageBox::warning(this,"注册通知","注册失败,该用户已存在");
    }
    else if("success" == result)
    {
        QMessageBox::information(this,"注册通知","注册成功");

    }

}



void Widget::on_loginButton_clicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();

    QJsonObject js_obj;
    js_obj.insert("cmd","login");
    js_obj.insert("username",username);
    js_obj.insert("password",password);

    client_send_data(js_obj);
}



void Widget::client_login_manage(QJsonObject &js_obj)
{
    QString result = js_obj.value("result").toString();

    if("not_exist" == result)
    {
        QMessageBox::warning(this,"登录通知","登录失败,该用户不存在");
    }
    else if("online" == result)
    {
        QMessageBox::warning(this,"登录通知","登录失败,该用户已登录在线");
    }
    else if("password_error" == result)
    {
        QMessageBox::warning(this,"登录通知","登录失败,密码错误");

    }
    else if("success" == result)
    {
        //切换新界面
        QString username = js_obj.value("username").toString();
        QString friendlist = js_obj.value("friendlist").toString();
        QString grouplist = js_obj.value("grouplist").toString();

        this->hide();

        socket->disconnect(SIGNAL(readyRead()));
        socket->disconnect(SIGNAL(disconnected()));

        Chatlist *c = new Chatlist(socket,friendlist,grouplist,username);
        c->setWindowTitle(username);
        c->show();

    }

}











