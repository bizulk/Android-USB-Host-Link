#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <thread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    proto_initData_EmulSlave(&devicedata);
    memset(&etat, 0, sizeof(etat));
    valeurRegistre = 0;
    proto_setReceiver(&etat, [] (void* instance, proto_Command cmd, const uint8_t * args) {
            static_cast<MainWindow*>(instance)->callBack(cmd, args);
        }, this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_GetRegister_clicked()
{
    // on fait une requête GET
    numeroRegistre = ui->RegisterNumber->value();
    args[0] = numeroRegistre;
    proto_writeFrame(proto_GET, args, device, &devicedata);

    int nbRead = 0;
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start < std::chrono::milliseconds{500}) {
        nbRead = proto_readBlob(&etat, device, &devicedata);
        if (nbRead > 0) break;
        std::this_thread::yield(); // on dit à l'OS qu'on peut attendre un peu
    }
    if (nbRead > 0)
        if(errorLog == ""){
            ui->Result->addItem(QString("Valeur du registre ").append(QString::number(numeroRegistre)).append(" = ").append(QString::number(valeurRegistre)));
        }else{
            QListWidgetItem* item = new QListWidgetItem(QString("Erreur reçue : ").append(errorLog));
            item->setForeground(Qt::red);
            ui->Result->addItem(item);
        }
    else
        ui->Result->addItem("Nous n'avons pas reçu de trame complète en 500 millisecondes ...");
}

void MainWindow::on_SetRegister_clicked()
{
    numeroRegistre = ui->RegisterNumber->value();
    args[0] = numeroRegistre;
    args[1] = ui->ValueToSet->value();
    proto_writeFrame(proto_SET, args, device, &devicedata);

    int nbRead = 0;
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start < std::chrono::milliseconds{500}) {
        nbRead = proto_readBlob(&etat, device, &devicedata);
        if (nbRead > 0) break;
        std::this_thread::yield(); // on dit à l'OS qu'on peut attendre un peu
    }
    if(errorLog == "")
        ui->Result->addItem(QString("La valeur ").append(QString::number(args[1])).append(" a été attribuée au registre ").append(QString::number(args[0])));
    else{
        QListWidgetItem* item = new QListWidgetItem(QString("Erreur reçue : ").append(errorLog));
        item->setForeground(Qt::red);
        ui->Result->addItem(item);
    }
}

void MainWindow::callBack(proto_Command command, const uint8_t * args){
    switch (command) {
    case proto_REPLY:
        valeurRegistre = args[0];
        errorLog = "";
        break;
    case proto_NOTIF_BAD_CRC:
        ui->Result->addItem(QString("Mauvais CRC... reçu=").append(QString::number(args[0])).append(", calculé=").append(QString::number(args[1])));
        break;
    case proto_STATUS:
        switch (args[0]) {
        case proto_NO_ERROR:
            errorLog = "";
            break;
        case proto_INVALID_CRC:
            errorLog = "Il y a eu un problème de CRC.";
            break;
        case proto_INVALID_REGISTER:
            errorLog = "Ce registre n'existe pas.";
            break;
        case proto_INVALID_VALUE:
            errorLog = "Il est impossible de mettre cette valeur dans le registre.";
            break;
        }
        break;
    }
}
