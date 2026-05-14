#include "include/subclasses/advancedfilesystemmodel.h"
#include "qmimedata.h"
#include "qurl.h"

bool AdvancedFileSystemModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role == Qt::EditRole) {
        QString oldPath = filePath(index);
        QString newName = value.toString();

        emit aboutToMoveOrRename(oldPath);

        bool result = QFileSystemModel::setData(index, value, role);

        if (result) {
            // Note: The index might change, so we construct the new path manually
            QDir parentDir = fileInfo(index).dir();
            QString newPath = parentDir.absoluteFilePath(newName);
            emit finishedMovingOrRenaming(newPath);
        }
        return result;
    }
    return QFileSystemModel::setData(index, value, role);
}

bool AdvancedFileSystemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
    if (action != Qt::MoveAction || !data->hasUrls()) {
        return QFileSystemModel::dropMimeData(data, action, row, column, parent);
    }
    QString destDirPath;
    if (parent.isValid()) {
        QFileInfo destInfo = fileInfo(parent);
        if (destInfo.isDir()) {
            destDirPath = destInfo.absoluteFilePath();
        } else {
            destDirPath = destInfo.absolutePath();
        }
    } else {
        destDirPath = rootPath();
    }
    QList<QString> sources;
    foreach (const QUrl &url, data->urls()) {
        QString path = url.toLocalFile();
        sources.append(path);
        emit aboutToMoveOrRename(path);
    }
    bool success = QFileSystemModel::dropMimeData(data, action, row, column, parent);
    if (success) {
        for (const QString &oldPath : sources) {
            QFileInfo oldInfo(oldPath);
            QString newPath = QDir(destDirPath).absoluteFilePath(oldInfo.fileName());
            emit finishedMovingOrRenaming(newPath);
        }
    }

    return success;
}