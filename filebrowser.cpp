#include "filebrowser.h"

filebrowser::filebrowser(const QString &rootPath, QTreeView *treeView, QWidget *parent)
    : QWidget{parent}
{
    root = rootPath;

    model = new QFileSystemModel(this);
    model->setRootPath(root);
    model->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    model->setNameFilters({"*.txt", "*.md", "*.html"});
    model->setNameFilterDisables(false);

    view = treeView;
    view->setModel(model);
    view->setRootIndex(model->index(root));
    view->setHeaderHidden(true);
    view->setColumnHidden(1, true);
    view->setColumnHidden(2, true);
    view->setColumnHidden(3, true);
    view->setIndentation(10);

    connect(view, &QTreeView::clicked, this, [this](const QModelIndex &index) {
        if (!model->isDir(index))
        {
            QString filepath = model->filePath(index);
            emit fileSelected(filepath);
        }
        else
        {
            if (view->isExpanded(index))
                view->collapse(index);
            else
                view->expand(index);
        }

    });
}

QString filebrowser::currentFilePath() const
{
    QModelIndex index = view->currentIndex();
    return model->filePath(index);
}

void filebrowser::createNote()
{
    bool ok;
    QString name = QInputDialog::getText(this, "New Note", "Note name:", QLineEdit::Normal, "untitled.txt", &ok);
    if (!ok || name.isEmpty()) return;

    QFile file(root + "/" + name);
    if (!file.exists()) file.open(QIODevice::WriteOnly);
}

void filebrowser::createFolder()
{
    bool ok;
    QString name = QInputDialog::getText(this, "New Folder", "Folder name:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    QDir dir(root);
    dir.mkdir(name);
}

void filebrowser::deleteSelected()
{
    QString path = currentFilePath();
    if (path.isEmpty()) return;

    QFileInfo info(path);
    if (info.isDir())
        QDir(path).removeRecursively();
    else
        QFile::remove(path);
}
