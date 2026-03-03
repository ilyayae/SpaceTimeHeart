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
        myTextView = ui->textView;
        connect(myTextEdit, &HyperlinkTextBrowser::textChanged, this, &MarkdownEditor::hyperlinkTextEdit_textChanged);


        textEditScroll = myTextEdit->verticalScrollBar();
        textViewScroll = myTextView->verticalScrollBar();
        connect(textEditScroll, &QScrollBar::valueChanged, this, &MarkdownEditor::myTextEdit_scrolled);
        connect(textViewScroll, &QScrollBar::valueChanged, this, &MarkdownEditor::myTextView_scrolled);


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

void MarkdownEditor::hyperlinkTextEdit_textChanged()
{
    qDebug() << "Haiii";
    myTextView->setMarkdown(myTextEdit->toPlainText());

    /*
X = % / 100 * (MAX - MIN) + MIN
152.28 = 23 / 100 * (642 - 6) + 6
% = (X - MIN)/(MAX - MIN) * 100
23 = (152.28 - 6)/(642- 6) * 100
     */
}


void MarkdownEditor::myTextEdit_scrolled(int value)
{
    if(syncingScroll) return;
    syncingScroll = true;
    int range = textEditScroll->maximum() - textEditScroll->minimum();
    if (range == 0) return;
    float percent = float(value - textEditScroll->minimum()) / (textEditScroll->maximum() - textEditScroll->minimum());
    int result = percent * (textViewScroll->maximum() - textViewScroll->minimum()) + textViewScroll->minimum();
    textViewScroll->setValue(result);
    syncingScroll = false;
}
void MarkdownEditor::myTextView_scrolled(int value)
{
    if(syncingScroll) return;
    syncingScroll = true;
    int range = textViewScroll->maximum() - textViewScroll->minimum();
    if (range == 0) return;
    float percent = float(value - textViewScroll->minimum()) / (textViewScroll->maximum() - textViewScroll->minimum());
    int result = percent * (textEditScroll->maximum() - textEditScroll->minimum()) + textEditScroll->minimum();
    textEditScroll->setValue(result);
    syncingScroll = false;
}

