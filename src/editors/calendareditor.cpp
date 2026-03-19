#include "include/editors/calendareditor.h"
#include "ui_calendareditor.h"

CalendarEditor::CalendarEditor(QWidget *parent, UuidRegistry *registry)
    : QMainWindow(parent)
    , ui(new Ui::CalendarEditor)
{
    ui->setupUi(this);
    myRegistry = registry;
}

CalendarEditor::~CalendarEditor()
{
    delete ui;
}

void CalendarEditor::Initialize(CalendarData data)
{
    myData = data;
    UpdateCalendar(0,0);
}

void CalendarEditor::UpdateCalendar(int year, int month)
{
    currentYear = year;
    currentMonth = month;

    ui->Date->setText(myData.config.months[currentMonth].name + ", year " + QString::number(currentYear));

    QLayoutItem* item;
    while ((item = ui->DayHolder->layout()->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    for(int o = 0; o < myData.config.weekLength; o++)
    {
        QLabel *weekdayLabel = new QLabel(ui->DayHolder);
        QString fullName = myData.config.dayNames[o];
        QString display = fullName.length() > 3 ? fullName.left(3) : fullName;
        weekdayLabel->setText(display);
        weekdayLabel->setToolTip(fullName);
        weekdayLabel->setAlignment(Qt::AlignCenter);
        weekdayLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Maximum);
        weekdayLabel->setMinimumWidth(0);
        weekdayLabel->setWordWrap(false);
        qobject_cast<QGridLayout*>(ui->DayHolder->layout())->addWidget(weekdayLabel, 0, o);
    }

    int firstDayWeekday = myData.config.weekdayOf(myData.config.absoluteDay(currentYear, currentMonth, 1));
    for(int o = 1; o < myData.config.months[currentMonth].dayCount+1; o++)
    {
        QList<double> moonsphases;
        for(int i = 0; i < myData.config.moons.count(); i++)
        {
            double phase = std::fmod((myData.config.absoluteDay(currentYear, currentMonth, o) - myData.config.moons[i].epochDayOffset) / myData.config.moons[i].cycleLengthDays, 1.0);
            moonsphases.append(phase);
        }
        QVector<DayLink> links = myData.linksForDay(currentYear, currentMonth, o);
        DaySlot *day = new DaySlot(ui->DayHolder, o, myData.config.weekdayOf(myData.config.absoluteDay(currentYear, currentMonth, o)), &links, &moonsphases);
        int col = myData.config.weekdayOf(myData.config.absoluteDay(currentYear, currentMonth, o));
        int adjustedIndex = (o - 1) + firstDayWeekday;
        int row = adjustedIndex / myData.config.weekLength + 1;
        qobject_cast<QGridLayout*>(ui->DayHolder->layout())->addWidget(day, row, col);
        connect(day, &DaySlot::clicked, this, &CalendarEditor::SelectDay);
    }
}

void CalendarEditor::on_Date_clicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Go to date");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QComboBox *monthCombo = new QComboBox(&dialog);
    for (int i = 0; i < myData.config.months.size(); i++)
        monthCombo->addItem(myData.config.months[i].name, i);
    monthCombo->setCurrentIndex(currentMonth);

    QSpinBox *yearSpin = new QSpinBox(&dialog);
    yearSpin->setRange(-999999, 999999);
    yearSpin->setValue(currentYear);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    layout->addWidget(new QLabel("Month:", &dialog));
    layout->addWidget(monthCombo);
    layout->addWidget(new QLabel("Year:", &dialog));
    layout->addWidget(yearSpin);
    layout->addWidget(buttons);

    if (dialog.exec() == QDialog::Accepted) {
        UpdateCalendar(yearSpin->value(), monthCombo->currentIndex());
    }
}

void CalendarEditor::SelectDay(int day)
{
    QLayoutItem* item;
    bool first = true;
    while ((item = ui->DayLinksHolder->layout()->takeAt(0)) != nullptr) {
        if (item->widget()) {
            if(first)
            {
                first = false;
            }
            else
            {
                item->widget()->deleteLater();
            }
        }
        delete item;
    }
    selectedDay = day;

    QVector<DayLink> links = myData.linksForDay(currentYear, currentMonth, day);
    for(int i = 0; i < links.count(); i++)
    {
        DayLink link = links[i];
        LinkInDay *linkInDay = new LinkInDay(ui->DayLinksHolder, &link);
        ui->DayLinksHolder->layout()->addWidget(linkInDay);
    }

    qDebug() << "Selected Day " + QString::number(selectedDay);
}

void CalendarEditor::on_NextMonth_clicked()
{
    if(currentMonth+1 >= myData.config.monthCount())
    {
        UpdateCalendar(currentYear+1, 0);
    }
    else
    {
        UpdateCalendar(currentYear, currentMonth+1);
    }
}

void CalendarEditor::on_NextYear_clicked()
{
    UpdateCalendar(currentYear+1, currentMonth);
}

void CalendarEditor::on_PrevYear_clicked()
{
    if(currentYear-1 >= -999999)
    {
        UpdateCalendar(currentYear-1, currentMonth);
    }
}

void CalendarEditor::on_PrevMonth_clicked()
{
    if(currentMonth-1 < 0)
    {
        if(currentYear-1 >= -999999)
        {
            UpdateCalendar(currentYear-1, myData.config.monthCount()-1);
        }
    }
    else
    {
        UpdateCalendar(currentYear, currentMonth-1);
    }
}

void CalendarEditor::on_SpecificLink_clicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Create New Link");
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QLineEdit *descEdit = new QLineEdit(&dialog);
    layout->addWidget(new QLabel("Description:", &dialog));
    layout->addWidget(descEdit);
    QColor chosenColor("#5599DD");
    QPushButton *colorPick = new QPushButton(&dialog);
    colorPick->setStyleSheet("background-color: " + chosenColor.name() + ";");
    colorPick->setFixedHeight(24);
    connect(colorPick, &QPushButton::clicked, this, [&chosenColor, colorPick]() {
        QColor c = QColorDialog::getColor(chosenColor, colorPick, "Pick a color");
        if (c.isValid()) {
            chosenColor = c;
            colorPick->setStyleSheet("background-color: " + c.name() + ";");
        }
    });
    layout->addWidget(new QLabel("Color:", &dialog));
    layout->addWidget(colorPick);

    // Fetch all entries once from registry
    QList<QPair<QUuid, QString>> allEntries = myRegistry->getAllUuids();
    QLineEdit *searchEdit = new QLineEdit(&dialog);
    searchEdit->setPlaceholderText("Search for a note...");
    QListWidget *resultsList = new QListWidget(&dialog);
    resultsList->setMaximumHeight(150);
    resultsList->hide();
    QUuid chosenUuid;

    connect(searchEdit, &QLineEdit::textChanged, this,
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

    connect(resultsList, &QListWidget::itemClicked, this,
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

    if (dialog.exec() == QDialog::Accepted) {
        DayLink link;
        link.displayLabel  = descEdit->text();
        link.colorHex      = chosenColor.name();
        link.targetNoteId  = chosenUuid.toString(QUuid::WithoutBraces);
        myData.addSpecificLink(currentYear, currentMonth, selectedDay, link);
        SelectDay(selectedDay);
    }
}

void CalendarEditor::on_YearlyLink_clicked()
{

}

