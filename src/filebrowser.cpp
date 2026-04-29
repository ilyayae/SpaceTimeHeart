#include "include\filebrowser.h"
#include "ui_filebrowser.h"

#include <QFile>
filebrowser::filebrowser(const QString &rootPath, QWidget *parent)
    : QMainWindow{parent}
    , ui(new Ui::filebrowser)
{
    ui->setupUi(this);
    root = rootPath;

    model = new QFileSystemModel(this);
    model->setRootPath(root);
    model->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    model->setNameFilters({"*.txt", "*.md", "*.ccal", "*.iman"});
    model->setNameFilterDisables(false);

    view = new QTreeView(this);
    view->setModel(model);
    view->setRootIndex(model->index(root));
    view->setHeaderHidden(true);
    view->setColumnHidden(1, true);
    view->setColumnHidden(2, true);
    view->setColumnHidden(3, true);
    view->setIndentation(10);

    ui->centralwidget->layout()->addWidget(view);

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

filebrowser::~filebrowser()
{
    delete ui;
}

QString filebrowser::currentFilePath() const
{
    QModelIndex index = view->currentIndex();
    return model->filePath(index);
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

void filebrowser::on_actionNewPlainNote_triggered()
{

    bool ok;
    QString name = QInputDialog::getText(this, "New Note", "Note name:", QLineEdit::Normal, "untitled", &ok);
    if (!ok || name.isEmpty()) return;
    name = name + ".txt";

    QFile file(root + "/" + name);
    if (!file.exists()) file.open(QIODevice::WriteOnly);
}


void filebrowser::on_actionNewMarkdownNote_triggered()
{
    bool ok;
    QString name = QInputDialog::getText(this, "New Note", "Note name:", QLineEdit::Normal, "untitled", &ok);
    if (!ok || name.isEmpty()) return;
    name = name + ".md";

    QFile file(root + "/" + name);
    if (!file.exists()) file.open(QIODevice::WriteOnly);
}


void filebrowser::on_actionNewCallendar_triggered()
{
    calconfig = new CalendarConfig(nullptr);
    connect(calconfig, &CalendarConfig::savedData, this, &filebrowser::createCalendar);
    calconfig->setWindowTitle("Create your calendar");
    calconfig->show();
}


void filebrowser::on_actionNewImageAnnotation_triggered()
{
    QSettings settings("zhopets", "SpaceTimeHeart");
    QString filePath = QFileDialog::getOpenFileName(this, "Please select an image", "/home", "Images (*.png *.jpg *.webp *.svg)" );
    if(filePath.isEmpty())
        return;
    QFile file(filePath);
    file.open(QIODevice::ReadOnly);
    QByteArray image = file.readAll();

    bool ok;
    QString name = QInputDialog::getText(
        this, "New Calendar", "Calendar file name:",
        QLineEdit::Normal, "untitled", &ok);
    if(!ok)
        return;
    if (!name.endsWith(IMAN_EXTENSION))
        name += IMAN_EXTENSION;
    QString fullPath = root + "/" + name;
    ImageAnnotationData *newIMAN = new ImageAnnotationData();
    newIMAN->imageData = image;
    QFileInfo info(filePath);
    newIMAN->imageFormat = info.suffix();
    newIMAN->myPath = fullPath;
    if(!newIMAN->save(fullPath, *newIMAN)) {
        QMessageBox::warning(this, "Error",
                             "Failed to save image annotation file:\n" + filePath);
    }
}

void filebrowser::on_actionNewFolder_triggered()
{
    bool ok;
    QString name = QInputDialog::getText(this, "New Folder", "Folder name:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    QDir dir(root);
    dir.mkdir(name);
}

void filebrowser::createCalendar()
{
    bool ok;
    QString name = QInputDialog::getText(
        this, "New Calendar", "Calendar file name:",
        QLineEdit::Normal, "untitled", &ok);

    if (!ok || name.isEmpty()) {
        calconfig->close();
        calconfig->deleteLater();
        calconfig = nullptr;
        return;
    }
    if (!name.endsWith(CCAL_EXTENSION))
        name += CCAL_EXTENSION;

    QString fullPath = root + "/" + name;
    CalendarData fileData;
    fileData.config = calconfig->toConfigData();
    fileData.config.calendarName = name.chopped(CCAL_EXTENSION.size()); // strip extension for display name

    if (!fileData.save(fullPath, fileData)) {
        QMessageBox::warning(this, "Error",
                             "Failed to save calendar file:\n" + fullPath);
    }

    calconfig->close();
    calconfig->deleteLater();
    calconfig = nullptr;
}


void filebrowser::on_actionDelete_triggered()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete this?",
                                  "Are you sure you want to delete this object?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        emit goToEmpty(EMPTY);
        deleteSelected();
        view->clearSelection();
    }
}


void filebrowser::on_actionMindMap_triggered()
{
    emit openMindMap();
}

