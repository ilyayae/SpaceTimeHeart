#include "tests.h"
#include <QAbstractButton>

Tests::Tests(QObject *parent)
    : QObject{parent}
{

}

void Tests::init() {
    QStandardPaths::setTestModeEnabled(true);

    tempDir = new QTemporaryDir();
    QVERIFY(tempDir->isValid());

    testSettings = new QSettings(tempDir->path() + "/test_settings.ini", QSettings::IniFormat);
    testSettings->setValue("general/WorkDirectory", tempDir->path());

    dummyWidget = new QWidget();
    dummyLayout = new QGridLayout(dummyWidget);

    docHandler = new DocumentHandler(nullptr, testSettings, dummyLayout);
    dummyRegistry = new UuidRegistry(tempDir->path() + "/dummy_registry.db", this);
}

void Tests::cleanupTestCase() {
    delete docHandler;
    delete dummyLayout;
    delete dummyWidget;
    delete testSettings;
    delete tempDir;
}

void Tests::testInitialState() {
    QCOMPARE(docHandler->getCurrentEditor(), EMPTY);
}

void Tests::testSwitchEditor() {
    docHandler->switchEditor(TEXT);
    QCOMPARE(docHandler->getCurrentEditor(), TEXT);

    docHandler->switchEditor(MARKDOWN);
    QCOMPARE(docHandler->getCurrentEditor(), MARKDOWN);

    docHandler->switchEditor(EMPTY);
    QCOMPARE(docHandler->getCurrentEditor(), EMPTY);
}

void Tests::testFileExtensionParsing() {
    QString txtPath = tempDir->path() + "/test.txt";
    QString mdPath = tempDir->path() + "/test.md";

    QFile txtFile(txtPath);
    if (txtFile.open(QIODevice::WriteOnly)) {
        txtFile.write("Dummy text");
        txtFile.close();
    }

    QFile mdFile(mdPath);
    if (mdFile.open(QIODevice::WriteOnly)) {
        mdFile.write("# Dummy Markdown");
        mdFile.close();
    }

    docHandler->loadFile(txtPath);
    QCOMPARE(docHandler->getCurrentEditor(), TEXT);

    docHandler->loadFile(mdPath);
    QCOMPARE(docHandler->getCurrentEditor(), MARKDOWN);

    QString unknownPath = tempDir->path() + "/test.unknown";
    QFile unkFile(unknownPath); unkFile.open(QIODevice::WriteOnly); unkFile.close();
    docHandler->loadFile(unknownPath);
    QCOMPARE(docHandler->getCurrentEditor(), EMPTY);
}

void Tests::testConfigMenu() {
    ConfigMenu menu(nullptr, testSettings);

    QLineEdit *lineEdit = menu.findChild<QLineEdit*>("PTWD_LineEdit");
    QPushButton *saveBtn = menu.findChild<QPushButton*>("pButton_Save");
    QPushButton *cancelBtn = menu.findChild<QPushButton*>("pButton_Cancel");

    QVERIFY(lineEdit != nullptr);
    QVERIFY(saveBtn != nullptr);
    QVERIFY(cancelBtn != nullptr);

    QCOMPARE(lineEdit->text(), tempDir->path());

    lineEdit->setText("/dummy/test/path");
    QSignalSpy spy(&menu, &ConfigMenu::savedSettings);

    saveBtn->click();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(testSettings->value("general/WorkDirectory").toString(), QString("/dummy/test/path"));
    QCOMPARE(testSettings->value("general/FirstLaunch").toBool(), false);
}

