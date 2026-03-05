#include "include/editors/markdowneditor.h"
#include "ui_markdowneditor.h"

MarkdownEditor::MarkdownEditor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MarkdownEditor)
{
    ui->setupUi(this);
}

MarkdownEditor::~MarkdownEditor()
{
    delete ui;
}

QTextBrowser* MarkdownEditor::GetQTextEdit()
{
    if (!myTextEdit)
    {
        QToolBar *toolbar = ui->toolBar;
        QLabel* iconLabel = new QLabel();
        QIcon icon = QIcon::fromTheme("zoom-in");
        iconLabel->setPixmap(icon.pixmap(toolbar->iconSize()));
        QSlider *slider = new QSlider(Qt::Horizontal);
        QAction *separator = new QAction();
        separator->setSeparator(true);
        toolbar->addWidget(iconLabel);
        toolbar->addWidget(slider);
        toolbar->addAction(separator);
        slider->setMinimum(-5);
        slider->setMaximum(10);
        slider->setMaximumWidth(200);
        connect(slider, &QSlider::valueChanged, this, &MarkdownEditor::updateZoom);
        toolbar->addAction(separator);


        myTextView = new CustomTextBrowser(ui->centralwidget);
        myTextEdit = new CustomTextBrowser(ui->centralwidget);
        myTextEdit->setReadOnly(false);

        connect(myTextEdit, &CustomTextBrowser::textChanged, this, &MarkdownEditor::hyperlinkTextEdit_textChanged);


        textEditScroll = myTextEdit->verticalScrollBar();
        textViewScroll = myTextView->verticalScrollBar();
        connect(myTextEdit->verticalScrollBar(), &QScrollBar::valueChanged,
                this, &MarkdownEditor::myTextEdit_scrolled);
        connect(myTextView->verticalScrollBar(), &QScrollBar::valueChanged,
                this, &MarkdownEditor::myTextView_scrolled);


        myFRWidget = new FindReplaceWidget(this, myTextEdit);
        QLayout *l = ui->centralwidget->layout();
        QHBoxLayout *hbox = (QHBoxLayout*)l;
        hbox->insertWidget(0, myFRWidget);
        ui->HolderOfText->layout()->addWidget(myTextView);
        ui->HolderOfText->layout()->addWidget(myTextEdit);
        myFRWidget->show(false);

        markdownUpdateTimer = new QTimer(this);
        markdownUpdateTimer->setSingleShot(true);
        connect(markdownUpdateTimer, &QTimer::timeout, this, [this]() {
            myTextView->setMarkdown(myTextEdit->toPlainText());
            myTextEdit_scrolled(textEditScroll->value());
        });

        updateZoom(startZoom);
        slider->setValue(currZoom);
    }
    return myTextEdit;
}

void MarkdownEditor::updateZoom(int zoom)
{
    myTextEdit->zoomOut(currZoom);
    currZoom = zoom;
    emit zoomChanged(currZoom);
    myTextEdit->zoomIn(currZoom);
}



void MarkdownEditor::on_actionFind_toggled(bool checked)
{
    myFRWidget->show(checked);
    myTextEdit->highlightText(checked, "");
}

void MarkdownEditor::hyperlinkTextEdit_textChanged()
{
    markdownUpdateTimer->start(1000);
    emit Updated();
}


void MarkdownEditor::myTextEdit_scrolled(int value)
{
    if (syncingScroll) return;
    syncingScroll = true;

    int srcRange = textEditScroll->maximum() - textEditScroll->minimum();
    int dstRange = textViewScroll->maximum() - textViewScroll->minimum();

    if (srcRange > 0 && dstRange > 0)
    {
        float percent = float(value - textEditScroll->minimum()) / srcRange;
        percent = qBound(0.0f, percent, 1.0f);
        textViewScroll->setValue(percent * dstRange + textViewScroll->minimum());
    }

    syncingScroll = false;
}

void MarkdownEditor::myTextView_scrolled(int value)
{
    if (syncingScroll) return;
    syncingScroll = true;

    int srcRange = textViewScroll->maximum() - textViewScroll->minimum();
    int dstRange = textEditScroll->maximum() - textEditScroll->minimum();

    if (srcRange > 0 && dstRange > 0)
    {
        float percent = float(value - textViewScroll->minimum()) / srcRange;
        percent = qBound(0.0f, percent, 1.0f);
        textEditScroll->setValue(percent * dstRange + textEditScroll->minimum());
    }

    syncingScroll = false;
}

void MarkdownEditor::on_actionUndo_triggered()
{
    myTextEdit->undo();
}


