#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <../libcomm/protocomm-emulslave.h>
#include <../libcomm/protocomm.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    proto_Data_EmulSlave devicedata;
    proto_Device device = proto_getDevice_EmulSlave();

    proto_State etat;

    uint8_t args[proto_MAX_ARGS];
    uint8_t numeroRegistre;
    uint8_t valeurRegistre;

    QString errorLog;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // callBack sera appelé quand on a reçu une trame
    void callBack(proto_Command, uint8_t const*);

private:
    Ui::MainWindow *ui;

private slots:
    void on_GetRegister_clicked();
    void on_SetRegister_clicked();
};

#endif // MAINWINDOW_H
