#ifndef DOCUMENTHANDLER_H
#define DOCUMENTHANDLER_H

#include <QWidget>
#include <QObject>
#include <QTextEdit>
#include <QSettings>
#include <QGridLayout>
#include <emptyeditor.h>
#include <texteditor.h>
#include <markdowneditor.h>
#include <htmleditor.h>

enum CurrentEditor {
    EMPTY,
    TEXT,
    MARKDOWN,
    HTML,
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

private:
    QString filePath;
    bool saveable = true;
    QSettings *Settings;
    QGridLayout *EditorPlace;

    //Editors
    CurrentEditor currentEditor;
    QTextEdit *textEdit;
    EmptyEditor *emptyEditor;
    TextEditor *textEditor;
    MarkdownEditor *markdownEditor;
    HtmlEditor *htmlEditor;
signals:
    void fileUpdated();
};

#endif // DOCUMENTHANDLER_H
