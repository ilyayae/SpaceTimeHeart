#ifndef ADVANCEDFILESYSTEMMODEL_H
#define ADVANCEDFILESYSTEMMODEL_H

#include <QFileSystemModel>

class AdvancedFileSystemModel : public QFileSystemModel {
    Q_OBJECT
public:
    explicit AdvancedFileSystemModel(QObject *parent = nullptr) : QFileSystemModel(parent) {}
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

signals:
    void aboutToMoveOrRename(const QString &oldPath);
    void finishedMovingOrRenaming(const QString &newPath);
};

#endif // ADVANCEDFILESYSTEMMODEL_H
