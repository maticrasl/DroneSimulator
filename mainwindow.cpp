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

void MainWindow::on_bLoad_clicked()
{
    QString fileNameObj = ui->inputFileLineEdit->text();
    QString fileNameTex = ui->inputFileLineEditTexture->text();
    QString objectName;
    if(fileNameObj.length() > 3 && fileNameTex.length() > 3) {
        objectName = QString::number(ui->widget->getObjectCount());
        objectName = objectName + ". object";
        ui->objectListComboBox->addItem(objectName);
        ui->widget->addObject(fileNameObj);
        ui->widget->setSelectedObject(static_cast<int>(ui->widget->getObjectCount()) - 1);
        glm::vec3 color = ui->widget->getObjectColor();
        ui->colorRValue->setValue(static_cast<int>(color[0] * 100.0f));
        ui->colorGValue->setValue(static_cast<int>(color[1] * 100.0f));
        ui->colorBValue->setValue(static_cast<int>(color[2] * 100.0f));

        ui->widget->loadTexture(fileNameTex);
    }
}

void MainWindow::on_bMoveAround_clicked()
{
    ui->widget->setFocus();
}

void MainWindow::on_objectListComboBox_highlighted(int index)
{
    ui->widget->setSelectedObject(index + 1);
    glm::vec3 color = ui->widget->getObjectColor();
    ui->colorRValue->setValue(static_cast<int>(color[0] * 100.0f));
    ui->colorGValue->setValue(static_cast<int>(color[1] * 100.0f));
    ui->colorBValue->setValue(static_cast<int>(color[2] * 100.0f));
    ui->shiningValue->setValue(ui->widget->getObjectShining());
}

void MainWindow::updateObjectColor() {
    glm::vec3 color;
    color[0] = static_cast<float>(ui->colorRValue->value()) / 100.0f;
    color[1] = static_cast<float>(ui->colorGValue->value()) / 100.0f;
    color[2] = static_cast<float>(ui->colorBValue->value()) / 100.0f;
    ui->widget->setObjectColor(color);
    ui->widget->setObjectShining(ui->shiningValue->value());
}

void MainWindow::on_colorRValue_valueChanged(int arg1)
{
    updateObjectColor();
}

void MainWindow::on_colorGValue_valueChanged(int arg1)
{
    updateObjectColor();
}

void MainWindow::on_colorBValue_valueChanged(int arg1)
{
    updateObjectColor();
}

void MainWindow::on_lightPowerSlider_sliderMoved(int position)
{
    ui->widget->setLightPower(static_cast<float>(ui->lightPowerSlider->value()));
}

void MainWindow::on_shiningValue_valueChanged(int arg1)
{
    updateObjectColor();
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
