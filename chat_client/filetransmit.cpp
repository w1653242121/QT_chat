#include "filetransmit.h"
#include <QHostAddress>
#include <QFile>
#include <QTimer>
#include <QThread>


FileTransmit::FileTransmit(QString filename,QString friendname)
{
    this->filename = filename;
    this->friendname = friendname;

}


void FileTransmit::start_working()
{
    file_socket = new QTcpSocket;

    file_socket->connectToHost(QHostAddress(IP),PORT);

    if(!file_socket->waitForConnected(6000))
    {
        emit signal_connect_fail();
        return;
    }

    //open file
    QFile *myfile = new QFile(filename);
    myfile->open(QIODevice::ReadOnly);

    bool i = true;
    QByteArray filedata;

    while(i)
    {
        filedata.clear();
        filedata = myfile->read(8);

        if(filedata.isEmpty())
        {
            QJsonObject js_obj;
            js_obj.insert("cmd","file");
            js_obj.insert("step","3");
            js_obj.insert("friendname",friendname);

            filetransmit_send_data(js_obj);
            break;
        }


        QJsonObject js_obj;
        js_obj.insert("cmd","file");
        js_obj.insert("step","2");
        js_obj.insert("friendname",friendname);
        js_obj.insert("text",QString(filedata));

        filetransmit_send_data(js_obj);

        QThread::usleep(200000);

    }

    myfile->close();
    delete myfile;


    QThread::usleep(100000);

    file_socket->close();
    delete file_socket;

    emit signal_transmit_finish();

}



void FileTransmit::filetransmit_send_data(QJsonObject &j)
{
    QByteArray data;
    QByteArray ba = QJsonDocument(j).toJson();

    int len = ba.size();

    data.insert(0,(char *)&len,4);
    data.append(ba);

    file_socket->write(data);
    file_socket->flush();

}













