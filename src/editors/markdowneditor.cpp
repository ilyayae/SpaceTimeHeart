#include "include/editors/markdowneditor.h"
#include "ui_markdowneditor.h"

MarkdownEditor::MarkdownEditor(QWidget *parent, UuidRegistry *reg)
    : QMainWindow(parent)
    , ui(new Ui::MarkdownEditor)
{
    ui->setupUi(this);
    registry = reg;
}

MarkdownEditor::~MarkdownEditor()
{
    delete ui;
}

CustomTextBrowser* MarkdownEditor::GetQTextEdit()
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
        myTextEdit->registry = registry;
        myTextView->registry = registry;

        connect(myTextEdit, &CustomTextBrowser::textChanged, this, &MarkdownEditor::hyperlinkTextEdit_textChanged);
        connect(myTextEdit, &CustomTextBrowser::uuidClicked, this, [this](QUuid uuid){
            emit uuidClicked(uuid);
        });
        connect(myTextView, &CustomTextBrowser::mdCheckboxChanged, this, &MarkdownEditor::setEditorText);
        connect(myTextView, &CustomTextBrowser::uuidClicked, this, [this](QUuid uuid){
            emit uuidClicked(uuid);
        });


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
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();

    QTextCursor iterCursor(myTextEdit->document());
    iterCursor.setPosition(start);
    int startBlock = iterCursor.blockNumber();
    iterCursor.setPosition(end);
    int endBlock = iterCursor.blockNumber();

    QRegularExpression prefixRegex(R"(^((?:   )*)(-\[x\]\s?|-\[\]\s?|-\s?|\d+\.\s?))");

    iterCursor.beginEditBlock();
    for (int i = startBlock; i <= endBlock; ++i) {
        QTextBlock block = myTextEdit->document()->findBlockByNumber(i);
        iterCursor.setPosition(block.position());
        iterCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        QString lineText = iterCursor.selectedText();

        if (lineText.trimmed().isEmpty())
            continue;

        QRegularExpressionMatch match = prefixRegex.match(lineText + " ");

        if (match.hasMatch()) {
            QString indent = match.captured(1);
            QString marker = match.captured(2);

            if (QRegularExpression(R"(^-\s?)").match(marker).hasMatch()) {
                QString content = lineText.mid(match.capturedLength());
                iterCursor.insertText(content);
            }

            else {
                QString newLine = indent + "- " + lineText.mid(match.capturedLength());
                iterCursor.insertText(newLine);
            }
        }
        else {
            QRegularExpression leadingSpaces(R"(^(\s*))");
            QRegularExpressionMatch spaceMatch = leadingSpaces.match(lineText);
            QString indent = spaceMatch.captured(1);
            QString content = lineText.mid(indent.length());
            iterCursor.insertText(indent + "- " + content);
        }
    }
    iterCursor.endEditBlock();
    cursor = myTextEdit->textCursor();
    int maxPos = myTextEdit->document()->characterCount() - 1;
    cursor.setPosition(qMin(anchSave, maxPos));
    cursor.setPosition(qMin(save, maxPos), QTextCursor::KeepAnchor);
    myTextEdit->setTextCursor(cursor);
}

