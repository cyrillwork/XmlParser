#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QString>
#include <QProgressDialog>

#include "database.h"
#include "parserxmlworker.h"
#include "dialogeditrecord.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_loadButton_clicked();
    void on_clearButton_clicked();
    void on_closeButton_clicked();
    void setProgressInsert(int value);
    void setProgressParseXML(int value, QString fileName, bool result);
    void customMenuRequested(QPoint pos);
    void removeRecord();
    void editRecord();
    void updateModel();

signals:
    void startParseXML(QFileInfoList *fileList, QMultiMap <QString, QString> *array);
    void startInsert(QMultiMap <QString, QString> *array);
    void cleanData();

private:
    DataBase *workerBD;
    QSqlTableModel *tableModel;

    ParserXMLWorker *workerXML;

    Ui::MainWindow *ui;
    QMultiMap <QString, QString> arrayXml;

    QThread workerXMLThread;
    QThread workerBDThread;

    QFileInfoList *fileList;
    QProgressDialog *progressDialog;
};

#endif // MAINWINDOW_H
