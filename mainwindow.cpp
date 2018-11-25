#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtXml>
#include <QFile>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    workerBD = new DataBase();
    workerBD->connectToDataBase();
    workerBD->printTables();
    workerBD->moveToThread(&workerBDThread);

    connect(this, &MainWindow::startInsert, workerBD, &DataBase::doInsert);
    connect(workerBD, &DataBase::changeProgressInsert, this, &MainWindow::setProgressInsert);
    connect(this, &MainWindow::cleanData, workerBD, &DataBase::doCleanData);

    tableModel = new QSqlTableModel(this);
    tableModel->setTable(TABLE);

    QStringList headers;

    headers << trUtf8("Имя")
            << trUtf8("Значение");

    for(int i = 0, j = 0; i < tableModel->columnCount(); i++, j++)
    {
        tableModel->setHeaderData(i,Qt::Horizontal,headers[j]);
    }
    tableModel->setEditStrategy(QSqlTableModel::OnRowChange);
    // Устанавливаем сортировку по возрастанию данных по нулевой колонке
    tableModel->setSort(0,Qt::AscendingOrder);
    // Делаем выборку данных из таблицы
    tableModel->select();

    // Устанавливаем модель на TableView
    ui->tableView->setModel(tableModel);
    // Разрешаем выделение строк
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем режим выделения лишь одно строки в таблице
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    // Устанавливаем размер колонок по содержимому
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    // Устанавливаем Контекстное Меню
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    //connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotEditRecord()));
    // Подключаем СЛОТ вызова контекстного меню
    connect(ui->tableView, &QTableView::customContextMenuRequested, this, &MainWindow::customMenuRequested);

    workerXML = new ParserXMLWorker();
    workerXML->moveToThread(&workerXMLThread);

    //connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &MainWindow::startParseXML, workerXML, &ParserXMLWorker::doParser);
    connect(workerXML, &ParserXMLWorker::changeProgressParseXML, this, &MainWindow::setProgressParseXML);

    workerXMLThread.start();
    workerBDThread.start();

    fileList = NULL;
}


void MainWindow::customMenuRequested(QPoint pos)
{
    /* Создаем объект контекстного меню */
    QMenu * menu = new QMenu(this);
    /* Создаём действия для контекстного меню */
    QAction * editDevice = new QAction(trUtf8("Редактировать"), this);
    QAction * deleteDevice = new QAction(trUtf8("Удалить"), this);
    /* Подключаем СЛОТы обработчики для действий контекстного меню */
    connect(editDevice, &QAction::triggered, this, &MainWindow::editRecord);     // Обработчик вызова диалога редактирования
    connect(deleteDevice, &QAction::triggered, this, &MainWindow::removeRecord); // Обработчик удаления записи

    /* Устанавливаем действия в меню */
    menu->addAction(editDevice);
    menu->addAction(deleteDevice);
    /* Вызываем контекстное меню */
    menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}

void MainWindow::removeRecord()
{
    // Выясняем, какая из строк была выбрана
    int row = ui->tableView->selectionModel()->currentIndex().row();

    // Проверяем, что строка была действительно выбрана
    if(row >= 0)
    {
        tableModel->removeRow(row);
        tableModel->select();
        ui->tableView->setCurrentIndex(tableModel->index(-1, -1));
    }
}

void MainWindow::editRecord()
{
    DialogEditRecord *editDialog = new DialogEditRecord(ui->tableView->selectionModel()->currentIndex().row());
    connect(editDialog, &DialogEditRecord::readyToUpdate, this, &MainWindow::updateModel);

    editDialog->setWindowTitle(trUtf8("Редактировать запись"));
    editDialog->exec();
}

void MainWindow::updateModel()
{
    tableModel->select();
    ui->tableView->resizeColumnsToContents();
}


MainWindow::~MainWindow()
{
    workerXMLThread.quit();
    workerXMLThread.wait();

    workerBDThread.quit();
    workerBDThread.wait();

    delete workerXML;
    delete ui;
}

void MainWindow::on_loadButton_clicked()
{
    // QString fileName = QFileDialog::getOpenFileName(this, tr("Open xml file"), "./", tr("XML files (*.xml)"));
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Выбирите директорию с xml файлами"),
                                                      "./",
                                                      QFileDialog::ShowDirsOnly
                                                      | QFileDialog::DontResolveSymlinks);
    if(dirName == "")
    {
        return;
    }


    QDir dir(dirName);

    dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    dir.setNameFilters(QStringList("*.xml"));

    fileList = new QFileInfoList (dir.entryInfoList());

    if(fileList->size() == 0)
    {
        QMessageBox::warning(this, "Сообщение", "В выбранной директории нет xml файлов");
        return;
    }

    progressDialog = new QProgressDialog("Parse file ...", QString(), 0, fileList->size(), this);
    progressDialog->setMinimumDuration(0);
    progressDialog->setWindowTitle("Пожалуйста подождите");
    progressDialog->setAutoClose(true);

    emit startParseXML(fileList, &arrayXml);
}

void MainWindow::on_clearButton_clicked()
{
    qDebug () << "clear";
    tableModel->clear();

    qDebug () << "reset";
    ui->tableView->reset();

    emit cleanData();
}

void MainWindow::on_closeButton_clicked()
{
    this->close();
}


void MainWindow::setProgressInsert(int value)
{
    QString mess = "Загрузка данных в БД";
    progressDialog->setValue(value);
    progressDialog->setLabelText(mess);

    if(value == arrayXml.size())
    {
        qDebug() << "!!! updateTableView";
        tableModel->setTable(TABLE);
        tableModel->select();
	
	// Устанавливаем размер колонок по содержимому
	ui->tableView->resizeColumnsToContents();

    }

    QApplication::instance()->processEvents();
}

void MainWindow::setProgressParseXML(int value, QString fileName, bool result)
{
    if(value == fileList->size())
    {
        QString mess = "Загрузка данных в БД";

        progressDialog->setLabelText(mess);
        progressDialog->setValue(0);
        progressDialog->setRange(0, arrayXml.size());

        for(QMultiMap<QString, QString>::iterator iii = arrayXml.begin(); iii != arrayXml.end(); iii++)
        {
            qDebug() << "key" << iii.key() << " value" << iii.value();
        }

        emit startInsert(&arrayXml);

        delete fileList;
    }
    else
    {
        QString mess = "Разбор файла " + fileName;
        mess += (result)?" успешено":" ошибка";

        progressDialog->setValue(value);
        progressDialog->setLabelText(mess);
    }

    QApplication::instance()->processEvents();
}
