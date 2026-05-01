#include "include/filebrowser.h"
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
    model->setReadOnly(false);

    view = new QTreeView(this);
    view->setModel(model);
    view->setRootIndex(model->index(root));
    view->setHeaderHidden(true);
    view->setColumnHidden(1, true);
    view->setColumnHidden(2, true);
    view->setColumnHidden(3, true);
    view->setIndentation(10);
    view->viewport()->installEventFilter(this);
    view->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    view->setDragEnabled(true);
    view->setAcceptDrops(true);
    view->setDropIndicatorShown(true);
    view->setDragDropMode(QAbstractItemView::InternalMove);


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

bool filebrowser::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == view->viewport() && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        QModelIndex index = view->indexAt(mouseEvent->pos());

        if (!index.isValid()) {
            view->selectionModel()->clearSelection();
            view->setCurrentIndex(QModelIndex());
        }
    }
    return QObject::eventFilter(obj, event);
}

QString filebrowser::currentFilePath() const
{
    QModelIndex index = view->currentIndex();
    return model->filePath(index);
}

QString filebrowser::getUniquePath(const QString &baseName, const QString &extension)
{
    QModelIndex currentIndex = view->selectionModel()->currentIndex();
    QString targetDirectory;

    if (currentIndex.isValid()) {
        QString path = model->filePath(currentIndex);
        QFileInfo info(path);
        targetDirectory = info.isDir() ? path : info.absolutePath();
    } else {
        targetDirectory = root;
    }

    QString fullPath = targetDirectory + "/" + baseName + extension;
    QFileInfo checkFile(fullPath);

    int counter = 1;
    while (checkFile.exists()) {
        fullPath = root + "/" + baseName + "(" + QString::number(counter) + ")" + extension;
        checkFile.setFile(fullPath);
        counter++;
    }
    return fullPath;
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
    QString baseName;
    while (true) {
        baseName = QInputDialog::getText(this, "New Note", "Note name:", QLineEdit::Normal, "untitled", &ok);

        if (!ok) return;
        if (baseName.isEmpty()) continue;

        static QRegularExpression illegalChars("[\\\\/:*?\"<>|]");
        if (baseName.contains(illegalChars)) {
            QMessageBox::warning(this, "Invalid Name",
                                 "A filename cannot contain any of the following characters: \n \\ / : * ? \" < > |");
            continue;
        }

        break;
    }
    if (!ok || baseName.isEmpty()) return;

    QString fullPath = getUniquePath(baseName, ".txt");
    QFile file(fullPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "Hello World!";
        file.close();
    }
}

void filebrowser::on_actionNewMarkdownNote_triggered()
{
    bool ok;
    QString baseName;
    while (true) {
        baseName = QInputDialog::getText(this, "New Note", "Note name:", QLineEdit::Normal, "untitled", &ok);

        if (!ok) return;
        if (baseName.isEmpty()) continue;

        static QRegularExpression illegalChars("[\\\\/:*?\"<>|]");
        if (baseName.contains(illegalChars)) {
            QMessageBox::warning(this, "Invalid Name",
                                 "A filename cannot contain any of the following characters: \n \\ / : * ? \" < > |");
            continue;
        }

        break;
    }
    if (!ok || baseName.isEmpty()) return;

    QString fullPath = getUniquePath(baseName, ".md");
    QFile file(fullPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "# Hello! \n ## Use Show/Hide Editor button to start editing!";
        file.close();
    }
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
    QString baseName;
    while (true) {
        baseName = QInputDialog::getText(this, "New Note", "Note name:", QLineEdit::Normal, "untitled", &ok);

        if (!ok) return;
        if (baseName.isEmpty()) continue;

        static QRegularExpression illegalChars("[\\\\/:*?\"<>|]");
        if (baseName.contains(illegalChars)) {
            QMessageBox::warning(this, "Invalid Name",
                                 "A filename cannot contain any of the following characters: \n \\ / : * ? \" < > |");
            continue;
        }

        break;
    }
    if(!ok)
        return;
    QString fullPath = getUniquePath(baseName, ".iman");
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
    QString baseName;
    while (true) {
        baseName = QInputDialog::getText(this, "New Note", "Note name:", QLineEdit::Normal, "untitled", &ok);

        if (!ok) return;
        if (baseName.isEmpty()) continue;

        static QRegularExpression illegalChars("[\\\\/:*?\"<>|]");
        if (baseName.contains(illegalChars)) {
            QMessageBox::warning(this, "Invalid Name",
                                 "A filename cannot contain any of the following characters: \n \\ / : * ? \" < > |");
            continue;
        }

        break;
    }
    if (!ok || baseName.isEmpty()) return;

    QDir dir(root);
    dir.mkdir(baseName);
}

void filebrowser::createCalendar()
{
    bool ok;
    QString baseName;
    while (true) {
        baseName = QInputDialog::getText(this, "New Note", "Note name:", QLineEdit::Normal, "untitled", &ok);

        if (!ok) return;
        if (baseName.isEmpty()) continue;

        static QRegularExpression illegalChars("[\\\\/:*?\"<>|]");
        if (baseName.contains(illegalChars)) {
            QMessageBox::warning(this, "Invalid Name",
                                 "A filename cannot contain any of the following characters: \n \\ / : * ? \" < > |");
            continue;
        }

        break;
    }

    if (!ok || baseName.isEmpty()) {
        calconfig->close();
        calconfig->deleteLater();
        calconfig = nullptr;
        return;
    }
    QString fullPath = getUniquePath(baseName, ".ccal");
    CalendarData fileData;
    fileData.config = calconfig->toConfigData();
    fileData.config.calendarName = baseName.chopped(CCAL_EXTENSION.size()); // strip extension for display name

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
        view->selectionModel()->clearSelection();
    }
}


void filebrowser::on_actionMindMap_triggered()
{
    emit openMindMap();
}

