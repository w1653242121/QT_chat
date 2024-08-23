#include "groupcreate.h"
#include "ui_groupcreate.h"

GroupCreate::GroupCreate(QString uname,Chatlist *c) :
    ui(new Ui::GroupCreate)
{
    ui->setupUi(this);

    username = uname;
    this->c = c;

}

GroupCreate::~GroupCreate()
{
    delete ui;
}

void GroupCreate::on_cancelButton_clicked()
{
    this->close();
}


void GroupCreate::on_pushButton_clicked()
{
    QString newGName = ui->groupnameEdit->text();

    QJsonObject js_obj;
    js_obj.insert("cmd","creategroup");
    js_obj.insert("groupname",newGName);
    js_obj.insert("owner",username);

    c->chat_send_data(js_obj);
    this->close();
}










