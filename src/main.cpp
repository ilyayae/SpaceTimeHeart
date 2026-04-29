#include "include/mainwindow.h"
#include "test/tests.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(SOLID_BACKEND, "kf.solid.backends.udisks2")
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QStringList args = QCoreApplication::arguments();
    if (args.contains("--test")) {
        Tests testObject;
        int testArgc = 1;
        char *testArgv[] = { argv[0] };
        return QTest::qExec(&testObject, testArgc, testArgv);
    }

    QLoggingCategory::setFilterRules("kf.solid.backends.udisks2.debug=false\nkf.solid.backends.udisks2.warning=false");

    a.setOrganizationName("zhopets");
    a.setApplicationName("SpaceTimeHeart");

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "SpaceTimeHeart_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    MainWindow w;
    w.setWindowTitle("Space-Time-Heart");
    w.show();
    return a.exec();
}
