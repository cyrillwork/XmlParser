#ifndef PARSERXMLWORKER_H
#define PARSERXMLWORKER_H

#include <QObject>
#include <QFile>
#include <QFileInfoList>
#include <QXmlStreamReader>

class ParserXMLWorker: public QObject
{
    Q_OBJECT

public:
    ParserXMLWorker();

public slots:
    void doParser(QFileInfoList *fileList, QMultiMap<QString, QString> *arrayXml);

signals:
    void changeProgressParseXML(int value, QString fileName, bool result);
};

#endif // PARSERXMLWORKER_H