void MarkdownEditor::on_actionNumberedList_triggered()
{
    QTextCursor cursor = myTextEdit->textCursor();
    int save = cursor.position();
    int anchSave = cursor.anchor();
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();

    QTextCursor iterCursor(myTextEdit->document());
    iterCursor.setPosition(start);
    int startBlock = iterCursor.blockNumber();
    iterCursor.setPosition(end);
    int endBlock = iterCursor.blockNumber();

    QRegularExpression prefixRegex(R"(^((?:   )*)(-\[x\]\s?|-\[\]\s?|-\s?|\d+\.\s?))");

    QMap<int, int> indentCounters;

    iterCursor.beginEditBlock();
    for (int i = startBlock; i <= endBlock; ++i) {
        QTextBlock block = myTextEdit->document()->findBlockByNumber(i);
        iterCursor.setPosition(block.position());
        iterCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        QString lineText = iterCursor.selectedText();

        if (lineText.trimmed().isEmpty())
            continue;

        QRegularExpressionMatch match = prefixRegex.match(lineText + " ");

        if (match.hasMatch()) {
            QString indent = match.captured(1);
            QString marker = match.captured(2);
            int indentLevel = indent.length() / 3;

            if (QRegularExpression(R"(^\d+\.\s?)").match(marker).hasMatch()) {
                QString content = lineText.mid(match.capturedLength());
                iterCursor.insertText(content);
                QList<int> keys = indentCounters.keys();
                for (int k : keys) {
                    if (k >= indentLevel) indentCounters.remove(k);
                }
            }
            else {
                indentCounters[indentLevel] = indentCounters.value(indentLevel, 0) + 1;
                QList<int> keys = indentCounters.keys();
                for (int k : keys) {
                    if (k > indentLevel) indentCounters.remove(k);
                }
                QString newLine = indent + QString::number(indentCounters[indentLevel]) + ". " + lineText.mid(match.capturedLength());
                iterCursor.insertText(newLine);
            }
        }
        else {
            QRegularExpression leadingSpaces(R"(^(\s*))");
            QRegularExpressionMatch spaceMatch = leadingSpaces.match(lineText);
            QString indent = spaceMatch.captured(1);
            int indentLevel = indent.length() / 3;
            QString content = lineText.mid(indent.length());

            indentCounters[indentLevel] = indentCounters.value(indentLevel, 0) + 1;
            QList<int> keys = indentCounters.keys();
            for (int k : keys) {
                if (k > indentLevel) indentCounters.remove(k);
            }
            iterCursor.insertText(indent + QString::number(indentCounters[indentLevel]) + ". " + content);
        }
    }
    iterCursor.endEditBlock();

    cursor = myTextEdit->textCursor();
    int maxPos = myTextEdit->document()->characterCount() - 1;
    cursor.setPosition(qMin(anchSave, maxPos));
    cursor.setPosition(qMin(save, maxPos), QTextCursor::KeepAnchor);
    myTextEdit->setTextCursor(cursor);
}

void MarkdownEditor::on_actionMarkList_triggered()
{
    QTextCursor cursor = myTextEdit->textCursor();
    int save = cursor.position();
    int anchSave = cursor.anchor();
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();

    QTextCursor iterCursor(myTextEdit->document());
    iterCursor.setPosition(start);
    int startBlock = iterCursor.blockNumber();
    iterCursor.setPosition(end);
    int endBlock = iterCursor.blockNumber();

    QRegularExpression prefixRegex(R"(^((?:   )*)(-\[x\]\s?|-\[\]\s?|-\s?|\d+\.\s?))");

    iterCursor.beginEditBlock();
    for (int i = startBlock; i <= endBlock; ++i) {
        QTextBlock block = myTextEdit->document()->findBlockByNumber(i);
        iterCursor.setPosition(block.position());
        iterCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        QString lineText = iterCursor.selectedText();

        if (lineText.trimmed().isEmpty())
            continue;

        QRegularExpressionMatch match = prefixRegex.match(lineText + " ");

        if (match.hasMatch()) {
            QString indent = match.captured(1);
            QString marker = match.captured(2);
            if (QRegularExpression(R"(^-\[x\]\s?|^-\[\]\s?)").match(marker).hasMatch()) {
                QString content = lineText.mid(match.capturedLength());
                iterCursor.insertText(content);
            }
            else {
                QString newLine = indent + "-[] " + lineText.mid(match.capturedLength());
                iterCursor.insertText(newLine);
            }
        }
        else {
            QRegularExpression leadingSpaces(R"(^(\s*))");
            QRegularExpressionMatch spaceMatch = leadingSpaces.match(lineText);
            QString indent = spaceMatch.captured(1);
            QString content = lineText.mid(indent.length());
            iterCursor.insertText(indent + "-[] " + content);
        }
    }
    iterCursor.endEditBlock();

    cursor = myTextEdit->textCursor();
    int maxPos = myTextEdit->document()->characterCount() - 1;
    cursor.setPosition(qMin(anchSave, maxPos));
    cursor.setPosition(qMin(save, maxPos), QTextCursor::KeepAnchor);
    myTextEdit->setTextCursor(cursor);
}

