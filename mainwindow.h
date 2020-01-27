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

        void on_bMoveAround_clicked();

        void on_bRefreshCOMPort_clicked();

        void on_bConnectCOMPort_clicked();

        void on_bStartRec_clicked();

        void on_bStop_clicked();

        void on_bStartReplay_clicked();

private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
