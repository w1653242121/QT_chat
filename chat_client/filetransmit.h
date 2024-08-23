#ifndef FILETRANSMIT_H
#define FILETRANSMIT_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>


#define IP "47.120.55.0"
#define PORT 8002


class FileTransmit : public QObject
{
    Q_OBJECT
public:
    explicit FileTransmit(QString,QString);

signals:
    void signal_connect_fail();
    void signal_transmit_finish();


public slots:
    void start_working();  //线程开始工作


private:
    void filetransmit_send_data(QJsonObject &);


private:
    QString filename;
    QString friendname;
    QTcpSocket *file_socket;



};

#endif // FILETRANSMIT_H
