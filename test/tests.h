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
#include "include/mindMapObjects/mindmapnode.h"
#include "include/mindMapObjects/mindmapedge.h"
#include "include/mindMapObjects/mindmapview.h"
#include <QGraphicsSceneMouseEvent>
#include "include/listEntries/monthinyearentry.h"
#include "include/listEntries/moonentry.h"
#include "include/listEntries/dayinweekentry.h"
#include "include/calendarObjects/dayslot.h"
#include "include/calendarObjects/linkinday.h"
#include "include/calendarObjects/moonvisualiser.h"
#include <QMouseEvent>
#include "include/imageAnnotationObjects/customgraphicsview.h"
#include "include/imageAnnotationObjects/markeritem.h"
#include "include/imageAnnotationObjects/shapegraphicsobject.h"
#include "include/imageAnnotationObjects/shapeinprogress.h"
#include "include/imageAnnotationObjects/vectorpaintercommands.h"
#include "include/imageAnnotationObjects/pointhandle.h"
#include <QUndoStack>
#include <QGraphicsRectItem>
#include <QScrollBar>

class Tests : public QObject
{
    Q_OBJECT
public:
    explicit Tests(QObject *parent = nullptr);

private slots:
    void init();
    void cleanupTestCase();
    void testInitialState();
    void testSwitchEditor();
    void testFileExtensionParsing();
    void testConfigMenu();
    void testCalendarConfig();
    void testEmptyEditor();
    void testImageAnnotationEditor();
    void testMarkdownEditor();
    void testMindMap();
    void testTextEditor();
    void testCalendarEditor();
    void testCustomTextBrowser();
    void testFindReplaceWidget();
    void testNote();
    void testImageAnnotationData();
    void testCalendarData();
    void testUuidRegistry();
    void testMindMapNode();
    void testMindMapEdge();
    void testMindMapView();
    void testMonthInYearEntry();
    void testMoonEntry();
    void testDayInWeekEntry();
    void testDaySlot();
    void testLinkInDay();
    void testMoonVisualiser();
    void testCustomGraphicsView();
    void testMarkerItem();
    void testVectorPainterCommands();

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
