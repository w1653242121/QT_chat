#ifndef CHATPRIVATE_H
#define CHATPRIVATE_H

#include <QWidget>
#include <QList>
#include <QCloseEvent>
#include <QJsonDocument>
#include <QJsonObject>

namespace Ui {
class ChatPrivate;
}

class Chatlist;
class ChatPrivate;

struct private_chat_window
{
    ChatPrivate *c;
    QString friendname;
};

class ChatPrivate : public QWidget
{
    Q_OBJECT

public:
    explicit ChatPrivate(QList<struct private_chat_window> * ,QString,QString,Chatlist *c);
    void closeEvent(QCloseEvent *);
    ~ChatPrivate();

private slots:
    void on_sendinfoButton_clicked();

    void on_privatelineEdit_returnPressed();

    void signal_friend_launch_chat_reply(QJsonObject &);

    void on_sendfileButton_clicked();

    void signal_file_offline_slots();

    void signal_file_online_slots();

    void signal_connect_fail_slots();



signals:
    void signal_start_file_woking();



private:
    Ui::ChatPrivate *ui;
    QList<struct private_chat_window> *plist;
    QString friendname;
    QString username;
    Chatlist *m_c;
    QString all_text;
    QString filename;
};

#endif // CHATPRIVATE_H
