#include "include/subclasses/findreplacewidget.h"

FindReplaceWidget::FindReplaceWidget(QWidget *parent, QTextBrowser *browser)
    : QWidget{parent}
    , ui(new Ui::FindReplaceWidget)
    , myTextEdit(browser)
{
    ui->setupUi(this);
}
FindReplaceWidget::~FindReplaceWidget()
{
    delete ui;
}
//Function to show/hide the widget
//Functions to find necessary strings by passing down the textedit
//currnt implementation of next/previous buttons might be very expensive to compute, please rewrite it soon

void FindReplaceWidget::show(bool b)
{
    setVisible(b);
    if(b)
    {
        ui->FindLineEdit->setText("");
        ui->ReplaceLineEdit->setText("");

    }
}

QList<int> FindReplaceWidget::findText()
{
    QString search = ui->FindLineEdit->text();
    QList<int> positions;
    QScrollBar *scroll = myTextEdit->verticalScrollBar();
    int savedScroll = scroll->value();
    scroll->blockSignals(true);
    myTextEdit->moveCursor(QTextCursor::Start);
    while (myTextEdit->find(search))
    {
        QTextCursor cursor = myTextEdit->textCursor();
        positions.append(cursor.selectionStart());
    }
    scroll->setValue(savedScroll);
    scroll->blockSignals(false);

    return positions;
}

void FindReplaceWidget::selectText()
{
    if (textFoundPositions.empty()) return;
    if (currentFoundPosition >= textFoundPositions.count())
        currentFoundPosition = textFoundPositions.count() - 1;
    QTextCursor cursor = myTextEdit->textCursor();
    cursor.setPosition(textFoundPositions[currentFoundPosition]);
    cursor.setPosition(textFoundPositions[currentFoundPosition] + currentFoundWordLength, QTextCursor::KeepAnchor);
    myTextEdit->setTextCursor(cursor);
    myTextEdit->ensureCursorVisible();
    QTimer::singleShot(0, this, [this]() {
        emit findReplaceChanged();
    });
    ui->CurrentFoundLabel->setText(
        QString::number(currentFoundPosition + 1) + "/" +
        QString::number(textFoundPositions.count()));
}

void FindReplaceWidget::on_FindButton_clicked()
{
    QList<int> positions = findText();
    if (!positions.empty())
    {
        textFoundPositions = positions;
        currentFoundPosition = 0;
        currentFoundWordLength = ui->FindLineEdit->text().length();
        CustomTextBrowser* hyper = (CustomTextBrowser*)myTextEdit;
        hyper->highlightText(true, ui->FindLineEdit->text());
        selectText();
        isActivelySearchingWords = true;
    }
}

void FindReplaceWidget::on_ReplaceButton_clicked()
{
    if (!ui->ReplaceLineEdit->text().isEmpty() && textFoundPositions.count() != 0)
    {
        int prevcount = textFoundPositions.count();
        QString txtrep = ui->ReplaceLineEdit->text();
        selectText();
        myTextEdit->insertPlainText(txtrep);
        textFoundPositions = findText();
        if(prevcount == textFoundPositions.count())
        {
            on_FindNextButton_clicked();
        }
        else
        {
            selectText();
        }
    }
}

void FindReplaceWidget::on_FindPrevButton_clicked()
{
    if(isActivelySearchingWords)
    {
        if(textFoundPositions.count() > findText().count())
        {
            textFoundPositions = findText();
            selectText();
        }
        else
        {
            textFoundPositions = findText();
            if(currentFoundPosition > textFoundPositions.count()-1)
            {
                currentFoundPosition = textFoundPositions.count()-1;
                selectText();
            }
            else if(currentFoundPosition > 0)
            {
                currentFoundPosition--;
                selectText();
            }
            else
            {
                currentFoundPosition = textFoundPositions.count()-1;
                selectText();
            }
        }
    }
}

void FindReplaceWidget::on_FindNextButton_clicked()
{
    if(isActivelySearchingWords)
    {
        if(textFoundPositions.count() > findText().count())
        {
            textFoundPositions = findText();
            selectText();
        }
        else
        {
            textFoundPositions = findText();
            if(currentFoundPosition >= textFoundPositions.count()-1)
            {
                currentFoundPosition = 0;
                selectText();
            }
            else if(currentFoundPosition < textFoundPositions.count()-1)
            {
                currentFoundPosition++;
                selectText();
            }
            else if(currentFoundPosition < 0)
            {
                currentFoundPosition = 0;
                selectText();
            }
        }
    }
}
