#ifndef GROUPCREATE_H
#define GROUPCREATE_H

#include <QWidget>
#include "chatlist.h"
#include <QJsonObject>
#include <QJsonDocument>

namespace Ui {
class GroupCreate;
}

class GroupCreate : public QWidget
{
    Q_OBJECT

public:
    explicit GroupCreate(QString ,Chatlist *);
    ~GroupCreate();

private slots:
    void on_cancelButton_clicked();

    void on_pushButton_clicked();

private:
    Ui::GroupCreate *ui;

    QString username;
    Chatlist *c;
};

#endif // GROUPCREATE_H
