QT       += core gui
QT += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
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
    src\noteTypes\note.cpp

HEADERS += \
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
    include/subclasses/customtextbrowser.h

FORMS += \
    ui\findreplacewidget.ui \
    ui\configmenu.ui \
    ui\editors\emptyeditor.ui \
    ui\editors\htmleditor.ui \
    ui\mainwindow.ui \
    ui\editors\markdowneditor.ui \
    ui\editors\texteditor.ui

TRANSLATIONS +=
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
