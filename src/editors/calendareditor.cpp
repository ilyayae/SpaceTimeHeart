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

void CalendarEditor::Initialize(CalendarData *data)
{
    myData = data;
    UpdateCalendar(myData->lastViewedYear, myData->lastViewedMonth);
    ui->DayLinksHolder->layout()->setAlignment(Qt::AlignLeft);
}

void CalendarEditor::UpdateCalendar(int year, int month)
{
    currentYear = year;
    currentMonth = month;
    selectedDay = nullptr;
    myData->lastViewedMonth = currentMonth;
    myData->lastViewedYear = currentYear;

    ui->Date->setText(myData->config.months[currentMonth].name + ", year " + QString::number(currentYear));

    QLayoutItem* item;
    while ((item = ui->DayHolder->layout()->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    for(int o = 0; o < myData->config.weekLength; o++)
    {
        QLabel *weekdayLabel = new QLabel(ui->DayHolder);
        QString fullName = myData->config.dayNames[o];
        QString display = fullName.length() > 3 ? fullName.left(3) : fullName;
        weekdayLabel->setText(display);
        weekdayLabel->setToolTip(fullName);
        weekdayLabel->setAlignment(Qt::AlignCenter);
        weekdayLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Maximum);
        weekdayLabel->setMinimumWidth(0);
        weekdayLabel->setWordWrap(false);
        qobject_cast<QGridLayout*>(ui->DayHolder->layout())->addWidget(weekdayLabel, 0, o);
    }

    int firstDayWeekday = myData->config.weekdayOf(myData->config.absoluteDay(currentYear, currentMonth, 1));
    for(int o = 1; o < myData->config.months[currentMonth].dayCount+1; o++)
    {
        QList<double> moonsphases;
        QList<QString> mooncolors;
        for(int i = 0; i < myData->config.moons.count(); i++)
        {
            double phase = std::fmod((myData->config.absoluteDay(currentYear, currentMonth, o) - myData->config.moons[i].epochDayOffset) / myData->config.moons[i].cycleLengthDays, 1.0);
            moonsphases.append(phase);
            mooncolors.append(myData->config.moons[i].color);
        }
        DaySlot *day = new DaySlot(ui->DayHolder, o, myData->config.weekdayOf(myData->config.absoluteDay(currentYear, currentMonth, o)), myData->linksForDay(currentYear, currentMonth, o).count(), &moonsphases, &mooncolors);
        int col = myData->config.weekdayOf(myData->config.absoluteDay(currentYear, currentMonth, o));
        int adjustedIndex = (o - 1) + firstDayWeekday;
        int row = adjustedIndex / myData->config.weekLength + 1;
        qobject_cast<QGridLayout*>(ui->DayHolder->layout())->addWidget(day, row, col);
        connect(day, &DaySlot::clicked, this, &CalendarEditor::SelectDay);
    }

    QLayout *layout = ui->DayLinksHolder->layout();
    while (layout->count() > 1) {
        QLayoutItem *item = layout->takeAt(layout->count() - 1);
        if (item->widget())
            item->widget()->deleteLater();
        delete item;
    }
    ui->LinkFrame->setVisible(false);
    for(int i = 0; i < myData->config.months[currentMonth].dayCount; i++)
    {
        QVector<DayLink> links = myData->linksForDay(currentYear, currentMonth, i);
        for (int i = 0; i < links.count(); i++)
        {
            DayLink link = links[i];
            QFileInfo info(myRegistry->getPath(QUuid::fromString(link.targetNoteId)));
            LinkInDay *linkInDay = new LinkInDay(ui->DayLinksHolder, link, info.baseName(), false);
            layout->addWidget(linkInDay);
            connect(linkInDay, &LinkInDay::GoToNote, this, &CalendarEditor::emitUuid);
            connect(linkInDay, &LinkInDay::DestroyLink, this, &CalendarEditor::DestroyLink);
        }
    }
}

void CalendarEditor::on_Date_clicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Go to date");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QComboBox *monthCombo = new QComboBox(&dialog);
    for (int i = 0; i < myData->config.months.size(); i++)
        monthCombo->addItem(myData->config.months[i].name, i);
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

void CalendarEditor::SelectDay(DaySlot *day)
{
    ui->Scroll->show();
    ui->LinkFrame->setVisible(true);
    QLayout *layout = ui->DayLinksHolder->layout();
    while (layout->count() > 1) {
        QLayoutItem *item = layout->takeAt(layout->count() - 1);
        if (item->widget())
            item->widget()->deleteLater();
        delete item;
    }

    if(selectedDay != nullptr)
        selectedDay->ColorMe("#FFF0F0");

    selectedDay = day;

    selectedDay->ColorMe("#ffe6e6");

    QVector<DayLink> links = myData->linksForDay(currentYear, currentMonth, day->dayNumber);
    for (int i = 0; i < links.count(); i++)
    {
        DayLink link = links[i];
        QFileInfo info(myRegistry->getPath(QUuid::fromString(link.targetNoteId)));
        LinkInDay *linkInDay = new LinkInDay(ui->DayLinksHolder, link, info.baseName());
        layout->addWidget(linkInDay);
        connect(linkInDay, &LinkInDay::GoToNote, this, &CalendarEditor::emitUuid);
        connect(linkInDay, &LinkInDay::DestroyLink, this, &CalendarEditor::DestroyLink);
    }
}

void CalendarEditor::DestroyLink(DayLink link)
{
    if(selectedDay != nullptr)
    {
        QMapIterator<RecurringDateKey, QVector<DayLink>> iteratorRec(myData->recurringEvents);
        while(iteratorRec.hasNext())
        {
            iteratorRec.next();
            QList<DayLink> linksForKey = myData->recurringEvents[iteratorRec.key()];
            for(int i = linksForKey.size() - 1; i >= 0; i--)
            {
                if(linksForKey[i].colorHex == link.colorHex && linksForKey[i].displayLabel == link.displayLabel && linksForKey[i].targetNoteId == link.targetNoteId) linksForKey.remove(i);
            }
            myData->recurringEvents.remove(iteratorRec.key());
            for(int i = 0; i < linksForKey.count(); i++)
            {
                myData->recurringEvents.insert(iteratorRec.key(), linksForKey);
            }
        }

        QMapIterator<SpecificDateKey, QVector<DayLink>> iteratorSpe(myData->specificEvents);
        while(iteratorSpe.hasNext())
        {
            iteratorSpe.next();
            QList<DayLink> linksForKey = myData->specificEvents[iteratorSpe.key()];
            for(int i = linksForKey.size() - 1; i >= 0; i--)
            {
                if(linksForKey[i].colorHex == link.colorHex && linksForKey[i].displayLabel == link.displayLabel && linksForKey[i].targetNoteId == link.targetNoteId) linksForKey.remove(i);
            }
            myData->specificEvents.remove(iteratorSpe.key());
            for(int i = 0; i < linksForKey.count(); i++)
            {
                myData->specificEvents.insert(iteratorSpe.key(), linksForKey);
            }
        }

        SelectDay(selectedDay);
        selectedDay->thisDaysLinks = myData->linksForDay(currentYear, currentMonth, selectedDay->dayNumber).count();
        selectedDay->UpdateEventTracker();
        emit Updated();
    }
}

void CalendarEditor::on_NextMonth_clicked()
{
    if(currentMonth+1 >= myData->config.monthCount())
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
            UpdateCalendar(currentYear-1, myData->config.monthCount()-1);
        }
    }
    else
    {
        UpdateCalendar(currentYear, currentMonth-1);
    }
}

