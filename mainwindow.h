#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QTimer>

#include "documenthandler.h"
#include "filebrowser.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionNew_triggered();

    void on_actionNew_File_triggered();

    void on_actionOpen_File_triggered();

    void on_actionSave_File_triggered();

    void openFromBrowser(const QString &path);

    void on_textEdit_textChanged();

private:
    Ui::MainWindow *ui;
    DocumentHandler *dochandl;
    filebrowser *browser;
    QTimer *timer;
};
#endif // MAINWINDOW_H