void Tests::testCalendarConfig() {
    CalendarConfig config;

    QPushButton *addMonth = config.findChild<QPushButton*>("AddMonth");
    QPushButton *addMoon = config.findChild<QPushButton*>("AddMoon");
    QPushButton *addDay = config.findChild<QPushButton*>("AddDay");
    QPushButton *confirm = config.findChild<QPushButton*>("ConfirmCreation");

    QVERIFY(addMonth != nullptr);
    QVERIFY(addMoon != nullptr);
    QVERIFY(addDay != nullptr);
    QVERIFY(confirm != nullptr);

    addMonth->click();
    addMonth->click();
    addMoon->click();
    addDay->click();
    addDay->click();
    addDay->click();

    CalendarConfigData data = config.toConfigData();
    QCOMPARE(data.months.size(), 2);
    QCOMPARE(data.moons.size(), 1);
    QCOMPARE(data.dayNames.size(), 3);
    QCOMPARE(data.weekLength, 3);
    QCOMPARE(data.calendarName, QString("Custom Calendar"));

    QSignalSpy spy(&config, &CalendarConfig::savedData);
    confirm->click();
    QCOMPARE(spy.count(), 1);
}

void Tests::testEmptyEditor() {
    EmptyEditor editor;
    QVERIFY(editor.parentWidget() == nullptr);
    editor.show();
    QVERIFY(editor.isVisible());
}

void Tests::testImageAnnotationEditor() {
    ImageAnnotationEditor editor;

    QList<QToolButton*> toolButtons = editor.findChildren<QToolButton*>();
    QVERIFY(toolButtons.size() >= 2);

    QList<QComboBox*> comboBoxes = editor.findChildren<QComboBox*>();
    QVERIFY(comboBoxes.size() >= 2);

    QList<QSpinBox*> spinBoxes = editor.findChildren<QSpinBox*>();
    QVERIFY(spinBoxes.size() >= 2);
}

void Tests::testMarkdownEditor() {
    MarkdownEditor editor(nullptr, dummyRegistry);
    auto *textEdit = editor.GetQTextEdit();
    QVERIFY(textEdit != nullptr);

    textEdit->setPlainText("testing");
    QCOMPARE(textEdit->toPlainText(), QString("testing"));

    QTextCursor cursor = textEdit->textCursor();
    cursor.select(QTextCursor::Document);
    textEdit->setTextCursor(cursor);
    editor.on_actionBold_triggered();

    QVERIFY(textEdit->toPlainText().contains("testing") && textEdit->toPlainText().length() > 7);

    QSignalSpy spyUpdate(&editor, &MarkdownEditor::Updated);
    editor.hyperlinkTextEdit_textChanged();
    QCOMPARE(spyUpdate.count(), 1);

    QSignalSpy spyZoom(&editor, &MarkdownEditor::zoomChanged);
    editor.updateZoom(4);
    QCOMPARE(spyZoom.count(), 1);
    QCOMPARE(spyZoom.takeFirst().at(0).toInt(), 4);

    QSignalSpy spySaveAs(&editor, &MarkdownEditor::saveAsButton);
    editor.on_actionSaveAs_triggered();
    QCOMPARE(spySaveAs.count(), 1);
}

void Tests::testMindMap() {
    MindMap mindMap(nullptr, dummyRegistry);
    QVERIFY(mindMap.centralWidget() != nullptr);
}

void Tests::testTextEditor() {
    TextEditor editor(nullptr, dummyRegistry);
    auto *textEdit = editor.getQTextEdit();
    QVERIFY(textEdit != nullptr);

    QSignalSpy spySave(&editor, &TextEditor::saveButton);
    editor.on_actionSave_triggered();
    QCOMPARE(spySave.count(), 1);

    QSignalSpy spyUpdated(&editor, &TextEditor::Updated);
    editor.hyperlinkTextEdit_textChanged();
    QCOMPARE(spyUpdated.count(), 1);

    QSignalSpy spyZoom(&editor, &TextEditor::zoomChanged);
    editor.updateZoom(-2);
    QCOMPARE(spyZoom.count(), 1);
    QCOMPARE(spyZoom.takeFirst().at(0).toInt(), -2);
}

void Tests::testCalendarEditor() {
    CalendarEditor editor(nullptr, dummyRegistry);

    QVERIFY(editor.centralWidget() != nullptr);
    QVERIFY(editor.findChild<QWidget*>("DayHolder") != nullptr);
    QVERIFY(editor.findChild<QWidget*>("DayLinksHolder") != nullptr);

    QVERIFY(editor.findChild<QWidget*>("NextMonth") != nullptr);
    QVERIFY(editor.findChild<QWidget*>("NextYear") != nullptr);
    QVERIFY(editor.findChild<QWidget*>("PrevYear") != nullptr);
    QVERIFY(editor.findChild<QWidget*>("PrevMonth") != nullptr);
}

