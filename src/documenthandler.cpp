#include "include/documenthandler.h"
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>

DocumentHandler::DocumentHandler(QWidget *parent, QSettings *settings, QGridLayout *editorPlace)
    : QWidget{parent}
{
    Settings = settings;
    EditorPlace = editorPlace;
    currentEditor = NUL;
    switchEditor(EMPTY);
    //Settings->value("general/WorkDirectory", "/home").toString()
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(appDataPath);
    registry = new UuidRegistry(appDataPath + "/notes_registry.db", this);
}
DocumentHandler::~DocumentHandler()
{
    if (textEdit) textEdit->deleteLater();
    if (emptyEditor) emptyEditor->deleteLater();
    if (textEditor) textEditor->deleteLater();
    if (markdownEditor) markdownEditor->deleteLater();
    if (calendarEditor) calendarEditor->deleteLater();
    if (imageAnnotationEditor) imageAnnotationEditor->deleteLater();
    if (htmlEditor) htmlEditor->deleteLater();
}

void DocumentHandler::loadFile(QString fileName) {
    if (fileName.isEmpty()) return;
    QFileInfo info(fileName);
    QString suffix = info.completeSuffix();

    if (suffix == "txt")
    {
        switchEditor(TEXT);
    } else if (suffix == "md")
    {
        switchEditor(MARKDOWN);
    } else if (suffix == "ccal")
    {
        switchEditor(CALENDAR);
    } else if (suffix == "iman")
    {
        switchEditor(IMAGEANNOTATION);
    } else
    {
        switchEditor(EMPTY);
    }

    if (textEdit != nullptr)
    {
        saveable = false;

        currentNote = new Note(fileName);
        textEdit->setPlainText(currentNote->getContent());
        filePath = fileName;

        saveable = true;
    }
    else if (calendarEditor != nullptr)
    {
        saveable = false;
        currentCalendar = new CalendarData();
        currentCalendar->load(fileName, *currentCalendar);
        calendarEditor->Initialize(currentCalendar);
        filePath = fileName;
        saveable = true;
    }
    else if (imageAnnotationEditor != nullptr)
    {
        saveable = false;
        currentImageAnnotation = new ImageAnnotationData();
        currentImageAnnotation->load(fileName, *currentImageAnnotation);
        imageAnnotationEditor->Initialize(currentImageAnnotation);
        filePath = fileName;
        saveable = true;
    }

}

void DocumentHandler::saveFile() {
    if (filePath.isEmpty()) {
        saveAsFile();
    }
    else
    {
        if(currentEditor == TEXT || currentEditor == MARKDOWN)
        {
            if(currentNote != nullptr)
            {
                currentNote->setContent(textEdit->toPlainText());
                currentNote->save();
                registry->writeEntry(currentNote->getUuid(), currentNote->getPath());
            }
        }
        else if (currentEditor == CALENDAR)
        {
            if(currentCalendar != nullptr)
            {
                currentCalendar->save(filePath, *currentCalendar);
                registry->writeEntry(currentCalendar->GetUuid(), currentCalendar->GetPath());
            }
        }
        else if (currentEditor == IMAGEANNOTATION)
        {
            if(currentImageAnnotation != nullptr)
            {
                currentImageAnnotation->save(filePath, *currentImageAnnotation);
                registry->writeEntry(currentImageAnnotation->GetUuid(), currentImageAnnotation->GetPath());
            }
        }
    }
}

void DocumentHandler::saveAsFile() {

    filePath = QFileDialog::getSaveFileName(nullptr, tr("Save File As"), Settings->value("general/WorkDirectory", "/home").toString(), tr("Text Files (*.txt);;All Files (*)"));
    if (filePath.isEmpty()) return;

    if(currentEditor == TEXT || currentEditor == MARKDOWN)
    {
        currentNote->setContent(textEdit->toPlainText());
        currentNote->saveAs(filePath);
        registry->writeEntry(currentNote->getUuid(), currentNote->getPath());
    }
    else if (currentEditor == CALENDAR)
    {
        currentCalendar->save(filePath, *currentCalendar);
        registry->writeEntry(currentCalendar->GetUuid(), currentCalendar->GetPath());
    }
    else if (currentEditor == IMAGEANNOTATION)
    {
        currentImageAnnotation->save(filePath, *currentImageAnnotation);
        registry->writeEntry(currentImageAnnotation->GetUuid(), currentImageAnnotation->GetPath());
    }
}

