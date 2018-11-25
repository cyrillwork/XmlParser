#include "database.h"

DataBase::DataBase(QObject *parent) : QObject(parent)
{
}


void DataBase::connectToDataBase()
{
    if(!QFile(DATABASE_NAME).exists())
    {
        this->restoreDataBase();
    }
    else
    {
        this->openDataBase();
    }  
}


/* Методы восстановления базы данных
 * */
bool DataBase::restoreDataBase()
{
    if(this->openDataBase())
    {
        if(!this->createTable())
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        qDebug() << "Не удалось восстановить базу данных";
        return false;
    }
    return false;
}

/* Метод для открытия базы данных */
bool DataBase::openDataBase()
{
    /* База данных открывается по заданному пути
     * и имени базы данных, если она существует
     * */
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName(HOST_NAME);
    db.setDatabaseName(DATABASE_NAME);

    if(db.open())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool DataBase::createTable()
{
    QSqlQuery query;
    QString strCreate = "CREATE TABLE ";
    strCreate +=TABLE;
    strCreate += " (name INTEGER, value VARCHAR(255));";

    if(!query.exec(strCreate))
    {
        qDebug() << "Error crate table " << query.lastError();
        return false;
    }

    return true;
}

void DataBase::printTables()
{
    foreach(QString str, db.tables())
    {
        qDebug() << "Tables:" << str;
    }
}

void DataBase::doInsert(QMultiMap<QString, QString> *arrayXml)
{
    int count = 0;

    //doCleanData();
    createTable();

    QSqlQuery query;
    //Insert data in database
    //QString strInsert = "INSERT INTO ";
    //strInsert += TABLE;
    //strInsert += " (name, value) VALUES('%1', '%2');";

    QString strInsert = "INSERT INTO ";
    strInsert += TABLE;
    strInsert += " (name, value) VALUES(:name, :value)";

    query.prepare(strInsert);

    for(QMultiMap<QString, QString>::iterator iii = arrayXml->begin(); iii != arrayXml->end(); iii++)
    {
        //QString strInsert2 = strInsert.arg(iii.key())
        //                              .arg(iii.value());

        query.bindValue(":name", iii.key());
        query.bindValue(":value", iii.value());

        if(!query.exec())
        {
            qDebug() << "Error insert data";
            break;
        }

        if((count % 10) == 0)
        {

            emit changeProgressInsert(count);
        }
        count++;

    }



    emit changeProgressInsert(arrayXml->size());
}

void DataBase::doCleanData()
{
    QSqlQuery query;

    QString strDrop = "DROP TABLE ";
    strDrop += TABLE;
    strDrop += " ;";

    if(!query.exec(strDrop))
    {
        qDebug() << "Error drop table " << query.lastError();
    }

}