void Tests::testCustomTextBrowser() {
    CustomTextBrowser browser(nullptr);
    browser.registry = dummyRegistry;

    browser.setMarkdown("-[] Checkbox text");
    QVERIFY(browser.toPlainText().contains(QChar(0x2610)));

    browser.setMarkdown("-[x] Checkbox text done");
    QVERIFY(browser.toPlainText().contains(QChar(0x2611)));

    browser.clear();
    browser.setPlainText("- Task item 1");
    QTextCursor cursor = browser.textCursor();
    cursor.movePosition(QTextCursor::End);
    browser.setTextCursor(cursor);

    QKeyEvent enterEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
    browser.keyPressEvent(&enterEvent);

    QVERIFY(browser.toPlainText().contains("- Task item 1\n-"));

    browser.clear();
    browser.setPlainText("1. First item");
    cursor = browser.textCursor();
    cursor.movePosition(QTextCursor::End);
    browser.setTextCursor(cursor);

    browser.keyPressEvent(&enterEvent);

    QVERIFY(browser.toPlainText().contains("1. First item\n2."));

    browser.clear();
    browser.setPlainText("2. Sub item");
    cursor = browser.textCursor();
    cursor.movePosition(QTextCursor::End);
    browser.setTextCursor(cursor);

    QKeyEvent tabEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
    browser.keyPressEvent(&tabEvent);

    QVERIFY(browser.toPlainText().contains("   1. Sub item"));

    browser.setMarkdown("-[] Toggle box");
    browser.show();
    QCoreApplication::processEvents();
    QSignalSpy spyCheckbox(&browser, &CustomTextBrowser::mdCheckboxChanged);

    QTest::mouseClick(browser.viewport(), Qt::LeftButton, Qt::NoModifier, QPoint(10, 10));

    if (spyCheckbox.count() > 0) {
        QString updatedMd = spyCheckbox.takeFirst().at(0).toString();
        QVERIFY(updatedMd.contains("-[x]"));
    }
}

void Tests::testFindReplaceWidget() {
    CustomTextBrowser browser(nullptr);
    browser.show();

    FindReplaceWidget frw(nullptr, &browser);
    frw.show(true);

    QLineEdit *findEdit = frw.findChild<QLineEdit*>("FindLineEdit");
    QLineEdit *replaceEdit = frw.findChild<QLineEdit*>("ReplaceLineEdit");
    QAbstractButton *findBtn = frw.findChild<QAbstractButton*>("FindButton");
    QAbstractButton *nextBtn = frw.findChild<QAbstractButton*>("FindNextButton");
    QAbstractButton *prevBtn = frw.findChild<QAbstractButton*>("FindPrevButton");
    QAbstractButton *replaceBtn = frw.findChild<QAbstractButton*>("ReplaceButton");
    QLabel *statusLabel = frw.findChild<QLabel*>("CurrentFoundLabel");

    QVERIFY(findEdit);
    QVERIFY(replaceEdit);
    QVERIFY(findBtn);
    QVERIFY(nextBtn);
    QVERIFY(prevBtn);
    QVERIFY(replaceBtn);
    QVERIFY(statusLabel);

    findEdit->setText("dummy data");
    frw.show(true);
    QVERIFY(findEdit->text().isEmpty());

    browser.setPlainText("apple banana apple cherry apple");

    findEdit->setText("apple");
    QSignalSpy spyFind(&frw, &FindReplaceWidget::findReplaceChanged);
    findBtn->click();

    QCoreApplication::processEvents();

    QCOMPARE(statusLabel->text(), QString("1/3"));
    QCOMPARE(spyFind.count(), 1);

    nextBtn->click();
    QCoreApplication::processEvents();
    QCOMPARE(statusLabel->text(), QString("2/3"));

    prevBtn->click();
    QCoreApplication::processEvents();
    QCOMPARE(statusLabel->text(), QString("1/3"));

    nextBtn->click();
    QCoreApplication::processEvents();

    replaceEdit->setText("orange");
    replaceBtn->click();
    QCoreApplication::processEvents();

    QVERIFY(browser.toPlainText().contains("orange"));

    QCOMPARE(statusLabel->text(), QString("2/2"));
}

