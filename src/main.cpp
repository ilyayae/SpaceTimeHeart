#include "include/mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(SOLID_BACKEND, "kf.solid.backends.udisks2")
int main(int argc, char *argv[])
{

    QLoggingCategory::setFilterRules("kf.solid.backends.udisks2.debug=false\nkf.solid.backends.udisks2.warning=false");
    QApplication a(argc, argv);

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
