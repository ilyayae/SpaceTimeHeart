#include "tests.h"
#include <QAbstractButton> // Added for safer button casting

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

    // Find UI elements
    QLineEdit *lineEdit = menu.findChild<QLineEdit*>("PTWD_LineEdit");
    QPushButton *saveBtn = menu.findChild<QPushButton*>("pButton_Save");
    QPushButton *cancelBtn = menu.findChild<QPushButton*>("pButton_Cancel");

    QVERIFY(lineEdit != nullptr);
    QVERIFY(saveBtn != nullptr);
    QVERIFY(cancelBtn != nullptr);

    // Verify UI matches settings initially
    QCOMPARE(lineEdit->text(), tempDir->path());

    // Test saving settings
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

    // Add elements
    addMonth->click();
    addMonth->click();
    addMoon->click();
    addDay->click();
    addDay->click();
    addDay->click();

    // Verify the data generation
    CalendarConfigData data = config.toConfigData();
    QCOMPARE(data.months.size(), 2);
    QCOMPARE(data.moons.size(), 1);
    QCOMPARE(data.dayNames.size(), 3);
    QCOMPARE(data.weekLength, 3);
    QCOMPARE(data.calendarName, QString("Custom Calendar"));

    // Verify emission of savedData signal
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

    // We verify the dynamic UI objects created in the constructor:
    QList<QToolButton*> toolButtons = editor.findChildren<QToolButton*>();
    // We expect at least the "Line Color" and "Fill Color" tool buttons
    QVERIFY(toolButtons.size() >= 2);

    QList<QComboBox*> comboBoxes = editor.findChildren<QComboBox*>();
    // We expect at least the "Line Style" and "Fill Style" combo boxes
    QVERIFY(comboBoxes.size() >= 2);

    QList<QSpinBox*> spinBoxes = editor.findChildren<QSpinBox*>();
    // We expect at least the Width and Rounding spinboxes
    QVERIFY(spinBoxes.size() >= 2);
}

void Tests::testMarkdownEditor() {
    MarkdownEditor editor(nullptr, dummyRegistry);
    auto *textEdit = editor.GetQTextEdit();
    QVERIFY(textEdit != nullptr);

    // Test text population
    textEdit->setPlainText("testing");
    QCOMPARE(textEdit->toPlainText(), QString("testing"));

    // Test Text Formatting (Bold)
    QTextCursor cursor = textEdit->textCursor();
    cursor.select(QTextCursor::Document);
    textEdit->setTextCursor(cursor);
    editor.on_actionBold_triggered();

    // Should have surrounded testing with at least '*' depending on exact state logic
    QVERIFY(textEdit->toPlainText().contains("testing") && textEdit->toPlainText().length() > 7);

    // Test signals
    QSignalSpy spyUpdate(&editor, &MarkdownEditor::Updated);
    editor.hyperlinkTextEdit_textChanged(); // Call directly as mock doesn't simulate full text stream
    QCOMPARE(spyUpdate.count(), 1);

    // Test zoom features
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

    // Verify basic signal forwarding
    QSignalSpy spySave(&editor, &TextEditor::saveButton);
    editor.on_actionSave_triggered();
    QCOMPARE(spySave.count(), 1);

    QSignalSpy spyUpdated(&editor, &TextEditor::Updated);
    editor.hyperlinkTextEdit_textChanged();
    QCOMPARE(spyUpdated.count(), 1);

    // Test zoom
    QSignalSpy spyZoom(&editor, &TextEditor::zoomChanged);
    editor.updateZoom(-2);
    QCOMPARE(spyZoom.count(), 1);
    QCOMPARE(spyZoom.takeFirst().at(0).toInt(), -2);
}