void Tests::testNote() {
    QString path = tempDir->path() + "/test_note.txt";

    QFile dummy(path);
    dummy.open(QIODevice::WriteOnly);
    dummy.close();

    QUuid linkUuid = QUuid::createUuid();

    Note note1(path);
    note1.setContent("Testing the Note Content.\nLine 2.");
    note1.setLinkedUuids({linkUuid});
    note1.save();
    QUuid noteUuid = note1.getUuid();

    Note note2(path);
    QCOMPARE(note2.getUuid(), noteUuid);
    QCOMPARE(note2.getContent(), QString("Testing the Note Content.\nLine 2."));

    QList<QUuid> loadedLinks = note2.GetMyLinks();
    QCOMPARE(loadedLinks.size(), 1);
    QCOMPARE(loadedLinks.first(), linkUuid);
}

void Tests::testImageAnnotationData() {
    QString path = tempDir->path() + "/test_image.iman";
    QUuid link1 = QUuid::createUuid();

    ImageAnnotationData data1;
    data1.addMarker(15.0, 25.0, 10, link1, "#FFFFFF", "Sample Marker", "icon1");
    data1.addShape({{1.0, 1.0}, {5.0, 5.0}}, true, "#000000", "#111111", 3, "solid", "solid", 0.0);

    QVERIFY(data1.save(path, data1));
    QUuid savedUuid = data1.GetUuid();

    ImageAnnotationData data2;
    QVERIFY(data1.load(path, data2));

    QCOMPARE(data2.GetUuid(), savedUuid);

    QList<QUuid> links = data2.GetMyLinks();
    QCOMPARE(links.size(), 1);
    QCOMPARE(links.first(), link1);

    QCOMPARE(data2.markers.size(), 1);
    QCOMPARE(data2.shapes.size(), 1);
    QCOMPARE(data2.markers.first().Label, QString("Sample Marker"));
    QCOMPARE(data2.shapes.first().StyleOfLine.width, 3);
}

void Tests::testCalendarData() {
    QString path = tempDir->path() + "/test_cal.ccal";
    QUuid targetNote = QUuid::createUuid();

    CalendarData data1;
    data1.config.calendarName = "Epic Test Calendar";
    data1.config.weekLength = 7;

    DayLink link;
    link.targetNoteId = targetNote.toString(QUuid::WithoutBraces);
    link.displayLabel = "Anniversary";
    link.colorHex = "#FF00FF";

    data1.addRecurringLink(6, 15, link);
    data1.addSpecificLink(2025, 1, 1, link);

    QVERIFY(data1.save(path, data1));
    QUuid savedUuid = data1.GetUuid();

    CalendarData data2;
    QVERIFY(data1.load(path, data2));

    QCOMPARE(data2.GetUuid(), savedUuid);
    QCOMPARE(data2.config.calendarName, QString("Epic Test Calendar"));

    QCOMPARE(data2.linksForDay(2025, 6, 15).size(), 1);
    QCOMPARE(data2.linksForDay(2025, 1, 1).size(), 1);

    data2.removeLinksToNote(targetNote.toString(QUuid::WithoutBraces));
    QCOMPARE(data2.linksForDay(2025, 6, 15).size(), 0);
    QCOMPARE(data2.linksForDay(2025, 1, 1).size(), 0);
}

