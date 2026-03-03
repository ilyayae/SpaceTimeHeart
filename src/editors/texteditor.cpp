#include "include/editors/texteditor.h"
#include "ui_texteditor.h"
#include <QRegularExpression>
#include <QUuid>

TextEditor::TextEditor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TextEditor)
{
    ui->setupUi(this);
}

TextEditor::~TextEditor()
{
    delete ui;
}

QTextBrowser* TextEditor::getQTextEdit()
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
        currZoom = 0;
        slider->setMinimum(-5);
        slider->setMaximum(10);
        slider->setMaximumWidth(200);
        connect(slider, &QSlider::valueChanged, this, &TextEditor::updateZoom);
        toolbar->addAction(separator);


        myTextEdit = new HyperlinkTextBrowser(ui->centralwidget);

        myFRWidget = new FindReplaceWidget(this, myTextEdit);
        ui->centralwidget->layout()->addWidget(myFRWidget);
        ui->centralwidget->layout()->addWidget(myTextEdit);
        connect(myTextEdit, &HyperlinkTextBrowser::textChanged, this, &TextEditor::hyperlinkTextEdit_textChanged);
        myFRWidget->show(false);
    }
    return myTextEdit;
}

void TextEditor::on_actionUndo_triggered()
{
    myTextEdit->undo();
}


void TextEditor::on_actionRedo_triggered()
{
    myTextEdit->redo();
}


void TextEditor::on_actionSave_triggered()
{
    emit saveButton();
}


void TextEditor::on_actionSaveAs_triggered()
{
    emit saveAsButton();
}


void TextEditor::on_actionCut_triggered()
{
    myTextEdit->cut();
}


void TextEditor::on_actionCopy_triggered()
{
    myTextEdit->copy();
}


void TextEditor::on_actionPaste_triggered()
{
    myTextEdit->paste();
}

void TextEditor::on_actionFind_toggled(bool checked)
{
    myFRWidget->show(checked);
    myTextEdit->highlightText(checked, "");
}

void TextEditor::updateZoom(int zoom)
{
    myTextEdit->zoomOut(currZoom);
    currZoom = zoom;
    myTextEdit->zoomIn(currZoom);
}


void TextEditor::hyperlinkTextEdit_textChanged()
{
}

