#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <thread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    device = proto_getDevice_EmulSlave();
    proto_initData_EmulSlave(&devicedata);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_GetRegister_clicked()
{
    // on fait une requête GET
    _numeroRegistre = ui->RegisterNumber->value();
    proto_Status_t status = proto_cio_master_get(_numeroRegistre, &_valeurRegistre, 0, device, &devicedata);
    analyseStatus(proto_GET, status);
}

void MainWindow::on_SetRegister_clicked()
{
    _numeroRegistre = ui->RegisterNumber->value();
    _valeurRegistre = ui->ValueToSet->value();
    proto_Status_t status = proto_cio_master_set(_numeroRegistre, _valeurRegistre, 0, device, &devicedata);
    analyseStatus(proto_SET, status);
}

void MainWindow::analyseStatus(proto_Command_t command, proto_Status_t status){
    QListWidgetItem* item = new QListWidgetItem();
    switch (status) {
    case proto_NO_ERROR:
        switch (command) {
        case proto_GET:
            item->setText("Valeur du registre " + QString::number(_numeroRegistre) + " = " + QString::number(_valeurRegistre));
            break;
        case proto_SET:
            item->setText("La valeur " + QString::number(_valeurRegistre) + " a été attribuée au registre " + QString::number(_numeroRegistre));
            break;
        }
        break;
    case proto_INVALID_CRC:
        // SLI est-ce que l'on est pas en train d'afficher ça deux fois ?
        item->setText("Il y a eu un problème de CRC.");
        item->setForeground(Qt::red);
        break;
    case proto_INVALID_REGISTER:
        item->setText("Ce registre n'existe pas.");
        item->setForeground(Qt::red);
        break;
    case proto_INVALID_VALUE:
        item->setText("Il est impossible de mettre cette valeur dans le registre.");
        item->setForeground(Qt::red);
        break;
    }
    ui->Result->addItem(item);
    ui->Result->setCurrentRow(ui->Result->count() - 1);
}