void MarkdownEditor::on_actionRedo_triggered()
{
    myTextEdit->redo();
}


void MarkdownEditor::on_actionSave_triggered()
{
    emit saveButton();
}


void MarkdownEditor::on_actionSaveAs_triggered()
{
    emit saveAsButton();
}


void MarkdownEditor::on_actionCut_triggered()
{
    myTextEdit->cut();
}


void MarkdownEditor::on_actionCopy_triggered()
{
    myTextEdit->copy();
}


void MarkdownEditor::on_actionPaste_triggered()
{
    myTextEdit->paste();
}

void MarkdownEditor::on_actionBold_triggered()
{
    QTextCursor cursor = myTextEdit->textCursor();
    if (!cursor.hasSelection())
        return;

    int origStart = cursor.selectionStart();
    int origEnd = cursor.selectionEnd();
    int selStart = origStart;
    int selEnd = origEnd;
    QString docText = myTextEdit->toPlainText();

    while (selStart > 0 && docText[selStart - 1] == '*')
        selStart--;
    while (selEnd < docText.length() && docText[selEnd] == '*')
        selEnd++;
    while (selStart > 0 && docText[selStart - 1] == '~')
        selStart--;
    while (selEnd < docText.length() && docText[selEnd] == '~')
        selEnd++;
    if (selStart >= 2 && docText.mid(selStart - 2, 2) == "++")
        selStart -= 2;
    if (selEnd + 2 <= docText.length() && docText.mid(selEnd, 2) == "++")
        selEnd += 2;

    bool selectedWordOnly = (selStart != origStart || selEnd != origEnd);

    cursor.setPosition(selStart);
    cursor.setPosition(selEnd, QTextCursor::KeepAnchor);
    QString fullText = cursor.selectedText();

    int pos = 0;
    int endPos = fullText.length();

    bool hasUnderline = false;
    if (fullText.startsWith("++") && fullText.endsWith("++")) {
        hasUnderline = true;
        pos = 2;
        endPos -= 2;
    }

    int tildeLeading = 0;
    while (pos < endPos && fullText[pos] == '~') { tildeLeading++; pos++; }
    int starLeading = 0;
    while (pos < endPos && fullText[pos] == '*') { starLeading++; pos++; }
    int tildeTrailing = 0;
    while (endPos > pos && fullText[endPos - 1] == '~') { tildeTrailing++; endPos--; }
    int starTrailing = 0;
    while (endPos > pos && fullText[endPos - 1] == '*') { starTrailing++; endPos--; }

    bool hasStrike = (qMin(tildeLeading, tildeTrailing) >= 2);
    int markers = qMin(qMin(starLeading, starTrailing), 3);
    QString word = fullText.mid(pos, endPos - pos);

    int newMarkers;
    if (markers == 3)      newMarkers = 1;
    else if (markers == 2) newMarkers = 0;
    else if (markers == 1) newMarkers = 3;
    else                   newMarkers = 2;

    QString starStr = QString("*").repeated(newMarkers);
    QString tildeStr = hasStrike ? "~~" : "";
    QString uOpen = hasUnderline ? "++" : "";
    QString uClose = hasUnderline ? "++" : "";
    QString result = uOpen + tildeStr + starStr + word + starStr + tildeStr + uClose;

    cursor.insertText(result);

    if (selectedWordOnly) {
        int wordStart = selStart + uOpen.length() + tildeStr.length() + starStr.length();
        cursor.setPosition(wordStart);
        cursor.setPosition(wordStart + word.length(), QTextCursor::KeepAnchor);
    } else {
        cursor.setPosition(selStart);
        cursor.setPosition(selStart + result.length(), QTextCursor::KeepAnchor);
    }
    myTextEdit->setTextCursor(cursor);
}

