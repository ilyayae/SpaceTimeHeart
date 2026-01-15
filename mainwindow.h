#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QTimer>
#include <QSettings>
#include "documenthandler.h"
#include "filebrowser.h"
#include "configmenu.h"

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
    Ui::MainWindow *ui;

private slots:
    void on_actionNew_triggered();

    void on_actionNew_File_triggered();

    void on_actionOpen_File_triggered();

    void on_actionSave_File_triggered();

    void on_actionPreferences_triggered();

    void openFromBrowser(const QString &path);

    void initEverything();

    void StartSaveProcess();

private:
    DocumentHandler *dochandl;
    filebrowser *browser;
    QTimer *timer;
    QSettings *settings;
    ConfigMenu *configMenu;
};
#endif // MAINWINDOW_H
