#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <thread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    proto_initData_EmulSlave(&devicedata);
    memset(&_etat, 0, sizeof(_etat));
    _valeurRegistre = 0;
    // Je sias pas ce que c'est ce crochet ? un lambda ? on ne pouvait caster dans le type définit dans l'API ?
    proto_setReceiver(&_etat, [] (void* instance, proto_Command cmd, const uint8_t * _args) {
            static_cast<MainWindow*>(instance)->onProtoFrameRcvd(cmd, args);
        }, this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_GetRegister_clicked()
{
    // on fait une requête GET
    _numeroRegistre = ui->RegisterNumber->value();
    _args[0] = _numeroRegistre;
    proto_writeFrame(proto_GET, _args, device, &devicedata);

    int nbRead = 0;
    auto start = std::chrono::steady_clock::now();
    // Nombre 'magique à éviter' #define REQUEST_TOUT_MS 500
    while (std::chrono::steady_clock::now() - start < std::chrono::milliseconds{500}) {
        nbRead = proto_readBlob(&_etat, device, &devicedata);
        if (nbRead > 0) break;
        std::this_thread::yield(); // on dit à l'OS qu'on peut attendre un peu
    }
    if (nbRead > 0)
        if(_errorLog == ""){
            // SLI refactoriser en une fonction de log
            ui->Result->addItem(QString("Valeur du registre ").append(QString::number(_numeroRegistre)).append(" = ").append(QString::number(_valeurRegistre)));
        }else{
            QListWidgetItem* item = new QListWidgetItem(QString("Erreur reçue : ").append(_errorLog));
            item->setForeground(Qt::red);
            ui->Result->addItem(item);
        }
    else
        ui->Result->addItem("Nous n'avons pas reçu de trame complète en 500 millisecondes ...");
}

void MainWindow::on_SetRegister_clicked()
{
    _numeroRegistre = ui->RegisterNumber->value();
    _args[0] = _numeroRegistre;
    _args[1] = ui->ValueToSet->value();
    proto_writeFrame(proto_SET, _args, device, &devicedata);

    int nbRead = 0;
    auto start = std::chrono::steady_clock::now();
    // SLI  Alors j'aime bien ce code mais aujourd'hui ce n'est pas du tout ce qu'il faut faire, writeFrame doit faire tout ce travail
    while (std::chrono::steady_clock::now() - start < std::chrono::milliseconds{500}) {
        nbRead = proto_readBlob(&_etat, device, &devicedata);
        if (nbRead > 0) break;
        std::this_thread::yield(); // on dit à l'OS qu'on peut attendre un peu
    }
    if(_errorLog == "")
        ui->Result->addItem(QString("La valeur ").append(QString::number(_args[1])).append(" a été attribuée au registre ").append(QString::number(_args[0])));
    else{
        QListWidgetItem* item = new QListWidgetItem(QString("Erreur reçue : ").append(_errorLog));
        item->setForeground(Qt::red);
        ui->Result->addItem(item);
    }
}

void MainWindow::onProtoFrameRcvd(proto_Command command, const uint8_t * args){
    switch (command) {
    case proto_REPLY:
        _valeurRegistre = args[0];
        _errorLog = "";
        break;
    case proto_NOTIF_BAD_CRC:
        ui->Result->addItem(QString("Mauvais CRC... reçu=").append(QString::number(args[0])).append(", calculé=").append(QString::number(args[1])));
        break;
    // SLI On peut se créer une Array de message en fonction de status, plus souple.
    // Retourner proto status plutôt,
    case proto_STATUS:
        switch (args[0]) {
        case proto_NO_ERROR:
            _errorLog = "";
            break;
        case proto_INVALID_CRC:
            // SLI est-ce que l'on est pas en train d'afficher ça deux fois ?
            _errorLog = "Il y a eu un problème de CRC.";
            break;
        case proto_INVALID_REGISTER:
            _errorLog = "Ce registre n'existe pas.";
            break;
        case proto_INVALID_VALUE:
            _errorLog = "Il est impossible de mettre cette valeur dans le registre.";
            break;
        }
        break;
    }
}
