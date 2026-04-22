#include "include/editors/texteditor.h"
#include "include/noteTypes/uuidregistry.h"
#include "ui_texteditor.h"
#include <QRegularExpression>
#include <QUuid>

TextEditor::TextEditor(QWidget *parent, UuidRegistry *reg)
    : QMainWindow(parent)
    , ui(new Ui::TextEditor)
{
    ui->setupUi(this);
    registry = reg;
}

TextEditor::~TextEditor()
{
    delete ui;
}

CustomTextBrowser* TextEditor::getQTextEdit()
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
        connect(slider, &QSlider::valueChanged, this, &TextEditor::updateZoom);
        toolbar->addAction(separator);

        myTextEdit = new CustomTextBrowser(ui->centralwidget);
        myTextEdit->setReadOnly(false);
        myTextEdit->registry = registry;
        myFRWidget = new FindReplaceWidget(this, myTextEdit);
        ui->centralwidget->layout()->addWidget(myFRWidget);
        ui->centralwidget->layout()->addWidget(myTextEdit);
        connect(myTextEdit, &CustomTextBrowser::textChanged, this, &TextEditor::hyperlinkTextEdit_textChanged);
        myFRWidget->show(false);
        updateZoom(currZoom);

        updateZoom(startZoom);
        slider->setValue(currZoom);
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
    emit zoomChanged(currZoom);
    myTextEdit->zoomIn(currZoom);
}

QList<QUuid> TextEditor::getUuids()
{
    return myTextEdit->getUuids();
}

void TextEditor::hyperlinkTextEdit_textChanged()
{
    emit Updated();
}

void TextEditor::on_actionInsertLink_triggered()
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