void Tests::testUuidRegistry() {
    QString dbPath = tempDir->path() + "/test_uuid_registry.db";
    UuidRegistry registry(dbPath);

    QUuid testUuid = QUuid::createUuid();
    QUuid linkUuid = QUuid::createUuid();
    QString relativePath = "some/nested/test/file.txt";

    QVERIFY(registry.writeEntry(testUuid, relativePath, {linkUuid}));

    QVERIFY(registry.contains(testUuid));
    QCOMPARE(registry.getUuid(relativePath), testUuid);

    QString workDirPath = QSettings("zhopets", "SpaceTimeHeart").value("general/WorkDirectory", "/home").toString();
    QDir workDir(workDirPath);
    QString expectedAbsolutePath = workDir.absoluteFilePath(relativePath);

    QCOMPARE(registry.getPath(testUuid), expectedAbsolutePath);

    QList<QUuid> connectedLinks = registry.getConnectedUuids(testUuid);
    QCOMPARE(connectedLinks.size(), 1);
    QCOMPARE(connectedLinks.first(), linkUuid);

    QVERIFY(registry.removeEntry(testUuid));
    QVERIFY(!registry.contains(testUuid));
}
void Tests::testMindMapNode() {
    QGraphicsScene scene;
    QUuid nodeUuid1 = QUuid::createUuid();
    QUuid nodeUuid2 = QUuid::createUuid();

    MindMapNode *node1 = new MindMapNode(nullptr, nodeUuid1, "Root Node", {nodeUuid2});
    MindMapNode *node2 = new MindMapNode(nullptr, nodeUuid2, "Child Node", {});

    scene.addItem(node1);
    scene.addItem(node2);

    QCOMPARE(node1->MyUuid, nodeUuid1);
    QCOMPARE(node1->MyName, QString("Root Node"));
    QCOMPARE(node1->MyConnections.size(), 1);

    QVERIFY(!node1->boundingRect().isEmpty());

    QMap<QUuid, MindMapNode*> nodesMap;
    nodesMap[nodeUuid1] = node1;
    nodesMap[nodeUuid2] = node2;

    node1->Connect(&nodesMap);

    QCOMPARE(scene.items().count(), 3);

    QSignalSpy spyClick(node1, &MindMapNode::Clicked);

    QGraphicsSceneMouseEvent pressEvent(QEvent::GraphicsSceneMousePress);
    pressEvent.setButton(Qt::LeftButton);
    node1->mousePressEvent(&pressEvent);

    QCOMPARE(spyClick.count(), 1);
    QCOMPARE(spyClick.takeFirst().at(0).toUuid(), nodeUuid1);

    node1->setHighlightEdges(true);
}

void Tests::testMindMapEdge() {
    QUuid u1 = QUuid::createUuid();
    QUuid u2 = QUuid::createUuid();

    MindMapNode n1(nullptr, u1, "Node 1", {});
    MindMapNode n2(nullptr, u2, "Node 2", {});

    n1.setPos(0, 0);
    n2.setPos(100, 100);

    MindMapEdge edge(&n1, &n2);

    QCOMPARE(edge.From, &n1);
    QCOMPARE(edge.To, &n2);

    QVERIFY(!edge.boundingRect().isEmpty());
    QVERIFY(edge.boundingRect().width() >= 100);
    QVERIFY(edge.boundingRect().height() >= 100);
}

