#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QHostAddress>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <string.h>
//#include <QDebug>




#define IP "47.120.55.0"
#define PORT 8002


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();


private slots:
    void client_connect_success();//连接成功
    void client_disconnect();//断开连接
    void client_reply_info();//接收消息


    void on_registerButton_clicked();//点击注册按钮
    void on_loginButton_clicked();//点击登录按钮

private:
    void client_send_data(QJsonObject &);//发送数据给服务器
    void client_read_data(QByteArray &);//读取解析服务器返回的数据
    void client_register_manage(QJsonObject &);//注册管理
    void client_login_manage(QJsonObject &);//登录管理


private:
    Ui::Widget *ui;
    QTcpSocket *socket;


};
#endif // WIDGET_H











