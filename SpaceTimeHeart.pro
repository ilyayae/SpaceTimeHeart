QT       += core gui
QT += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/noteTypes/imageannotationdata.cpp \
    src/editors/calendareditor.cpp \
    src/editors/imageannotationeditor.cpp \
    src/calendarconfig.cpp \
    src/noteTypes/calendardata.cpp \
    src/subclasses/customtextbrowser.cpp \
    src/subclasses/findreplacewidget.cpp \
    src/noteTypes/uuidregistry.cpp \
    src\configmenu.cpp \
    src\documenthandler.cpp \
    src\editors\emptyeditor.cpp \
    src\filebrowser.cpp \
    src\editors\htmleditor.cpp \
    src\main.cpp \
    src\mainwindow.cpp \
    src\editors\markdowneditor.cpp \
    src/subclasses/searchhighlighter.cpp \
    src\editors\texteditor.cpp \
    src\noteTypes\note.cpp \
    src/listEntries/dayinweekentry.cpp \
    src/listEntries/monthinyearentry.cpp \
    src/listEntries/moonentry.cpp \
    src/calendarObjects/dayslot.cpp \
    src/calendarObjects/linkinday.cpp \
    src/calendarObjects/moonvisualiser.cpp

HEADERS += \
    include/calendarconfig.h \
    include/editors/calendareditor.h \
    include/editors/imageannotationeditor.h \
    include/noteTypes/imageannotationdata.h \
    include/subclasses/findreplacewidget.h \
    include/noteTypes/uuidregistry.h \
    include\configmenu.h \
    include\documenthandler.h \
    include\editors\emptyeditor.h \
    include\filebrowser.h \
    include\editors\htmleditor.h \
    include\mainwindow.h \
    include\editors\markdowneditor.h \
    include/subclasses/searchhighlighter.h \
    include\editors\texteditor.h \
    include\noteTypes\note.h \
    include/subclasses/customtextbrowser.h \
    include/noteTypes/calendardata.h \
    include/listEntries/dayinweekentry.h \
    include/listEntries/monthinyearentry.h \
    include/listEntries/moonentry.h \
    include/calendarObjects/dayslot.h \
    include/calendarObjects/linkinday.h \
    include/calendarObjects/moonvisualiser.h

FORMS += \
    ui/calendarObjects/moonvisualiser.ui \
    ui/calendarObjects/dayslot.ui \
    ui/editors/calendareditor.ui \
    ui/editors/imageannotationeditor.ui \
    ui/calendarconfig.ui \
    ui/filebrowser.ui \
    ui/calendarObjects/linkinday.ui \
    ui/listEntries/dayinweekentry.ui \
    ui/listEntries/monthinyearentry.ui \
    ui/listEntries/moonentry.ui \
    ui\findreplacewidget.ui \
    ui\configmenu.ui \
    ui\editors\emptyeditor.ui \
    ui\editors\htmleditor.ui \
    ui\mainwindow.ui \
    ui\editors\markdowneditor.ui \
    ui\editors\texteditor.ui \

TRANSLATIONS +=
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