void DocumentHandler::switchEditor(CurrentEditor SwitchTo)
{
    if(textEdit != nullptr || currentCalendar != nullptr || currentImageAnnotation != nullptr)
    {
        saveFile();
    }
    switch (currentEditor) {
    case EMPTY:
        EditorPlace->removeWidget(emptyEditor);
        emptyEditor->deleteLater();
        emptyEditor = nullptr;
        break;
    case TEXT:
        EditorPlace->removeWidget(textEditor);
        textEditor->deleteLater();
        textEditor = nullptr;
        break;
    case MARKDOWN:
        EditorPlace->removeWidget(markdownEditor);
        markdownEditor->deleteLater();
        markdownEditor = nullptr;
        break;
    case CALENDAR:
        EditorPlace->removeWidget(calendarEditor);
        calendarEditor->deleteLater();
        calendarEditor = nullptr;
        break;
    case IMAGEANNOTATION:
        EditorPlace->removeWidget(imageAnnotationEditor);
        imageAnnotationEditor->deleteLater();
        imageAnnotationEditor = nullptr;
        break;
    default:
        break;
    }

    switch (SwitchTo) {
    case EMPTY:
        emptyEditor = new EmptyEditor();
        EditorPlace->addWidget(emptyEditor);
        textEdit = nullptr;
        break;
    case TEXT:
        textEditor = new TextEditor();
        textEditor->startZoom = currZoom;
        EditorPlace->addWidget(textEditor);
        textEdit = textEditor->getQTextEdit();
        connect(textEditor, &TextEditor::saveButton, this, &DocumentHandler::saveFile);
        connect(textEditor, &TextEditor::saveAsButton, this, &DocumentHandler::saveAsFile);
        connect(textEditor, &TextEditor::Updated, this, &DocumentHandler::emitUpdate);
        connect(textEditor, &TextEditor::zoomChanged, this, [this](int Zoom) {
            currZoom = Zoom;
        });
        connect((CustomTextBrowser*)textEdit, &CustomTextBrowser::uuidClicked, this, &DocumentHandler::parseUuid);
        break;
    case MARKDOWN:
        markdownEditor = new MarkdownEditor();
        markdownEditor->startZoom = currZoom;
        EditorPlace->addWidget(markdownEditor);
        textEdit = markdownEditor->GetQTextEdit();
        connect(markdownEditor, &MarkdownEditor::saveButton, this, &DocumentHandler::saveFile);
        connect(markdownEditor, &MarkdownEditor::saveAsButton, this, &DocumentHandler::saveAsFile);
        connect(markdownEditor, &MarkdownEditor::Updated, this, &DocumentHandler::emitUpdate);
        connect(markdownEditor, &MarkdownEditor::zoomChanged, this, [this](int Zoom) {
            currZoom = Zoom;
        });
        connect((CustomTextBrowser*)markdownEditor, &CustomTextBrowser::uuidClicked, this, &DocumentHandler::parseUuid);
        break;
    case CALENDAR:
        calendarEditor = new CalendarEditor();
        calendarEditor->myRegistry = registry;
        EditorPlace->addWidget(calendarEditor);
        connect(calendarEditor, &CalendarEditor::Updated, this, &DocumentHandler::emitUpdate);
        connect(calendarEditor, &CalendarEditor::uuidClicked, this, &DocumentHandler::parseUuid);
        textEdit = nullptr;
        break;
    case IMAGEANNOTATION:
        imageAnnotationEditor = new ImageAnnotationEditor();
        imageAnnotationEditor->myRegistry = registry;
        EditorPlace->addWidget(imageAnnotationEditor);
        connect(imageAnnotationEditor, &ImageAnnotationEditor::uuidClicked, this, &DocumentHandler::parseUuid);
        textEdit = nullptr;
        break;
    default:
        break;
    }
    currentEditor = SwitchTo;
}

void DocumentHandler::emitUpdate()
{
    emit fileUpdated();
}

void DocumentHandler::parseUuid(QUuid uuid)
{
    QString string = registry->getPath(uuid);
    loadFile(string);
    emit linkFollowed(string);
}
