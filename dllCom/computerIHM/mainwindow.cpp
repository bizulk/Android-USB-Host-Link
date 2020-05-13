#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <thread>

#include "device_emulslave.h"
#include "protocomm_master.h"

constexpr auto REQUEST_TOUT_MS = 500;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _protodev = devemulslave_create();
    _proto_this = proto_master_create(_protodev);
    proto_master_open(_proto_this, "");
}

MainWindow::~MainWindow()
{
    delete ui;
    proto_master_destroy(_proto_this);
}

void MainWindow::on_GetRegister_clicked()
{
    // on fait une requête GET
    _numeroRegistre = ui->RegisterNumber->value();
    proto_Status_t status = proto_master_get(_proto_this, _numeroRegistre, &_valeurRegistre);
    analyseStatus(proto_CMD_GET, status);
}

void MainWindow::on_SetRegister_clicked()
{
    _numeroRegistre = ui->RegisterNumber->value();
    _valeurRegistre = ui->ValueToSet->value();
    proto_Status_t status = proto_master_set(_proto_this, _numeroRegistre, _valeurRegistre);
    analyseStatus(proto_CMD_SET, status);
}

void MainWindow::analyseStatus(proto_Command_t command, proto_Status_t status){
    QListWidgetItem* item = new QListWidgetItem();
    switch (status) {
    case proto_NO_ERROR:
        switch (command) {
        case proto_CMD_GET:
            item->setText("Valeur du registre " + QString::number(_numeroRegistre) + " = " + QString::number(_valeurRegistre));
            break;
        case proto_CMD_SET:
            item->setText("La valeur " + QString::number(_valeurRegistre) + " a été attribuée au registre " + QString::number(_numeroRegistre));
            break;
        }
        break;
    case proto_PEER_ERR_CRC:
        item->setText("Erreur: le peer a refusé la trame, crc invalide");
        item->setForeground(Qt::red);
        break;
    case proto_ERR_CRC:
        item->setText("Erreur: Trame reçue invalide");
        item->setForeground(Qt::red);
        break;
    case proto_INVALID_ARG:
        item->setText("Erreur: Ce registre n'existe pas ou sa valeur est invalide");
        item->setForeground(Qt::red);
        break;
    case proto_TIMEOUT:
        item->setText("Erreur: Il s'est écoulé " + QString::number(REQUEST_TOUT_MS) + "ms sans recevoir de trame complète");
        item->setForeground(Qt::red);
        break;

    }
    ui->Result->addItem(item);
    ui->Result->setCurrentRow(ui->Result->count() - 1);
}
