#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	private slots:
		void on_actionTest_triggered();

        void on_pushButtonRotacija_clicked();

        void on_pushButton_2_clicked();

        void on_pushButton_clicked();

        void on_pushButton_3_clicked();

        void on_pushButton_5_pressed();

        void on_pushButton_4_clicked();

        void on_bLoad_clicked();

        void on_bMoveAround_clicked();

        void on_objectListComboBox_highlighted(int index);

        void on_colorRValue_valueChanged(int arg1);

        void updateObjectColor();

        void on_colorGValue_valueChanged(int arg1);

        void on_colorBValue_valueChanged(int arg1);

        void on_lightPowerSlider_sliderMoved(int position);

        void on_shiningValue_valueChanged(int arg1);

        void on_bRefreshCOMPort_clicked();

        void on_bConnectCOMPort_clicked();

        void on_bStartRec_clicked();

private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
