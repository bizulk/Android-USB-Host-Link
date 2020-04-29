#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <../libcomm/protocomm-emulslave.h>
#include <../libcomm/protocomm.h>
#include <stdio.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    // Mettre un minimum de commentaire svp !
    proto_Data_EmulSlave devicedata;
    proto_Device device = proto_getDevice_EmulSlave();

    proto_State _etat;
    // y'a vraiment besoin que ce soit un membre ?
    uint8_t _args[proto_MAX_ARGS];
    uint8_t _numeroRegistre;
    uint8_t _valeurRegistre;
    // Pas bien d'accord avec cette façon d'écrire le log, voir les commentaires dans l'implémentation;
    QString _errorLog;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // callBack sera appelé quand on a reçu une trame
    void onProtoFrameRcvd(proto_Command, uint8_t const*);

private:
    Ui::MainWindow *ui;

private slots:
    void on_GetRegister_clicked();
    void on_SetRegister_clicked();
};

#endif // MAINWINDOW_H
