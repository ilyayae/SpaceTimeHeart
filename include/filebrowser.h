#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QWidget>
#include <QFileSystemModel>
#include <QTreeView>
#include <QInputDialog>
#include <QDebug>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QMessageBox>

#include "include/calendarconfig.h"
#include "include/noteTypes/calendardata.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class filebrowser;
}
QT_END_NAMESPACE

class filebrowser : public QMainWindow
{
    Q_OBJECT
public:
    explicit filebrowser(const QString &rootPath, QWidget *parent = nullptr);
    ~filebrowser();
    QString currentFilePath() const;
    QTreeView *view;

public slots:
    void createFolder();
    void deleteSelected();
    void createCalendar();

private:
    Ui::filebrowser *ui;
    QFileSystemModel *model;
    QString root;
    CalendarConfig *calconfig;

signals:
    void fileSelected(const QString &path);
private slots:
    void on_actionNewPlainNote_triggered();
    void on_actionNewMarkdownNote_triggered();
    void on_actionNewCallendar_triggered();
    void on_actionNewImageAnnotation_triggered();
    void on_actionDeleteFolder_triggered();
    void on_actionDeleteFile_triggered();
    void on_actionNewFolder_triggered();
};

#endif // FILEBROWSER_H
