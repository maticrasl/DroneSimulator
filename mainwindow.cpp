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
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::on_actionTest_triggered(){
	QMessageBox::information(this,"Test title","Test test");
}

void MainWindow::on_pushButtonRotacija_clicked(){
    ui->widget->MoveObjXPlus();
}

void MainWindow::on_pushButton_5_pressed()
{
    ui->widget->MoveObjXMinus();
}

void MainWindow::on_pushButton_clicked()
{
    ui->widget->MoveObjYPlus();
}

void MainWindow::on_pushButton_4_clicked()
{
    ui->widget->MoveObjYMinus();
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->widget->MoveObjZPlus();
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->widget->MoveObjZMinus();
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
}

void MainWindow::on_bConnectCOMPort_clicked()
{
    ui->widget->openSerialPort(ui->COMPortComboBox->itemText(ui->COMPortComboBox->currentIndex()));
}

void MainWindow::on_bStartRec_clicked()
{
    ui->widget->startRecording();
}

void MainWindow::on_bStopRec_clicked()
{
    ui->widget->stopRecording();
}

void MainWindow::on_bStartReplay_clicked()
{
    ui->widget->startReplay();
}

void MainWindow::on_bStopReplay_clicked()
{
    ui->widget->stopReplay();
}
