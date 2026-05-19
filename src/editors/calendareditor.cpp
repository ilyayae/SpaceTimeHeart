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

int getRandomForMonth(int year, int month, unsigned int M) {
    unsigned int N = (year * 100) + month;
    unsigned int x = N;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return (x % M);
}

int getRandomForYear(int year, unsigned int M) {
    unsigned int N = year;
    unsigned int x = N;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return (x % M);
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
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    const int monthCount = myData->config.months.count();
    const int baseDaysInThisMonth = myData->config.months[month].dayCount;
    int firstDayAbs = myData->config.absoluteDay(currentYear, currentMonth, 1);
    int firstDayWeekday = myData->config.weekdayOf(currentYear, currentMonth, 1);

    QMultiMap<int, const LeapDayDefinition*> leapDaysMap;

    for(const auto& leapDay : myData->config.leapDays) {
        if(myData->config.isLeapYear(year, leapDay)) {
            if(leapDay.targetsMonth == month || (leapDay.pickRandomMonth && getRandomForYear(year, monthCount) == month)) {
                int dayID = baseDaysInThisMonth;
                if(leapDay.pickRandomDay) {
                    dayID = getRandomForMonth(year, month, baseDaysInThisMonth);
                }
                leapDaysMap.insert(dayID, &leapDay);
            }
        }
    }

    struct RenderCell {
        int displayDayNumber;
        int absDay;
        bool isLeapDay;
        QString leapHex;
        bool ignoresWeekdays;
        int standardWeekday;
        int gridRow;
        int gridCol;
        int leapColumnIndex;
        const LeapDayDefinition* leapDef;
    };

    QList<RenderCell> simulatedCells;

    QMap<int, int> maxExtraLeapColumnsAfter;

    int currentDayAbs = firstDayAbs;
    int currentWeekday = firstDayWeekday;
    int currentRow = 1;
    int displayDayNumber = 1;

    for (int baseDay = 1; baseDay <= baseDaysInThisMonth; baseDay++) {
        RenderCell normalCell = { displayDayNumber++, currentDayAbs++, false, "", false, currentWeekday, currentRow, 0, 0, nullptr };
        simulatedCells.append(normalCell);

        int dayJustProcessed = currentWeekday;
        currentWeekday++;
        if (currentWeekday >= myData->config.weekLength) {
            currentWeekday = 0;
            currentRow++;
        }
        int leapCountForThisDay = 0;

        auto it = leapDaysMap.find(baseDay);
        while (it != leapDaysMap.end() && it.key() == baseDay) {
            const LeapDayDefinition* leapDef = it.value();

            for(int k = 0; k < leapDef->daysAdded; k++) {
                RenderCell leapCell;
                leapCell.displayDayNumber = displayDayNumber++;
                leapCell.absDay = currentDayAbs++;
                leapCell.isLeapDay = true;
                leapCell.leapHex = "#89CFF0";
                leapCell.ignoresWeekdays = !leapDef->affectsWeekDays;
                leapCell.leapColumnIndex = 0;
                leapCell.leapDef = leapDef;
                if (leapCell.ignoresWeekdays) {
                    leapCell.gridRow = normalCell.gridRow;
                    leapCell.standardWeekday = dayJustProcessed;
                    leapCell.leapColumnIndex = leapCountForThisDay;
                    leapCountForThisDay++;
                    if (leapCountForThisDay > maxExtraLeapColumnsAfter.value(dayJustProcessed, 0)) {
                        maxExtraLeapColumnsAfter[dayJustProcessed] = leapCountForThisDay;
                    }
                } else {
                    leapCell.gridRow = currentRow;
                    leapCell.standardWeekday = currentWeekday;

                    currentWeekday++;
                    if (currentWeekday >= myData->config.weekLength) {
                        currentWeekday = 0;
                        currentRow++;
                    }
                }
                simulatedCells.append(leapCell);
            }
            it++;
        }
    }
    QMap<int, int> standardToGridCol;
    QMap<int, QList<int>> leapFollowToGridCol;
    QList<QString> headerNames;

    int currentColIndex = 0;
    for (int i = 0; i < myData->config.weekLength; i++) {
        standardToGridCol[i] = currentColIndex++;
        headerNames.append(myData->config.dayNames[i]);

        int extraCols = maxExtraLeapColumnsAfter.value(i, 0);
        for(int j = 0; j < extraCols; j++) {
            leapFollowToGridCol[i].append(currentColIndex++);
            headerNames.append("Leap");
        }
    }

    for(int i = 0; i < headerNames.count(); i++) {
        QLabel *weekdayLabel = new QLabel(ui->DayHolder);
        QString fullName = headerNames[i];
        QString display = fullName.length() > 3 ? fullName.left(3) : fullName;
        weekdayLabel->setText(display);
        weekdayLabel->setToolTip(fullName);
        weekdayLabel->setAlignment(Qt::AlignCenter);
        weekdayLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Maximum);
        weekdayLabel->setMinimumWidth(0);
        weekdayLabel->setWordWrap(false);
        qobject_cast<QGridLayout*>(ui->DayHolder->layout())->addWidget(weekdayLabel, 0, i);
    }
    for (RenderCell& cell : simulatedCells) {
        if (cell.isLeapDay && cell.ignoresWeekdays) {
            cell.gridCol = leapFollowToGridCol[cell.standardWeekday][cell.leapColumnIndex];
        } else {
            cell.gridCol = standardToGridCol[cell.standardWeekday];
        }

        QList<double> moonsphases;
        QList<QString> mooncolors;
        for(int i = 0; i < myData->config.moons.count(); i++) {
            double phase = std::fmod((cell.absDay - myData->config.moons[i].epochDayOffset) / myData->config.moons[i].cycleLengthDays, 1.0);
            moonsphases.append(phase);
            mooncolors.append(myData->config.moons[i].color);
        }
        //
        int number = cell.leapDef != nullptr ? cell.displayDayNumber + cell.leapDef->uniqueToMeLinks.count() : cell.displayDayNumber;
        DaySlot *day = new DaySlot(ui->DayHolder, number, cell.gridCol, myData->linksForDay(currentYear, currentMonth, cell.displayDayNumber, cell.leapDef).count(), &moonsphases, &mooncolors, cell.leapDef);
        //
        if (cell.isLeapDay) {
            day->ColorMe(cell.leapHex);
        }

        qobject_cast<QGridLayout*>(ui->DayHolder->layout())->addWidget(day, cell.gridRow, cell.gridCol);
        connect(day, &DaySlot::clicked, this, &CalendarEditor::SelectDay);
    }
    QLayout *layout = ui->DayLinksHolder->layout();
    while (layout->count() > 1) {
        QLayoutItem *item = layout->takeAt(layout->count() - 1);
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
    ui->LinkFrame->setVisible(false);

    for (const RenderCell& cell : simulatedCells) {
        QVector<DayLink> links = myData->linksForDay(currentYear, currentMonth, cell.displayDayNumber, cell.leapDef);

        for (int j = 0; j < links.count(); j++) {
            DayLink link = links[j];
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
    {
        if(selectedDay->leapDayDef == nullptr)
            selectedDay->ColorMe("#FFF4F4");
        else
            selectedDay->ColorMe("#89CFF0");
    }

    selectedDay = day;

    if(selectedDay->leapDayDef == nullptr)
        selectedDay->ColorMe("#ffe6e6");
    else
        selectedDay->ColorMe("#99B8C7");

    QVector<DayLink> links = myData->linksForDay(currentYear, currentMonth, day->dayNumber, day->leapDayDef);
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
                if(linksForKey[i].colorHex == link.colorHex && linksForKey[i].displayLabel == link.displayLabel && linksForKey[i].targetNoteId == link.targetNoteId)
                    linksForKey.remove(i);
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
                if(linksForKey[i].colorHex == link.colorHex && linksForKey[i].displayLabel == link.displayLabel && linksForKey[i].targetNoteId == link.targetNoteId)
                    linksForKey.remove(i);
            }
            myData->specificEvents.remove(iteratorSpe.key());
            for(int i = 0; i < linksForKey.count(); i++)
            {
                myData->specificEvents.insert(iteratorSpe.key(), linksForKey);
            }
        }
        for(int k = 0; k < myData->config.leapDays.size(); k++)
        {
            QVector<DayLink>& leapLinks = myData->config.leapDays[k].uniqueToMeLinks;
            for(int i = leapLinks.size() - 1; i >= 0; i--)
            {
                if(leapLinks[i].colorHex == link.colorHex &&
                    leapLinks[i].displayLabel == link.displayLabel &&
                    leapLinks[i].targetNoteId == link.targetNoteId)
                {
                    leapLinks.remove(i);
                }
            }
        }
        SelectDay(selectedDay);
        for(int i = 0; i < ui->DayHolder->layout()->count(); ++i)
        {
            QLayoutItem *item = ui->DayHolder->layout()->itemAt(i);
            if (item && item->widget()) {
                DaySlot *slot = qobject_cast<DaySlot*>(item->widget());
                if (slot) {
                    slot->thisDaysLinks = myData->linksForDay(currentYear, currentMonth, slot->dayNumber, slot->leapDayDef).count();
                    slot->UpdateEventTracker();
                }
            }
        }
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
        LinkScope chosenScope;
        DayLink link = CreateDayLink(chosenScope);

        if(link.targetNoteId.isEmpty())
            return;

        if (chosenScope == ScopeYearly)
        {
            RecurringDateKey rk{currentMonth, selectedDay->dayNumber};
            myData->recurringEvents[rk].append(link);
        }
        else if (chosenScope == ScopeLeapDay)
        {
            for(int k = 0; k < myData->config.leapDays.size(); k++)
            {
                if (&myData->config.leapDays[k] == selectedDay->leapDayDef)
                {
                    myData->config.leapDays[k].uniqueToMeLinks.append(link);
                    break;
                }
            }
        }
        else
        {
            myData->addSpecificLink(currentYear, currentMonth, selectedDay->dayNumber, link);
        }

        SelectDay(selectedDay);
        for(int i = 0; i < ui->DayHolder->layout()->count(); ++i)
        {
            QLayoutItem *item = ui->DayHolder->layout()->itemAt(i);
            if (item && item->widget()) {
                DaySlot *slot = qobject_cast<DaySlot*>(item->widget());
                if (slot) {
                    slot->thisDaysLinks = myData->linksForDay(currentYear, currentMonth, slot->dayNumber, slot->leapDayDef).count();
                    slot->UpdateEventTracker();
                }
            }
        }
        emit Updated();
    }
}