void MarkdownEditor::on_actionItalic_triggered()
{
    QTextCursor cursor = myTextEdit->textCursor();
    if (!cursor.hasSelection())
        return;

    int origStart = cursor.selectionStart();
    int origEnd = cursor.selectionEnd();
    int selStart = origStart;
    int selEnd = origEnd;
    QString docText = myTextEdit->toPlainText();

    while (selStart > 0 && docText[selStart - 1] == '*')
        selStart--;
    while (selEnd < docText.length() && docText[selEnd] == '*')
        selEnd++;
    while (selStart > 0 && docText[selStart - 1] == '~')
        selStart--;
    while (selEnd < docText.length() && docText[selEnd] == '~')
        selEnd++;
    if (selStart >= 2 && docText.mid(selStart - 2, 2) == "++")
        selStart -= 2;
    if (selEnd + 2 <= docText.length() && docText.mid(selEnd, 2) == "++")
        selEnd += 2;

    bool selectedWordOnly = (selStart != origStart || selEnd != origEnd);

    cursor.setPosition(selStart);
    cursor.setPosition(selEnd, QTextCursor::KeepAnchor);
    QString fullText = cursor.selectedText();

    int pos = 0;
    int endPos = fullText.length();

    bool hasUnderline = false;
    if (fullText.startsWith("++") && fullText.endsWith("++")) {
        hasUnderline = true;
        pos = 2;
        endPos -= 2;
    }

    int tildeLeading = 0;
    while (pos < endPos && fullText[pos] == '~') { tildeLeading++; pos++; }
    int starLeading = 0;
    while (pos < endPos && fullText[pos] == '*') { starLeading++; pos++; }
    int tildeTrailing = 0;
    while (endPos > pos && fullText[endPos - 1] == '~') { tildeTrailing++; endPos--; }
    int starTrailing = 0;
    while (endPos > pos && fullText[endPos - 1] == '*') { starTrailing++; endPos--; }

    bool hasStrike = (qMin(tildeLeading, tildeTrailing) >= 2);
    int markers = qMin(qMin(starLeading, starTrailing), 3);
    QString word = fullText.mid(pos, endPos - pos);

    int newMarkers;
    if (markers == 3)      newMarkers = 2;
    else if (markers == 2) newMarkers = 3;
    else if (markers == 1) newMarkers = 0;
    else                   newMarkers = 1;

    QString starStr = QString("*").repeated(newMarkers);
    QString tildeStr = hasStrike ? "~~" : "";
    QString uOpen = hasUnderline ? "++" : "";
    QString uClose = hasUnderline ? "++" : "";
    QString result = uOpen + tildeStr + starStr + word + starStr + tildeStr + uClose;

    cursor.insertText(result);

    if (selectedWordOnly) {
        int wordStart = selStart + uOpen.length() + tildeStr.length() + starStr.length();
        cursor.setPosition(wordStart);
        cursor.setPosition(wordStart + word.length(), QTextCursor::KeepAnchor);
    } else {
        cursor.setPosition(selStart);
        cursor.setPosition(selStart + result.length(), QTextCursor::KeepAnchor);
    }
    myTextEdit->setTextCursor(cursor);
}

void MarkdownEditor::on_actionStrikethrough_triggered()
{
    QTextCursor cursor = myTextEdit->textCursor();
    if (!cursor.hasSelection())
        return;

    int origStart = cursor.selectionStart();
    int origEnd = cursor.selectionEnd();
    int selStart = origStart;
    int selEnd = origEnd;
    QString docText = myTextEdit->toPlainText();

    while (selStart > 0 && docText[selStart - 1] == '*')
        selStart--;
    while (selEnd < docText.length() && docText[selEnd] == '*')
        selEnd++;
    while (selStart > 0 && docText[selStart - 1] == '~')
        selStart--;
    while (selEnd < docText.length() && docText[selEnd] == '~')
        selEnd++;
    if (selStart >= 2 && docText.mid(selStart - 2, 2) == "++")
        selStart -= 2;
    if (selEnd + 2 <= docText.length() && docText.mid(selEnd, 2) == "++")
        selEnd += 2;

    bool selectedWordOnly = (selStart != origStart || selEnd != origEnd);

    cursor.setPosition(selStart);
    cursor.setPosition(selEnd, QTextCursor::KeepAnchor);
    QString fullText = cursor.selectedText();

    int pos = 0;
    int endPos = fullText.length();

    bool hasUnderline = false;
    if (fullText.startsWith("++") && fullText.endsWith("++")) {
        hasUnderline = true;
        pos = 2;
        endPos -= 2;
    }

    int tildeLeading = 0;
    while (pos < endPos && fullText[pos] == '~') { tildeLeading++; pos++; }
    int starLeading = 0;
    while (pos < endPos && fullText[pos] == '*') { starLeading++; pos++; }
    int tildeTrailing = 0;
    while (endPos > pos && fullText[endPos - 1] == '~') { tildeTrailing++; endPos--; }
    int starTrailing = 0;
    while (endPos > pos && fullText[endPos - 1] == '*') { starTrailing++; endPos--; }

    bool hasStrike = (qMin(tildeLeading, tildeTrailing) >= 2);
    int markers = qMin(qMin(starLeading, starTrailing), 3);
    QString word = fullText.mid(pos, endPos - pos);

    bool newStrike = !hasStrike;

    QString starStr = QString("*").repeated(markers);
    QString tildeStr = newStrike ? "~~" : "";
    QString uOpen = hasUnderline ? "++" : "";
    QString uClose = hasUnderline ? "++" : "";
    QString result = uOpen + tildeStr + starStr + word + starStr + tildeStr + uClose;

    cursor.insertText(result);

    if (selectedWordOnly) {
        int wordStart = selStart + uOpen.length() + tildeStr.length() + starStr.length();
        cursor.setPosition(wordStart);
        cursor.setPosition(wordStart + word.length(), QTextCursor::KeepAnchor);
    } else {
        cursor.setPosition(selStart);
        cursor.setPosition(selStart + result.length(), QTextCursor::KeepAnchor);
    }
    myTextEdit->setTextCursor(cursor);
}

