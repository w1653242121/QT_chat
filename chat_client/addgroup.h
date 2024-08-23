#ifndef ADDGROUP_H
#define ADDGROUP_H

#include <QWidget>
#include "chatlist.h"
#include <QJsonObject>
#include <QJsonDocument>


namespace Ui {
class addgroup;
}

class addgroup : public QWidget
{
    Q_OBJECT

public:
    explicit addgroup(QString ,Chatlist *);
    ~addgroup();

private slots:
    void on_cancelButton_clicked();

    void on_pushButton_clicked();

private:
    Ui::addgroup *ui;

    QString username;
    Chatlist *c;
};

#endif // ADDGROUP_H
