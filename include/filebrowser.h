#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QWidget>
#include <QFileSystemModel>
#include <QTreeView>
#include <QInputDialog>
#include <QDebug>

class filebrowser : public QWidget
{
    Q_OBJECT
public:
    explicit filebrowser(const QString &rootPath, QTreeView *treeView, QWidget *parent = nullptr);
    QString currentFilePath() const;

public slots:
    void createNote();
    void createFolder();
    void deleteSelected();

private:
    QFileSystemModel *model;
    QTreeView *view;
    QString root;

signals:
    void fileSelected(const QString &path);
};

#endif // FILEBROWSER_H
