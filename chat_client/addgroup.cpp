#include "addgroup.h"
#include "ui_addgroup.h"

addgroup::addgroup(QString uname,Chatlist *c) :
    ui(new Ui::addgroup)
{
    ui->setupUi(this);

    username = uname;
    this->c = c;
}

addgroup::~addgroup()
{
    delete ui;
}


void addgroup::on_cancelButton_clicked()
{
    this->close();
}


void addgroup::on_pushButton_clicked()
{
    QString Gname = ui->groupnameEdit->text();
/*
    if(Gname == NULL)
    {
        QMessageBox::warning(this,"添加群聊提示","群名不能为空");
        return;
    }
*/
    QJsonObject js_obj;
    js_obj.insert("cmd","joingroup");
    js_obj.insert("username",username);
    js_obj.insert("groupname",Gname);

    c->chat_send_data(js_obj);
    this->close();


}

