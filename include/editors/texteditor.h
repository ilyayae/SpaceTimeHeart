#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QMainWindow>
#include <QTextBrowser>
#include <QClipboard>
#include <QSlider>
#include <QLabel>
#include <QListWidget>
#include <QDialog>
#include <QDialogButtonBox>

#include "include/subclasses/customtextbrowser.h"
#include "include/subclasses/findreplacewidget.h"
#include <include/noteTypes/uuidregistry.h>

namespace Ui {
class TextEditor;
}

class TextEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit TextEditor(QWidget *parent = nullptr, UuidRegistry *reg = nullptr);
    ~TextEditor();
    CustomTextBrowser* getQTextEdit();
    void highlightText(bool b);
    QList<QUuid> getUuids();
    int startZoom = 0;
    int currZoom = 0;
    UuidRegistry *registry;

public slots:
    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionSave_triggered();

    void on_actionSaveAs_triggered();

    void on_actionCut_triggered();

    void on_actionCopy_triggered();

    void on_actionPaste_triggered();

    void on_actionFind_toggled(bool arg1);

    void updateZoom(int zoom);

    void hyperlinkTextEdit_textChanged();

    void on_actionInsertLink_triggered();

private:
    Ui::TextEditor *ui;
    CustomTextBrowser *myTextEdit = nullptr;
    FindReplaceWidget *myFRWidget = nullptr;
    QList<int> textFoundPositions;
    int currentFoundPosition;
    int currentFoundWordLength;
    bool isActivelySearchingWords;
signals:
    void saveButton();
    void saveAsButton();
    void Updated();
    void zoomChanged(int zoom);
};
#endif // TEXTEDITOR_H
