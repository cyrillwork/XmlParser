#include "parserxmlworker.h"

#include <QDebug>
#include <QThread>

ParserXMLWorker::ParserXMLWorker()
{
}

void ParserXMLWorker::doParser(QFileInfoList *fileList, QMultiMap<QString, QString> *arrayXml)
{

    for(int iii = 0; iii<fileList->size(); iii++)
    {
        bool result = true;

        QString fileName(fileList->at(iii).absoluteFilePath());
        qDebug() << "Try work with file" << fileName;

        QFile file(fileName);

        if(file.open(QIODevice::ReadOnly))
        {
            QXmlStreamReader sr(&file);
            do
            {
                sr.readNext();
                if(sr.attributes().size() > 0)
                {
                    for(auto jjj : sr.attributes())
                    {
                        arrayXml->insert(jjj.name().toString(), jjj.value().toString());
                    }
                }

            } while(!sr.atEnd());

            if(sr.hasError())
            {
                qDebug() << "Error:" << sr.errorString();
                result = false;
            }
            file.close();
        }
        else
        {
            qDebug() << "Error open file" << fileName;
            result = false;
        }

        emit changeProgressParseXML(iii, fileList->at(iii).fileName(), result);

        QThread::sleep(1);
    }

    emit changeProgressParseXML(fileList->size(), QString(""), true);

}
