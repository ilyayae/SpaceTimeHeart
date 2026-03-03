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
        currZoom = 0;
        slider->setMinimum(-5);
        slider->setMaximum(10);
        slider->setMaximumWidth(200);
        connect(slider, &QSlider::valueChanged, this, &MarkdownEditor::updateZoom);
        toolbar->addAction(separator);


        myTextEdit = new HyperlinkTextBrowser(ui->centralwidget);

        myFRWidget = new FindReplaceWidget(this, myTextEdit);
        QLayout *l = ui->centralwidget->layout();
        QHBoxLayout *hbox = (QHBoxLayout*)l;
        hbox->insertWidget(0, myFRWidget);
        ui->HolderOfText->layout()->addWidget(myTextEdit);
        myFRWidget->show(false);
    }
    return myTextEdit;
}

void MarkdownEditor::updateZoom(int zoom)
{
    myTextEdit->zoomOut(currZoom);
    currZoom = zoom;
    myTextEdit->zoomIn(currZoom);
}


void MarkdownEditor::on_actionFind_toggled(bool checked)
{
    myFRWidget->show(checked);
    myTextEdit->highlightText(checked, "");
}
