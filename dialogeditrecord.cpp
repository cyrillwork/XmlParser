#include "dialogeditrecord.h"
#include "ui_dialogeditrecord.h"

DialogEditRecord::DialogEditRecord(int row, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogEditRecord)
{
    ui->setupUi(this);

    setupModel();

    if(row != -1)
    {
        mapper->setCurrentModelIndex(model->index(row,0));
    }

}

DialogEditRecord::~DialogEditRecord()
{
    delete ui;
}

void DialogEditRecord::setupModel()
{
    // Инициализируем модель и делаем выборку
    model = new QSqlTableModel(this);
    model->setTable(TABLE);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    // Инициализируем mapper и привязываем поля данных к объектам LineEdit
    mapper = new QDataWidgetMapper();
    mapper->setModel(model);

    mapper->addMapping(ui->lineEditName,  0);
    mapper->addMapping(ui->lineEditValue, 1);

    //mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    //connect(ui->previousButton, SIGNAL(clicked()), mapper, SLOT(toPrevious()));
    //connect(ui->nextButton, SIGNAL(clicked()), mapper, SLOT(toNext()));

    connect(mapper, SIGNAL(currentIndexChanged(int)), this, SLOT(updateButtons(int)));
}

void DialogEditRecord::on_pushButton_clicked()
{
    mapper->submit();
    model->submitAll();
    emit readyToUpdate();
    this->close();
}
