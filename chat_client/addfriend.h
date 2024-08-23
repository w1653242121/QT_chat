#ifndef ADDFRIEND_H
#define ADDFRIEND_H

#include <QWidget>
#include "chatlist.h"

namespace Ui {
class Addfriend;
}

class Addfriend : public QWidget
{
    Q_OBJECT

public:
    explicit Addfriend(QString &,Chatlist *);
    ~Addfriend();

private slots:
    void on_cancelButton_clicked();

    void on_pushButton_clicked();

private:
    Ui::Addfriend *ui;
    QString username;
    Chatlist *parent;
};

#endif // ADDFRIEND_H
