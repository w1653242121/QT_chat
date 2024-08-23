#ifndef CHATGROUP_H
#define CHATGROUP_H

#include <QWidget>
#include <QList>
#include <QCloseEvent>
#include <QJsonDocument>
#include <QJsonObject>

namespace Ui {
class ChatGroup;
}



class ChatGroup;
class Chatlist;

struct group_chat_window
{
    ChatGroup *g;
    //QString memberlist;
    QString groupname;
};


class ChatGroup : public QWidget
{
    Q_OBJECT

public:
    explicit ChatGroup(QList<struct group_chat_window> * ,QString,QString,Chatlist *);
    ~ChatGroup();

    void new_group_member(QString );
    void closeEvent(QCloseEvent *);

private slots:
    void return_member_reply(QJsonObject &);

    void on_send_infoButton_clicked();

    void on_group_chat_lineEdit_returnPressed();

    void signal_group_launch_chat_reply(QJsonObject &);

private:
    Ui::ChatGroup *ui;
    QList<struct group_chat_window> *glist;
    QString username;
    QString groupname;
    Chatlist *c;
    QString all_text;


};

#endif // CHATGROUP_H
