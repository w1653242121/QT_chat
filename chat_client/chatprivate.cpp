#include "chatprivate.h"
#include "ui_chatprivate.h"
#include "chatlist.h"
#include <QFileDialog>
#include <QThread>
#include "filetransmit.h"

#pragma execution_character_set("utf-8")s

ChatPrivate::ChatPrivate(QList<struct private_chat_window> *p,QString uname,QString fname,Chatlist *c) :
    ui(new Ui::ChatPrivate)
{
    ui->setupUi(this);

    this->plist = p;
    this->username = uname;
    this->friendname = fname;
    this->m_c = c;


    connect(c,&Chatlist::signal_friend_launch_chat,this,&ChatPrivate::signal_friend_launch_chat_reply);
    connect(c,&Chatlist::signal_file_offline,this,&ChatPrivate::signal_file_offline_slots);
    connect(c,&Chatlist::signal_file_online,this,&ChatPrivate::signal_file_online_slots);

}

ChatPrivate::~ChatPrivate()
{
    delete ui;
}

void ChatPrivate::closeEvent(QCloseEvent *CLO)
{
    for(int x = 0; x < plist->size(); x++)
    {
        if(plist->at(x).c == this)
        {
            plist->removeAt(x);
            break;
        }
    }

    this->close();
}



void ChatPrivate::on_sendinfoButton_clicked()
{
    QString info = ui->privatelineEdit->text();

    QJsonObject js_obj;
    js_obj.insert("cmd","private");
    js_obj.insert("text",info);
    js_obj.insert("username",username);
    js_obj.insert("tofriend",friendname);

    m_c->chat_send_data(js_obj);

    QString head = QString("%1:").arg(username);
    info.insert(0,head);

    all_text.append(info);
    all_text.append("\n\n");

    ui->privatetextEdit->setText(all_text);
    ui->privatelineEdit->clear();
}





void ChatPrivate::on_privatelineEdit_returnPressed()
{
    QString info = ui->privatelineEdit->text();

    QJsonObject js_obj;
    js_obj.insert("cmd","private");
    js_obj.insert("text",info);
    js_obj.insert("username",username);
    js_obj.insert("tofriend",friendname);

    m_c->chat_send_data(js_obj);

    QString head = QString("%1:").arg(username);
    info.insert(0,head);

    all_text.append(info);
    all_text.append("\n\n");

    ui->privatetextEdit->setText(all_text);
    ui->privatelineEdit->clear();
}



void ChatPrivate::signal_friend_launch_chat_reply(QJsonObject &j)
{

    QString info = j.value("text").toString();
    QString fromwho = j.value("fromfriend").toString();

    QString head = QString("%1:").arg(fromwho);
    info.insert(0,head);

    all_text.append(info);
    all_text.append("\n\n");

    ui->privatetextEdit->setText(all_text);



}



void ChatPrivate::on_sendfileButton_clicked()
{
    filename = QFileDialog::getOpenFileName(this,"选择文件",QCoreApplication::applicationFilePath());

    if(filename.isEmpty())
    {
        return;
    }

    QFile *file = new QFile(filename);
    file->open(QIODevice::ReadOnly);
    int filelength = file->size();

    QJsonObject js_obj;
    js_obj.insert("cmd","file");
    js_obj.insert("step","1");
    js_obj.insert("filename",filename);
    js_obj.insert("filelength",filelength);
    js_obj.insert("friendname",friendname);
    js_obj.insert("username",username);

    file->close();
    m_c->chat_send_data(js_obj);

}

void ChatPrivate::signal_file_offline_slots()
{
    QMessageBox::warning(this,"传输文件通知","失败,对方不在线");
}


void ChatPrivate::signal_file_online_slots()
{
    QThread *file_thread = new QThread;

    FileTransmit *my_file_transmit = new FileTransmit(filename,friendname);

    my_file_transmit->moveToThread(file_thread);

    file_thread->start();


    connect(this,&ChatPrivate::signal_start_file_woking,my_file_transmit,&FileTransmit::start_working);
    emit signal_start_file_woking();

    connect(my_file_transmit,&FileTransmit::signal_transmit_finish,this,[=]()
    {
        file_thread->quit();
        file_thread->wait();
        file_thread->deleteLater();
        my_file_transmit->deleteLater();

        QMessageBox::information(this,"传输文件通知","文件已发送");

    });
    connect(my_file_transmit,&FileTransmit::signal_connect_fail,this,&ChatPrivate::signal_connect_fail_slots);



}


void ChatPrivate::signal_connect_fail_slots()
{
    QMessageBox::warning(this,"传输文件","传输失败,连接超时");
}





