DayLink CalendarEditor::CreateDayLink(LinkScope &outScope)
{
    QDialog dialog(this);
    dialog.setWindowTitle("Create New Link");
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QString style =
        "QRadioButton::indicator::unchecked {"
        "  border: 2px solid black;"
        "  border-radius: 6px;"
        "  background-color: white;"
        "  width: 12px;"
        "  height: 12px;"
        "}"
        "QRadioButton::indicator::checked {"
        "  border: 2px solid black;"
        "  border-radius: 6px;"
        "  width: 12px;"
        "  height: 12px;"
        "  background: black;"
        "}";
    QGroupBox *scopeGroup = new QGroupBox("Link Type", &dialog);
    QVBoxLayout *scopeLayout = new QVBoxLayout(scopeGroup);
    QRadioButton *rbSpecific = new QRadioButton("Specific Date Only", scopeGroup);
    QRadioButton *rbYearly = new QRadioButton("Yearly (Recurring)", scopeGroup);
    QRadioButton *rbLeap = nullptr;
    rbSpecific->setStyleSheet(style);
    rbYearly->setStyleSheet(style);

    rbSpecific->setChecked(true);
    scopeLayout->addWidget(rbSpecific);
    scopeLayout->addWidget(rbYearly);
    if (selectedDay != nullptr && selectedDay->leapDayDef != nullptr) {
        rbLeap = new QRadioButton("Leap Day Only", scopeGroup);
        rbLeap->setStyleSheet(style);
        scopeLayout->addWidget(rbLeap);
    }
    layout->addWidget(scopeGroup);

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
                        if (++count >= 10) break;
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
        if (rbLeap && rbLeap->isChecked()) outScope = ScopeLeapDay;
        else if (rbYearly->isChecked())    outScope = ScopeYearly;
        else                               outScope = ScopeSpecific;

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
