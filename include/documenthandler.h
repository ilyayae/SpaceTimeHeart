#ifndef DOCUMENTHANDLER_H
#define DOCUMENTHANDLER_H

#include <QWidget>
#include <QObject>
#include <QTextEdit>
#include <QSettings>
#include <QGridLayout>

#include <include/noteTypes/note.h>
#include <include/noteTypes/uuidregistry.h>

#include <include/editors/emptyeditor.h>
#include <include/editors/texteditor.h>
#include <include/editors/markdowneditor.h>
#include <include/editors/htmleditor.h>
#include <include/editors/calendareditor.h>
#include <include/editors/imageannotationeditor.h>
#include <include/noteTypes/calendardata.h>


enum CurrentEditor {
    EMPTY,
    TEXT,
    MARKDOWN,
    CALENDAR,
    IMAGEANNOTATION,
    NUL
};

class DocumentHandler : public QWidget
{
    Q_OBJECT
public:
    explicit DocumentHandler(QWidget *parent = nullptr,  QSettings *settings = nullptr, QGridLayout *editorPlace = nullptr);
    void loadFile(QString fileName);
    void saveFile();
    void saveAsFile();
    void switchEditor(CurrentEditor SwitchTo);
    void parseUuid(QUuid uuid);
    void emitUpdate();

private:
    QString filePath;
    bool saveable = true;
    QSettings *Settings;
    QGridLayout *EditorPlace;

    Note *currentNote;
    CalendarData *currentCalendar;
    UuidRegistry *registry;
    int currZoom = 0;

    //Editors
    CurrentEditor currentEditor;
    QTextBrowser *textEdit = nullptr;
    EmptyEditor *emptyEditor;
    TextEditor *textEditor;
    MarkdownEditor *markdownEditor;
    CalendarEditor *calendarEditor;
    ImageAnnotationEditor *imageAnnotationEditor;
    HtmlEditor *htmlEditor;
signals:
    void fileUpdated();
    void linkFollowed(const QString &path);
};

#endif // DOCUMENTHANDLER_H
