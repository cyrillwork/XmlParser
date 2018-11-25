#ifndef DIALOGEDITRECORD_H
#define DIALOGEDITRECORD_H

#include "database.h"

#include <QDialog>
#include <QSqlTableModel>
#include <QDataWidgetMapper>
#include <QSqlTableModel>
#include <QDataWidgetMapper>



namespace Ui {
class DialogEditRecord;
}

class DialogEditRecord : public QDialog
{
    Q_OBJECT

public:
    explicit DialogEditRecord(int row, QWidget *parent = 0);
    ~DialogEditRecord();

signals:
    void readyToUpdate();

private slots:
    void on_pushButton_clicked();

private:
    void setupModel();

    Ui::DialogEditRecord *ui;
    QSqlTableModel              *model;
    QDataWidgetMapper           *mapper;
};

#endif // DIALOGEDITRECORD_H
