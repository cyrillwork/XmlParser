#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QtSql>

#define DATABASE_NAME "my_sqllite"
#define USER_NAME "user"
#define HOST_NAME "127.0.0.1"
#define PASSWORD "1"

#define TABLE "xmldata"


/* Вспомогтаельный класс для работы с БД */
class DataBase : public QObject
{
    Q_OBJECT
    bool restoreDataBase();
    bool openDataBase();

    QSqlDatabase db;

public:
    explicit DataBase(QObject *parent = 0);
    void connectToDataBase();
    void printTables();

    bool createTable();

signals:
    void changeProgressInsert(int value);

public slots:
    void doInsert(QMultiMap<QString, QString> *arrayXml);
    void doCleanData();

};

#endif // DATABASE_H
