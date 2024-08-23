#include "addfriend.h"
#include "ui_addfriend.h"

Addfriend::Addfriend(QString &uname,Chatlist *c) :
    ui(new Ui::Addfriend)
{
    ui->setupUi(this);

    username = uname;
    parent = c;
}

Addfriend::~Addfriend()
{
    delete ui;
}

void Addfriend::on_cancelButton_clicked()
{
    this->close();
}


void Addfriend::on_pushButton_clicked()
{
    QString friendname = ui->friendEdit->text();

    QJsonObject js_obj;
    js_obj.insert("cmd","addfriend");
    js_obj.insert("username",username);
    js_obj.insert("friend",friendname);

    parent->chat_send_data(js_obj);
    this->close();
}







