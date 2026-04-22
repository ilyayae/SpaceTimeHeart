#include "include/subclasses/customtextbrowser.h"
#include <QMouseEvent>
#include <QDesktopServices>

CustomTextBrowser::CustomTextBrowser(QWidget *parent)
    : QTextBrowser(parent)
{
    setUndoRedoEnabled(true);
    setOpenExternalLinks(true);
    uuidFindRegex.setPattern("\\[\\[([a-fA-F0-9\\-]{36})\\]\\]");
    linkFindRegex.setPattern("(http|ftp|https):\/\/([a-zA-Z0-9\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF_-]+(?:(?:\.[a-zA-Z0-9\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF_-]+)+))([a-zA-Z0-9\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF.,@?^=%&:\/~+#-]*[a-zA-Z0-9\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF@?^=%&\/~+#-])");
    highlighter = new SearchHighlighter(document());
}

void CustomTextBrowser::setMarkdown(const QString &markdown)
{

    originalMarkdown = markdown;
    QString processed = preprocessMarkdown(markdown);
    QTextBrowser::setMarkdown(processed);
    applyUnderline();
}

QString CustomTextBrowser::preprocessMarkdown(const QString &text)
{
    QStringList lines = text.split("\n");

    for (int i = 0; i < lines.size(); i++) {
        lines[i].replace(QRegularExpression(R"(-\s?\[x\])"), QString::fromUtf8("- ☑"));
        lines[i].replace(QRegularExpression(R"(-\s?\[\s?\])"), QString::fromUtf8("- ☐"));
        lines[i] = lines[i].trimmed() + "  ";
    }

    return lines.join("\n");
}

void CustomTextBrowser::paintEvent(QPaintEvent *event)
{
    QTextBrowser::paintEvent(event);

    QPainter painter(viewport());
    QRegularExpression uuidFindRegex;
    uuidFindRegex.setPattern("\\[\\[([a-fA-F0-9\\-]{36})\\]\\]");

    QRegularExpressionMatchIterator it = uuidFindRegex.globalMatch(toPlainText());
    while (it.hasNext())
    {
        QRegularExpressionMatch match = it.next();
        QTextCursor startCursor(document());
        startCursor.setPosition(match.capturedStart());
        QTextCursor endCursor(document());
        endCursor.setPosition(match.capturedEnd());
        QRect startRect = cursorRect(startCursor);
        QRect endRect = cursorRect(endCursor);
        QRect rect = startRect;
        rect.setRight(endRect.left());

        QString replace;
        QString m = match.captured();
        QString uuidString = m.mid(2, m.length() - 4);
        QUuid uuidCandidate = QUuid(uuidString);
        if(uuidCandidate.isNull())
        {
            replace = "Null link!";
        }
        else
        {
            QFileInfo info(registry->getPath(uuidCandidate));
            if(info.baseName().isEmpty())
            {
                replace = "Broken link! Points to non-registered file!";
            }
            else
            {
                replace = info.baseName();
            }
        }

        painter.setPen(QColor(230,230,230));
        painter.setBrush(QColor(230,230,230));
        painter.drawRect(rect);
        painter.setPen(Qt::black);
        painter.setFont(font());
        painter.drawText(rect, Qt::AlignHCenter | Qt::AlignVCenter, replace);

        //
    }
}

void CustomTextBrowser::applyUnderline()
{
    QTextDocument *doc = document();
    QTextCursor cursor(doc);

    while (true) {
        cursor = doc->find("++", cursor);
        if (cursor.isNull())
            break;
        int startPos = cursor.selectionStart();
        cursor.removeSelectedText();
        QTextCursor closeCursor = doc->find("++", cursor);
        if (closeCursor.isNull())
            break;
        int endPos = closeCursor.selectionStart();
        closeCursor.removeSelectedText();
        cursor.setPosition(startPos);
        cursor.setPosition(endPos, QTextCursor::KeepAnchor);
        QTextCharFormat fmt = cursor.charFormat();
        fmt.setFontUnderline(true);
        cursor.mergeCharFormat(fmt);
        cursor.setPosition(startPos + (endPos - startPos));
    }
}

void CustomTextBrowser::mousePressEvent(QMouseEvent *event)
{

    QTextCursor cursor = cursorForPosition(event->pos());
    QTextBlock block = cursor.block();
    QString blockText = block.text();

    if (blockText.contains(QChar(0x2610)) || blockText.contains(QChar(0x2611))) {
        int clickedIndex = 0;
        QTextBlock b = document()->begin();
        while (b.isValid() && b != block) {
            if (b.text().contains(QChar(0x2610)) || b.text().contains(QChar(0x2611)))
                clickedIndex++;
            b = b.next();
        }

        QStringList sourceLines = originalMarkdown.split("\n");
        int checkboxCount = 0;
        for (int i = 0; i < sourceLines.size(); i++) {
            if (sourceLines[i].contains("-[x]") || sourceLines[i].contains("-[]")) {
                if (checkboxCount == clickedIndex) {
                    if (sourceLines[i].contains("-[x]")) {
                        sourceLines[i].replace("-[x]", "-[]");
                    } else {
                        sourceLines[i].replace("-[]", "-[x]");
                    }

                    originalMarkdown = sourceLines.join("\n");
                    emit mdCheckboxChanged(originalMarkdown);
                    setMarkdown(originalMarkdown);
                    return;
                }
                checkboxCount++;
            }
        }
    }
    if (event->modifiers() & Qt::ControlModifier) {
        QString clicked = getClickedHyperlink(event->pos());
        if(!clicked.isEmpty())
        {
            if(clicked.size() == 40)
            {
                QString uuidString = clicked.mid(2, clicked.length() - 4);
                QUuid uuidCandidate = QUuid(uuidString);
                if(!uuidCandidate.isNull())
                {
                    emit uuidClicked(uuidCandidate);
                    return;
                }
            }
            QUrl url = QUrl(clicked);
            QDesktopServices::openUrl(url);
        }
    }
    QTextBrowser::mousePressEvent(event);
}