void MarkdownEditor::on_actionHeader_triggered()
{
    QTextCursor cursor = myTextEdit->textCursor();
    int save = cursor.position();
    int anchSave = cursor.anchor();
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();

    QTextCursor iterCursor(myTextEdit->document());
    iterCursor.setPosition(start);
    int startBlock = iterCursor.blockNumber();
    iterCursor.setPosition(end);
    int endBlock = iterCursor.blockNumber();

    int added = 0;
    iterCursor.beginEditBlock();
    for (int i = startBlock; i <= endBlock; ++i) {
        QTextBlock block = myTextEdit->document()->findBlockByNumber(i);
        iterCursor.setPosition(block.position());
        iterCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        QString lineText = iterCursor.selectedText();
        int level = 0;
        while (level < lineText.length() && lineText[level] == '#')
            level++;
        QString content = lineText.mid(level);
        if (content.startsWith(" "))
            content = content.mid(1);

        int oldLength = lineText.length();

        if (level >= 6) {
            iterCursor.insertText(content);
        } else {
            QString hashes = QString("#").repeated(level + 1);
            iterCursor.insertText(hashes + " " + content);
        }

        int newLength = iterCursor.block().text().length();
        added += newLength - oldLength;
    }
    iterCursor.endEditBlock();

    cursor = myTextEdit->textCursor();
    int maxPos = myTextEdit->document()->characterCount() - 1;
    cursor.setPosition(qMin(qMax(anchSave + added, 0), maxPos));
    cursor.setPosition(qMin(qMax(save + added, 0), maxPos), QTextCursor::KeepAnchor);
    myTextEdit->setTextCursor(cursor);
}

void MarkdownEditor::on_actionLine_triggered()
{
    QTextCursor cursor = myTextEdit->textCursor();
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    QString lineText = cursor.selectedText();

    cursor.movePosition(QTextCursor::EndOfBlock);

    if (!lineText.trimmed().isEmpty()) {
        cursor.insertBlock();
    }

    cursor.insertBlock();
    cursor.insertText("---");
    cursor.insertBlock();
    cursor.endEditBlock();
    myTextEdit->setTextCursor(cursor);
}

void MarkdownEditor::on_actionQuote_triggered()
{
    QTextCursor cursor = myTextEdit->textCursor();
    int save = cursor.position();
    int anchSave = cursor.anchor();
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();

    QTextCursor iterCursor(myTextEdit->document());
    iterCursor.setPosition(start);
    int startBlock = iterCursor.blockNumber();
    iterCursor.setPosition(end);
    int endBlock = iterCursor.blockNumber();

    int totalAdded = 0;
    iterCursor.beginEditBlock();
    for (int i = startBlock; i <= endBlock; ++i) {
        QTextBlock block = myTextEdit->document()->findBlockByNumber(i);
        iterCursor.setPosition(block.position());
        iterCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        QString lineText = iterCursor.selectedText();

        if (lineText.trimmed().isEmpty())
            continue;

        if (lineText.startsWith("> ")) {
            iterCursor.insertText(lineText.mid(2));
            totalAdded -= 2;
        } else if (lineText.startsWith(">")) {
            iterCursor.insertText(lineText.mid(1));
            totalAdded -= 1;
        } else {
            iterCursor.insertText("> " + lineText);
            totalAdded += 2;
        }
    }
    iterCursor.endEditBlock();

    cursor = myTextEdit->textCursor();
    int maxPos = myTextEdit->document()->characterCount() - 1;
    cursor.setPosition(qMin(qMax(anchSave + totalAdded, 0), maxPos));
    cursor.setPosition(qMin(qMax(save + totalAdded, 0), maxPos), QTextCursor::KeepAnchor);
    myTextEdit->setTextCursor(cursor);
}

