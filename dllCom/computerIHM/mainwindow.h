#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <protocomm_master.h>
#include <stdio.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    /// Le device pour notre protocole
    proto_Device_t _protodev;
    proto_hdle_t * _proto_this;

    uint8_t _numeroRegistre = 0;
    uint8_t _valeurRegistre = 0;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Fonction permettant d'analyser le statut et de remplir le log en fonction
    void analyseStatus(proto_Command_t, proto_Status_t);

private:
    Ui::MainWindow *ui;

private slots:
    void on_GetRegister_clicked();
    void on_SetRegister_clicked();
};

#endif // MAINWINDOW_H
