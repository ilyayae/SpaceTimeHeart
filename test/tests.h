#ifndef TESTS_H
#define TESTS_H

#include <QObject>
#include <QtTest>
#include <QCoreApplication>
#include <QGridLayout>
#include <QSignalSpy>
#include <QLineEdit>
#include <QPushButton>
#include <QToolBar>
#include <QKeyEvent>
#include <QTimer>
#include <QLabel>
#include <QTextCursor>

#include <include/documenthandler.h>
#include "include/configmenu.h"
#include "include/calendarconfig.h"
#include "include/editors/emptyeditor.h"
#include "include/editors/imageannotationeditor.h"
#include "include/editors/markdowneditor.h"
#include "include/editors/mindmap.h"
#include "include/editors/texteditor.h"
#include "include/editors/calendareditor.h"
#include "include/noteTypes/uuidregistry.h"
#include "include/noteTypes/note.h"
#include "include/noteTypes/calendardata.h"
#include "include/noteTypes/imageannotationdata.h"
#include "include/subclasses/customtextbrowser.h"
#include "include/subclasses/findreplacewidget.h"

class Tests : public QObject
{
    Q_OBJECT
public:
    explicit Tests(QObject *parent = nullptr);

private slots:
    // These special names run BEFORE and AFTER all tests
    void init();
    void cleanupTestCase();

    // Document Handler Tests
    void testInitialState();
    void testSwitchEditor();
    void testFileExtensionParsing();

    // Config Menu Tests
    void testConfigMenu();

    // Calendar Config Tests
    void testCalendarConfig();

    // Editor Tests
    void testEmptyEditor();
    void testImageAnnotationEditor();
    void testMarkdownEditor();
    void testMindMap();
    void testTextEditor();
    void testCalendarEditor();

    // Subclasses Tests
    void testCustomTextBrowser();
    void testFindReplaceWidget();

    // Note Data and Structs Tests
    void testNote();
    void testImageAnnotationData();
    void testCalendarData();
    void testUuidRegistry();

private:
    QTemporaryDir *tempDir;
    QSettings *testSettings;
    QWidget *dummyWidget;
    QGridLayout *dummyLayout;
    DocumentHandler *docHandler;
    UuidRegistry *dummyRegistry = nullptr;

signals:
};

#endif // TESTS_H