void CustomTextBrowser::mouseMoveEvent(QMouseEvent *event)
{
    QString hovered = getClickedHyperlink(event->pos());
    if (!hovered.isEmpty() && (event->modifiers() & Qt::ControlModifier)) {
        viewport()->setCursor(Qt::PointingHandCursor);
    } else {
        viewport()->setCursor(Qt::IBeamCursor);
    }
    QTextBrowser::mouseMoveEvent(event);
}

QString CustomTextBrowser::getClickedHyperlink(const QPoint &pos)
{
    QTextCursor cursor = cursorForPosition(pos);
    int cursorPos = cursor.position();
    QString text = toPlainText();
        QRegularExpressionMatchIterator one = uuidFindRegex.globalMatch(text);
        while (one.hasNext())
        {
            QRegularExpressionMatch match = one.next();
            int start = match.capturedStart();
            int end = match.capturedEnd();
            if (cursorPos >= start && cursorPos <= end)
            {
                return text.mid(start, end - start);
            }
        }
    QRegularExpressionMatchIterator two = linkFindRegex.globalMatch(text);
    while (two.hasNext())
    {
        QRegularExpressionMatch match = two.next();
        int start = match.capturedStart();
        int end = match.capturedEnd();
        if (cursorPos >= start && cursorPos <= end)
        {
            return text.mid(start, end - start);
        }
    }
    return "";
}

void CustomTextBrowser::highlightText(bool b, QString find)
{
    QTextCursor cursor = textCursor();
    if(b)
        highlighter->setSearchString(find);
    else
        highlighter->setSearchString("");
}

void CustomTextBrowser::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
    {
        //Check if the current line has list markers
        //If the current line does, add them to the beggining of the new line
        //Line markers can be almost infinite and in various combinations
        //Ensure that ' - ', ' .1 ' and ' -[] ' are all supported
        QTextCursor cursor = this->textCursor();
        cursor.beginEditBlock();
        int cursorPos = cursor.position();
        cursor.movePosition(QTextCursor::StartOfBlock);
        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        QString lineText = cursor.selectedText();
        QRegularExpression prefixRegex(R"(^((?:   )*(-\[x\]\s?|-\[\]\s?|-\s?|\d+\.\s?)))");
        QRegularExpressionMatch match = prefixRegex.match(lineText + " ");

        QString foundMarkers = "";

        if (match.hasMatch())
        {
            foundMarkers = match.captured(1);
            QString contentAfterPrefix = lineText.mid(foundMarkers.length()).trimmed();
            if (contentAfterPrefix.isEmpty())
            {
                cursor.removeSelectedText();
                cursor.endEditBlock();
                this->setTextCursor(cursor);
                return;
            }

            foundMarkers.replace("-[x]", "-[]");

            QRegularExpression numberRegex(R"((\d+)\.\s?)");
            QRegularExpressionMatch numberMatch = numberRegex.match(foundMarkers);
            if (numberMatch.hasMatch()) {
                int number = numberMatch.captured(1).toInt();
                number++;
                foundMarkers.replace(numberRegex, QString::number(number) + ". ");
            }
        }
        cursor.setPosition(cursorPos);
        cursor.insertText("\n" + foundMarkers);
        cursor.endEditBlock();
    }
    else if(event->key() == Qt::Key_Tab)
    {
        QTextCursor cursor = this->textCursor();
        cursor.beginEditBlock();
        int cursorPos = cursor.position();
        cursor.movePosition(QTextCursor::StartOfBlock);
        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        QString lineText = cursor.selectedText();
        QRegularExpression numberRegex(R"((\d+)\.\s?)");
        QRegularExpressionMatch numberMatch = numberRegex.match(lineText);
        if (numberMatch.hasMatch()) {
            lineText.replace(numberMatch.capturedStart(1), numberMatch.capturedLength(1), "1");
        }
        lineText.replace("-[x]", "-[]");
        cursor.insertText(lineText);
        cursor.movePosition(QTextCursor::StartOfBlock);
        cursor.insertText("   ");
        cursor.setPosition(cursorPos + 3);
        cursor.endEditBlock();
        this->setTextCursor(cursor);
    }
    else
    {
        QTextBrowser::keyPressEvent(event);
    }
    return;
}

QList<QUuid> CustomTextBrowser::getUuids()
{
    QList<QUuid> result;
        QRegularExpressionMatchIterator it = uuidFindRegex.globalMatch(originalText);
        while (it.hasNext())
        {
            QRegularExpressionMatch match = it.next();
            QUuid uuid = QUuid::fromString(match.captured(1));
            if (!uuid.isNull())
            {
                result.append(uuid);
            }
        }

    return result;
}