void MarkdownEditor::on_actionUnderline_triggered()
{
    QTextCursor cursor = myTextEdit->textCursor();
    if (!cursor.hasSelection())
        return;

    int origStart = cursor.selectionStart();
    int origEnd = cursor.selectionEnd();
    int selStart = origStart;
    int selEnd = origEnd;
    QString docText = myTextEdit->toPlainText();

    while (selStart > 0 && docText[selStart - 1] == '*')
        selStart--;
    while (selEnd < docText.length() && docText[selEnd] == '*')
        selEnd++;
    while (selStart > 0 && docText[selStart - 1] == '~')
        selStart--;
    while (selEnd < docText.length() && docText[selEnd] == '~')
        selEnd++;
    if (selStart >= 2 && docText.mid(selStart - 2, 2) == "++")
        selStart -= 2;
    if (selEnd + 2 <= docText.length() && docText.mid(selEnd, 2) == "++")
        selEnd += 2;

    bool selectedWordOnly = (selStart != origStart || selEnd != origEnd);

    cursor.setPosition(selStart);
    cursor.setPosition(selEnd, QTextCursor::KeepAnchor);
    QString fullText = cursor.selectedText();

    int pos = 0;
    int endPos = fullText.length();

    bool hasUnderline = false;
    if (fullText.startsWith("++") && fullText.endsWith("++")) {
        hasUnderline = true;
        pos = 2;
        endPos -= 2;
    }

    int tildeLeading = 0;
    while (pos < endPos && fullText[pos] == '~') { tildeLeading++; pos++; }
    int starLeading = 0;
    while (pos < endPos && fullText[pos] == '*') { starLeading++; pos++; }
    int tildeTrailing = 0;
    while (endPos > pos && fullText[endPos - 1] == '~') { tildeTrailing++; endPos--; }
    int starTrailing = 0;
    while (endPos > pos && fullText[endPos - 1] == '*') { starTrailing++; endPos--; }

    bool hasStrike = (qMin(tildeLeading, tildeTrailing) >= 2);
    int markers = qMin(qMin(starLeading, starTrailing), 3);
    QString word = fullText.mid(pos, endPos - pos);

    bool newUnderline = !hasUnderline;

    QString starStr = QString("*").repeated(markers);
    QString tildeStr = hasStrike ? "~~" : "";
    QString uOpen = newUnderline ? "++" : "";
    QString uClose = newUnderline ? "++" : "";
    QString result = uOpen + tildeStr + starStr + word + starStr + tildeStr + uClose;

    cursor.insertText(result);

    if (selectedWordOnly) {
        int wordStart = selStart + uOpen.length() + tildeStr.length() + starStr.length();
        cursor.setPosition(wordStart);
        cursor.setPosition(wordStart + word.length(), QTextCursor::KeepAnchor);
    } else {
        cursor.setPosition(selStart);
        cursor.setPosition(selStart + result.length(), QTextCursor::KeepAnchor);
    }
    myTextEdit->setTextCursor(cursor);
}

void MarkdownEditor::on_actionDotList_triggered()
{
    QTextCursor cursor = myTextEdit->textCursor();
    int save = cursor.position();
    int anchSave = cursor.anchor();

    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.insertText(" - ");

    cursor.setPosition(anchSave);
    cursor.setPosition(save, QTextCursor::KeepAnchor);
}
\
void MarkdownEditor::on_actionNumberedList_triggered()
{

}

void MarkdownEditor::on_actionMarkList_triggered()
{

}

void MarkdownEditor::on_actionHeader_triggered()
{

}


void MarkdownEditor::on_actionLine_triggered()
{

}


void MarkdownEditor::on_actionQuote_triggered()
{

}


void MarkdownEditor::on_actionCodeBlock_triggered()
{

}