void MarkdownEditor::on_actionCodeBlock_triggered()
{
    QTextCursor cursor = myTextEdit->textCursor();
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();

    cursor.beginEditBlock();

    cursor.setPosition(end);
    cursor.movePosition(QTextCursor::EndOfBlock);
    cursor.insertBlock();
    cursor.insertText("```");

    cursor.setPosition(start);
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.insertBlock();
    cursor.movePosition(QTextCursor::PreviousBlock);
    cursor.insertText("```");

    cursor.endEditBlock();

    cursor.movePosition(QTextCursor::EndOfBlock);
    myTextEdit->setTextCursor(cursor);
}

void MarkdownEditor::setEditorText(QString newSource)
{
    QTextCursor cursor = myTextEdit->textCursor();
    int savedPos = cursor.position();

    myTextEdit->blockSignals(true);
    myTextEdit->setPlainText(newSource);
    myTextEdit->blockSignals(false);

    cursor = myTextEdit->textCursor();
    int maxPos = myTextEdit->document()->characterCount() - 1;
    cursor.setPosition(qMin(savedPos, maxPos));
    myTextEdit->setTextCursor(cursor);
}

void MarkdownEditor::on_actionInsertLink_triggered()
{

    QDialog dialog;
    dialog.setWindowTitle("Insert Note Link");
    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QList<QPair<QUuid, QString>> allEntries = registry->getAllUuids();

    QLineEdit *searchEdit = new QLineEdit(&dialog);
    searchEdit->setPlaceholderText("Search for a note...");

    QListWidget *resultsList = new QListWidget(&dialog);
    resultsList->setMaximumHeight(150);
    resultsList->hide();

    QUuid chosenUuid;

    connect(searchEdit, &QLineEdit::textChanged, &dialog,
            [&allEntries, resultsList, &chosenUuid](const QString &text) {
                resultsList->clear();
                chosenUuid = QUuid();
                if (text.trimmed().isEmpty()) {
                    resultsList->hide();
                    return;
                }
                int count = 0;
                for (const auto &entry : allEntries) {
                    if (entry.second.contains(text, Qt::CaseInsensitive)) {
                        QListWidgetItem *item = new QListWidgetItem(entry.second);
                        item->setData(Qt::UserRole, entry.first.toString(QUuid::WithoutBraces));
                        resultsList->addItem(item);
                        if (++count >= 10)
                            break;
                    }
                }
                resultsList->setVisible(count > 0);
            });

    connect(resultsList, &QListWidget::itemClicked, &dialog,
            [searchEdit, resultsList, &chosenUuid](QListWidgetItem *item) {
                chosenUuid = QUuid::fromString(item->data(Qt::UserRole).toString());
                searchEdit->blockSignals(true);
                searchEdit->setText(item->text());
                searchEdit->blockSignals(false);
                resultsList->hide();
            });

    layout->addWidget(new QLabel("Link to note:", &dialog));
    layout->addWidget(searchEdit);
    layout->addWidget(resultsList);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);

    if (dialog.exec() != QDialog::Accepted || chosenUuid.isNull())
        return;

    // Insert [[uuid]] at the current cursor position
    QTextCursor cursor = myTextEdit->textCursor();
    cursor.insertText("[[" + chosenUuid.toString(QUuid::WithoutBraces) + "]]");
    myTextEdit->setTextCursor(cursor);
    myTextEdit->setFocus();
}