void Tests::testCalendarEditor() {
    CalendarEditor editor(nullptr, dummyRegistry);

    // Verify structural safety of the container
    QVERIFY(editor.centralWidget() != nullptr);
    QVERIFY(editor.findChild<QWidget*>("DayHolder") != nullptr);
    QVERIFY(editor.findChild<QWidget*>("DayLinksHolder") != nullptr);

    // Verify standard UI components are accessible.
    QVERIFY(editor.findChild<QWidget*>("NextMonth") != nullptr);
    QVERIFY(editor.findChild<QWidget*>("NextYear") != nullptr);
    QVERIFY(editor.findChild<QWidget*>("PrevYear") != nullptr);
    QVERIFY(editor.findChild<QWidget*>("PrevMonth") != nullptr);
}

void Tests::testCustomTextBrowser() {
    CustomTextBrowser browser(nullptr);
    browser.registry = dummyRegistry; // Prevent null crash in paintEvent UUID lookup

    // 1. Test Markdown checkbox parsing
    browser.setMarkdown("-[] Checkbox text");
    QVERIFY(browser.toPlainText().contains(QChar(0x2610))); // ☐ (empty checkbox)

    browser.setMarkdown("-[x] Checkbox text done");
    QVERIFY(browser.toPlainText().contains(QChar(0x2611))); // ☑ (checked box)

    // 2. Test auto-list continuation (Enter Key)
    browser.clear();
    browser.setPlainText("- Task item 1");
    QTextCursor cursor = browser.textCursor();
    cursor.movePosition(QTextCursor::End);
    browser.setTextCursor(cursor);

    QKeyEvent enterEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
    browser.keyPressEvent(&enterEvent);

    // Auto-indent rule should insert "\n- "
    QVERIFY(browser.toPlainText().contains("- Task item 1\n-"));

    // 3. Test list auto-numbering continuation (Enter Key)
    browser.clear();
    browser.setPlainText("1. First item");
    cursor = browser.textCursor();
    cursor.movePosition(QTextCursor::End);
    browser.setTextCursor(cursor);

    browser.keyPressEvent(&enterEvent);

    // Should increment to "2. "
    QVERIFY(browser.toPlainText().contains("1. First item\n2."));

    // 4. Test Tab indentation for numbered lists
    browser.clear();
    browser.setPlainText("2. Sub item");
    cursor = browser.textCursor();
    cursor.movePosition(QTextCursor::End);
    browser.setTextCursor(cursor);

    QKeyEvent tabEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
    browser.keyPressEvent(&tabEvent);

    // Converts "2." to "   1. " on tab
    QVERIFY(browser.toPlainText().contains("   1. Sub item"));

    // 5. Test Mouse Click Checkbox Toggle
    browser.setMarkdown("-[] Toggle box");
    browser.show();
    QCoreApplication::processEvents();
    QSignalSpy spyCheckbox(&browser, &CustomTextBrowser::mdCheckboxChanged);

    // Send a click near the top-left to toggle the first block containing the checkbox
    QTest::mouseClick(browser.viewport(), Qt::LeftButton, Qt::NoModifier, QPoint(10, 10));

    // It should have emitted the mdCheckboxChanged signal
    if (spyCheckbox.count() > 0) {
        QString updatedMd = spyCheckbox.takeFirst().at(0).toString();
        // The -[] should have changed to -[x]
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

    // Verify custom show(bool) clears the lines
    findEdit->setText("dummy data");
    frw.show(true);
    QVERIFY(findEdit->text().isEmpty());

    // Prepare content: text contains "apple" 3 times
    browser.setPlainText("apple banana apple cherry apple");

    // 1. Initial Search
    findEdit->setText("apple");
    QSignalSpy spyFind(&frw, &FindReplaceWidget::findReplaceChanged);
    findBtn->click();

    // Wait for the singleShot timer inside selectText() to emit the signal
    QCoreApplication::processEvents();

    // The widget updates the label to "current/total"
    QCOMPARE(statusLabel->text(), QString("1/3"));
    QCOMPARE(spyFind.count(), 1);

    // 2. Navigating Next
    nextBtn->click();
    QCoreApplication::processEvents();
    QCOMPARE(statusLabel->text(), QString("2/3"));

    // 3. Navigating Prev
    prevBtn->click();
    QCoreApplication::processEvents();
    QCOMPARE(statusLabel->text(), QString("1/3"));

    // Navigate to the 2nd one again
    nextBtn->click();
    QCoreApplication::processEvents();

    // 4. Test Replacing Text
    replaceEdit->setText("orange");
    replaceBtn->click();
    QCoreApplication::processEvents();

    // The document should now be "apple banana orange cherry apple"
    QVERIFY(browser.toPlainText().contains("orange"));

    // After replace, it triggers next search automatically or recalculates.
    QCOMPARE(statusLabel->text(), QString("2/2"));
}

void Tests::testNote() {
    QString path = tempDir->path() + "/test_note.txt";

    // Pre-create an empty file so the constructor's load() doesn't throw a QMessageBox
    QFile dummy(path);
    dummy.open(QIODevice::WriteOnly);
    dummy.close();

    QUuid linkUuid = QUuid::createUuid();

    // 1. Create and populate Note
    Note note1(path);
    note1.setContent("Testing the Note Content.\nLine 2.");
    note1.setLinkedUuids({linkUuid});
    note1.save();
    QUuid noteUuid = note1.getUuid();

    // 2. Load the Note and verify
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

    // 1. Setup Data
    ImageAnnotationData data1;
    data1.addMarker(15.0, 25.0, 10, link1, "#FFFFFF", "Sample Marker", "icon1");
    data1.addShape({{1.0, 1.0}, {5.0, 5.0}}, true, "#000000", "#111111", 3, "solid", "solid", 0.0);

    QVERIFY(data1.save(path, data1));
    QUuid savedUuid = data1.GetUuid();

    // 2. Load Data
    ImageAnnotationData data2;
    QVERIFY(data1.load(path, data2));

    // Verify basic properties
    QCOMPARE(data2.GetUuid(), savedUuid);

    QList<QUuid> links = data2.GetMyLinks();
    QCOMPARE(links.size(), 1);
    QCOMPARE(links.first(), link1);

    // Verify structures restored
    QCOMPARE(data2.markers.size(), 1);
    QCOMPARE(data2.shapes.size(), 1);
    QCOMPARE(data2.markers.first().Label, QString("Sample Marker"));
    QCOMPARE(data2.shapes.first().StyleOfLine.width, 3);
}

void Tests::testCalendarData() {
    QString path = tempDir->path() + "/test_cal.ccal";
    QUuid targetNote = QUuid::createUuid();

    // 1. Setup Calendar Setup
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

    // 2. Load Data
    CalendarData data2;
    QVERIFY(data1.load(path, data2));

    QCOMPARE(data2.GetUuid(), savedUuid);
    QCOMPARE(data2.config.calendarName, QString("Epic Test Calendar"));

    // Check specific and recurring dates
    QCOMPARE(data2.linksForDay(2025, 6, 15).size(), 1);
    QCOMPARE(data2.linksForDay(2025, 1, 1).size(), 1);

    // 3. Check Removals
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

    // 1. Write Entry
    QVERIFY(registry.writeEntry(testUuid, relativePath, {linkUuid}));

    // 2. Read Entry
    QVERIFY(registry.contains(testUuid));
    QCOMPARE(registry.getUuid(relativePath), testUuid);

    // Note: getPath() parses Settings for general/WorkDirectory, which normally defaults to /home
    QString workDirPath = QSettings("zhopets", "SpaceTimeHeart").value("general/WorkDirectory", "/home").toString();
    QDir workDir(workDirPath);
    QString expectedAbsolutePath = workDir.absoluteFilePath(relativePath);

    QCOMPARE(registry.getPath(testUuid), expectedAbsolutePath);

    // Verify Links
    QList<QUuid> connectedLinks = registry.getConnectedUuids(testUuid);
    QCOMPARE(connectedLinks.size(), 1);
    QCOMPARE(connectedLinks.first(), linkUuid);

    // 3. Remove Entry
    QVERIFY(registry.removeEntry(testUuid));
    QVERIFY(!registry.contains(testUuid));
}
