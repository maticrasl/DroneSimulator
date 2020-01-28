#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QtSerialPort/QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
    for(int i = 0; i < list.size(); i++) {
        ui->COMPortComboBox->addItem(list[i].portName());
    }
    ui->bStartRec->setStyleSheet("color: red;");
    ui->bStop->setStyleSheet("color: red;");
    ui->bStartReplay->setStyleSheet("color: green;");
    ui->bStartRec->setEnabled(false);
    ui->bStop->setEnabled(false);
    ui->bStartReplay->setEnabled(false);
    ui->widget->setFocus();
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::on_actionTest_triggered(){
	QMessageBox::information(this,"Test title","Test test");
}

void MainWindow::on_bMoveAround_clicked()
{
    ui->widget->setFocus();
}

void MainWindow::on_bRefreshCOMPort_clicked()
{
    ui->COMPortComboBox->clear();
    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
    for(int i = 0; i < list.size(); i++) {
        ui->COMPortComboBox->addItem(list[i].portName());
    }
    ui->widget->setFocus();
}

void MainWindow::on_bConnectCOMPort_clicked()
{
    if(ui->COMPortComboBox->count() == 0)
        return;
    ui->bStartRec->setEnabled(true);
    ui->bStartReplay->setEnabled(false);
    ui->widget->setFocus();
    ui->widget->openSerialPort(ui->COMPortComboBox->itemText(ui->COMPortComboBox->currentIndex()));
}

void MainWindow::on_bStartRec_clicked()
{
    ui->bStop->setEnabled(true);
    ui->bStartReplay->setEnabled(false);
    ui->bStartRec->setEnabled(false);
    ui->widget->setFocus();
    ui->widget->startRecording();
}

void MainWindow::on_bStop_clicked()
{
    ui->bStop->setEnabled(false);
    ui->bStartRec->setEnabled(true);
    ui->bStartReplay->setEnabled(true);
    ui->widget->setFocus();
    ui->widget->stop();
}

void MainWindow::on_bStartReplay_clicked()
{
    ui->bStop->setEnabled(true);
    ui->widget->setFocus();
    ui->widget->startReplay();
}