void Tests::testMindMapView() {
    QUuid id1 = QUuid::createUuid();
    QUuid id2 = QUuid::createUuid();
    QUuid id3 = QUuid::createUuid();

    QList<QUuid> uuids = {id1, id2, id3};
    QList<QString> paths = {"/fake/path/file1.md", "/fake/path/file2.md", "/fake/path/file3.md"};

    QList<QList<QUuid>> connections = {{id2, id3}, {}, {}};

    MindMapView view(nullptr, uuids, paths, connections);

    QVERIFY(view.scene() != nullptr);

    QCOMPARE(view.scene()->items().count(), 5);

    QTest::qWait(100);

    QSignalSpy spyViewClick(&view, &MindMapView::Clicked);

    MindMapNode* firstNode = nullptr;
    for(auto item : view.scene()->items()) {
        if((firstNode = qgraphicsitem_cast<MindMapNode*>(item))) {
            break;
        }
    }
    QVERIFY(firstNode != nullptr);

    emit firstNode->Clicked(id1);
    QCOMPARE(spyViewClick.count(), 1);
    QCOMPARE(spyViewClick.takeFirst().at(0).toUuid(), id1);

    qreal initialScale = view.transform().m11();

    QWheelEvent zoomInEvent(QPointF(0, 0), QPointF(0, 0), QPoint(0, 120), QPoint(0, 120),
                            Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
    view.wheelEvent(&zoomInEvent);

    QVERIFY(view.transform().m11() > initialScale);

    qreal zoomedScale = view.transform().m11();
    QWheelEvent zoomOutEvent(QPointF(0, 0), QPointF(0, 0), QPoint(0, -120), QPoint(0, -120),
                             Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
    view.wheelEvent(&zoomOutEvent);

    QVERIFY(view.transform().m11() < zoomedScale);
}
void Tests::testMonthInYearEntry() {
    MonthInYearEntry entry;

    QLineEdit *lineName = entry.findChild<QLineEdit*>("LineName");
    QLineEdit *lineDays = entry.findChild<QLineEdit*>("LineDays");
    QPushButton *removeBtn = entry.findChild<QPushButton*>("pushButton");

    QVERIFY(lineName != nullptr);
    QVERIFY(lineDays != nullptr);
    QVERIFY(removeBtn != nullptr);

    lineName->setText("January");
    QCOMPARE(entry.name, QString("January"));

    lineDays->setText("31");
    QCOMPARE(entry.days, 31);

    QSignalSpy spyDestroy(&entry, &MonthInYearEntry::destroyMe);
    removeBtn->click();

    QCOMPARE(spyDestroy.count(), 1);
    MonthInYearEntry* emittedPtr = qvariant_cast<MonthInYearEntry*>(spyDestroy.takeFirst().at(0));
    QCOMPARE(emittedPtr, &entry);
}

void Tests::testMoonEntry() {
    MoonEntry entry;

    QLineEdit *lineName = entry.findChild<QLineEdit*>("LineName");
    QLineEdit *lineCycle = entry.findChild<QLineEdit*>("LineCycle");
    QLineEdit *lineOffset = entry.findChild<QLineEdit*>("LineOffset");
    QPushButton *removeBtn = entry.findChild<QPushButton*>("RemoveButton");
    QPushButton *colorBtn = entry.findChild<QPushButton*>("ColorButton");

    QVERIFY(lineName != nullptr);
    QVERIFY(lineCycle != nullptr);
    QVERIFY(lineOffset != nullptr);
    QVERIFY(removeBtn != nullptr);
    QVERIFY(colorBtn != nullptr);

    QCOMPARE(entry.colorHex, QString("#C0C0C0"));

    lineName->setText("Luna");
    QCOMPARE(entry.name, QString("Luna"));

    lineCycle->setText("29.5");
    QVERIFY(qFuzzyCompare(entry.cycle, 29.5f));

    lineOffset->setText("12.25");
    QVERIFY(qFuzzyCompare(entry.offset, 12.25f));

    QSignalSpy spyDestroy(&entry, &MoonEntry::destroyMe);
    removeBtn->click();

    QCOMPARE(spyDestroy.count(), 1);
    MoonEntry* emittedPtr = qvariant_cast<MoonEntry*>(spyDestroy.takeFirst().at(0));
    QCOMPARE(emittedPtr, &entry);
}

void Tests::testDayInWeekEntry() {
    DayInWeekEntry entry;

    QLineEdit *lineName = entry.findChild<QLineEdit*>("LineName");
    QPushButton *removeBtn = entry.findChild<QPushButton*>("pushButton");

    QVERIFY(lineName != nullptr);
    QVERIFY(removeBtn != nullptr);

    lineName->setText("Monday");
    QCOMPARE(entry.name, QString("Monday"));

    QSignalSpy spyDestroy(&entry, &DayInWeekEntry::destroyMe);
    removeBtn->click();

    QCOMPARE(spyDestroy.count(), 1);
    DayInWeekEntry* emittedPtr = qvariant_cast<DayInWeekEntry*>(spyDestroy.takeFirst().at(0));
    QCOMPARE(emittedPtr, &entry);
}

void Tests::testLinkInDay() {
    DayLink dummyLink;
    dummyLink.displayLabel = "Meeting";
    dummyLink.colorHex = "#ABCDEF";
    dummyLink.targetNoteId = "test-uuid-1234";

    LinkInDay widgetWithX(nullptr, dummyLink, "Destination Note", true);

    widgetWithX.show();

    QLabel *desc = widgetWithX.findChild<QLabel*>("Description");
    QAbstractButton *noteBtn = widgetWithX.findChild<QAbstractButton*>("NoteLinkButton");
    QAbstractButton *destroyBtn = widgetWithX.findChild<QAbstractButton*>("DestroyButton");

    QVERIFY(desc != nullptr);
    QVERIFY(noteBtn != nullptr);
    QVERIFY(destroyBtn != nullptr);

    QCOMPARE(desc->text(), QString("Meeting"));
    QCOMPARE(noteBtn->text(), QString("Destination Note"));
    QVERIFY(destroyBtn->isVisible() == true);

    QSignalSpy spyGo(&widgetWithX, &LinkInDay::GoToNote);
    noteBtn->click();
    QCOMPARE(spyGo.count(), 1);
    QCOMPARE(spyGo.takeFirst().at(0).toString(), QString("test-uuid-1234"));

    QSignalSpy spyDestroy(&widgetWithX, &LinkInDay::DestroyLink);
    destroyBtn->click();
    QCOMPARE(spyDestroy.count(), 1);

    LinkInDay widgetNoX(nullptr, dummyLink, "Destination Note", false);
    widgetNoX.show();

    QAbstractButton *hiddenBtn = widgetNoX.findChild<QAbstractButton*>("DestroyButton");
    QVERIFY(hiddenBtn != nullptr);
    QVERIFY(hiddenBtn->isVisible() == false);
}

void Tests::testMoonVisualiser() {
    MoonPhase phases[] = {
        MoonPhase::New, MoonPhase::WaxCrescent, MoonPhase::Waxing,
        MoonPhase::WaxGibbous, MoonPhase::Full, MoonPhase::WanGibbous,
        MoonPhase::Waning, MoonPhase::WanCrescent, MoonPhase::None
    };
    for (MoonPhase phase : phases) {
        MoonVisualiser mv(nullptr, phase, "#FFFFFF");

        QCOMPARE(mv.width(), 16);
        QCOMPARE(mv.height(), 16);

        QPixmap pixmap(16, 16);
        mv.render(&pixmap);

        QVERIFY(!pixmap.isNull());
    }
}
void Tests::testDaySlot() {
    QList<double> moonPhases = {0.0, 0.5, 0.99};
    QList<QString> moonColors = {"#FFFFFF", "#CCCCCC", "#FF0000"};

    DaySlot slot(nullptr, 15, 3, 2, &moonPhases, &moonColors);

    QLabel *dayLabel = slot.findChild<QLabel*>("label");
    QLabel *eventsLabel = slot.findChild<QLabel*>("eventsLabel");

    QVERIFY(dayLabel != nullptr);
    QVERIFY(eventsLabel != nullptr);
    QCOMPARE(dayLabel->text(), QString("15"));
    QCOMPARE(eventsLabel->text(), QString("(2)"));

    QList<MoonVisualiser*> moons = slot.findChildren<MoonVisualiser*>();
    QCOMPARE(moons.size(), 3);

    QWidget *holder = slot.findChild<QWidget*>("HolderOfAll");
    QVERIFY(holder != nullptr);
    slot.ColorMe("#123456");
    QVERIFY(holder->styleSheet().contains("#123456"));

    QSignalSpy spyClick(&slot, &DaySlot::clicked);

    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPointF(5, 5), QPointF(5, 5), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    slot.mousePressEvent(&pressEvent);

    QCOMPARE(spyClick.count(), 1);

    DaySlot* emittedSlot = qvariant_cast<DaySlot*>(spyClick.takeFirst().at(0));
    QCOMPARE(emittedSlot, &slot);
}
void Tests::testCustomGraphicsView() {
    CustomGraphicsView view(nullptr);

    view.setSceneRect(0, 0, 2000, 2000);
    view.resize(500, 500);
    view.show();

    QTest::qWait(50);

    qreal initialScale = view.transform().m11();

    QWheelEvent zoomInEvent(QPointF(0, 0), QPointF(0, 0), QPoint(0, 120), QPoint(0, 120),
                            Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
    view.wheelEvent(&zoomInEvent);
    QVERIFY(view.transform().m11() > initialScale);

    qreal zoomedScale = view.transform().m11();
    QWheelEvent zoomOutEvent(QPointF(0, 0), QPointF(0, 0), QPoint(0, -120), QPoint(0, -120),
                             Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
    view.wheelEvent(&zoomOutEvent);
    QVERIFY(view.transform().m11() < zoomedScale);

    QSignalSpy spyClick(&view, &CustomGraphicsView::clicked);

    QMouseEvent pressLeft(QEvent::MouseButtonPress, QPointF(10, 10), QPointF(10, 10), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    view.mousePressEvent(&pressLeft);

    QMouseEvent releaseLeft(QEvent::MouseButtonRelease, QPointF(11, 11), QPointF(11, 11), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    view.mouseReleaseEvent(&releaseLeft);

    QCOMPARE(spyClick.count(), 1);

    QSignalSpy spyRClick(&view, &CustomGraphicsView::rClicked);

    QMouseEvent pressRight(QEvent::MouseButtonPress, QPointF(50, 50), QPointF(50, 50), Qt::RightButton, Qt::RightButton, Qt::NoModifier);
    view.mousePressEvent(&pressRight);

    QCOMPARE(spyRClick.count(), 1);

    view.horizontalScrollBar()->setValue(500);
    int initialScroll = view.horizontalScrollBar()->value();

    QMouseEvent pressLeftPan(QEvent::MouseButtonPress, QPointF(100, 100), QPointF(100, 100), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    view.mousePressEvent(&pressLeftPan);

    QMouseEvent moveLeft(QEvent::MouseMove, QPointF(150, 100), QPointF(150, 100), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    view.mouseMoveEvent(&moveLeft);

    QVERIFY(view.horizontalScrollBar()->value() != initialScroll);
}

void Tests::testMarkerItem() {
    MarkerData data;
    data.size = 32;
    data.IconId = "unknown_icon";
    data.Label = "Test Marker";
    data.Color = "#FF0000";

    MarkerItem item(&data);

    item.SelectMe(true);
    QVERIFY(item.isSelected == true);

    QRectF bounds = item.boundingRect();
    QCOMPARE(bounds.width(), 32.0);
    QCOMPARE(bounds.height(), 32.0);

    QPixmap pixmap(64, 64);
    QPainter painter(&pixmap);

    item.paint(&painter, nullptr, nullptr);

    QVERIFY(!pixmap.isNull());
}

void Tests::testVectorPainterCommands() {
    ImageAnnotationData data;
    ShapeData shape;
    shape.Closed = true;
    shape.XYPoints = {{0.0, 0.0}, {1.0, 1.0}};
    data.shapes.append(shape);

    QUndoStack stack;

    MovePointCommand *moveCmd = new MovePointCommand(&data, 0, 0, {0.5, 0.5});
    stack.push(moveCmd);
    QCOMPARE(data.shapes[0].XYPoints[0].first, 0.5);

    stack.undo();
    QCOMPARE(data.shapes[0].XYPoints[0].first, 0.0);

    ShapeData newData = shape;
    newData.StyleOfLine.width = 10;

    ChangeStyleCommand *styleCmd = new ChangeStyleCommand(&data.shapes[0], newData);
    stack.push(styleCmd);
    QCOMPARE(data.shapes[0].StyleOfLine.width, 10);

    stack.undo();
    QCOMPARE(data.shapes[0].StyleOfLine.width, shape.StyleOfLine.width);

    DeleteShapeCommand *delCmd = new DeleteShapeCommand(&data, data.shapes[0]);
    stack.push(delCmd);
    QCOMPARE(data.shapes.count(), 0);

    stack.undo();
    QCOMPARE(data.shapes.count(), 1);
}
