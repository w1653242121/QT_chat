#ifndef CHATLIST_H
#define CHATLIST_H

#include <QWidget>
#include <QTcpSocket>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QList>
#include "chatprivate.h"
#include "chatgroup.h"
#include <qdebug.h>
#include <QCloseEvent>
#include <QFile>


namespace Ui {
class Chatlist;
}

struct FileMessage
{
    QString filename;
    QString fromuser;
    int size;
    QFile *myFile;

};

class Chatlist : public QWidget
{
    Q_OBJECT

public:
    explicit Chatlist(QTcpSocket *,QString &,QString &,QString &);

    void chat_send_data(QJsonObject &);

    void closeEvent(QCloseEvent *);

    ~Chatlist();
private slots:
    void chat_server_reply();
    void chat_server_disconnect();
    void on_add_friend_Button_clicked();
    void chat_private_chat();
    void chat_group_chat();

    void on_create_group_Button_clicked();

    void on_add_group_Button_clicked();

private:
    void chat_read_data(QByteArray &);
    void chat_online_reply(QJsonObject &);
    void chat_addfriend_reply(QJsonObject &);
    void chat_be_friend_reply(QJsonObject &);
    void chat_private_reply(QJsonObject &);
    void chat_friend_launch_chat_reply(QJsonObject &);
    void chat_creategroup_reply(QJsonObject &);
    void chat_addgroup_reply(QJsonObject &);
    void chat_new_member_join_reply(QJsonObject &);
    void chat_groupchat_reply(QJsonObject &);
    void chat_friend_offline(QJsonObject &);
    void chat_file_reply(QJsonObject &);
    void chat_file_name_reply(QJsonObject &);
    void chat_file_transfer_reply(QJsonObject &);
    void chat_file_end_reply (QJsonObject &);

signals:
    void signal_friend_launch_chat(QJsonObject &);
    void signal_retuen_member(QJsonObject &);
    void signal_group_launch_chat(QJsonObject &);
    void signal_file_offline();
    void signal_file_online();


private:
    Ui::Chatlist *ui;
    QTcpSocket *socket;
    QString username;
    QString friendlist;
    QString grouplist;
    QList<struct private_chat_window> plist;
    QList<struct group_chat_window> glist;
    struct FileMessage chat_file;
};

#endif // CHATLIST_H