void CalendarEditor::on_SpecificLink_clicked()
{
    if(selectedDay != nullptr)
    {
        DayLink link = CreateDayLink();
        if(link.targetNoteId.isEmpty())
            return;
        myData->addSpecificLink(currentYear, currentMonth, selectedDay->dayNumber, link);
        selectedDay->thisDaysLinks++;
        SelectDay(selectedDay);
        selectedDay->UpdateEventTracker();
        emit Updated();
    }
}

void CalendarEditor::on_YearlyLink_clicked()
{
    if(selectedDay != nullptr)
    {
        DayLink link = CreateDayLink();
        if(link.targetNoteId.isEmpty())
            return;
        myData->addRecurringLink(currentMonth, selectedDay->dayNumber, link);
        selectedDay->thisDaysLinks++;
        SelectDay(selectedDay);
        selectedDay->UpdateEventTracker();
        emit Updated();
    }
}

DayLink CalendarEditor::CreateDayLink()
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
    connect(colorPick, &QPushButton::clicked, this, [&chosenColor, colorPick, &dialog]() {
        QColor c = QColorDialog::getColor(chosenColor, &dialog, "Pick a color");
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

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);

    if (dialog.exec() == QDialog::Accepted) {
        DayLink link;
        link.displayLabel = descEdit->text();
        link.colorHex = chosenColor.name();
        link.targetNoteId = chosenUuid.toString(QUuid::WithoutBraces);
        return link;
    }

    return DayLink();
}

void CalendarEditor::emitUuid(QString uuid)
{
    emit uuidClicked(QUuid::fromString(uuid));
}
