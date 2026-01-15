QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    configmenu.cpp \
    documenthandler.cpp \
    emptyeditor.cpp \
    filebrowser.cpp \
    htmleditor.cpp \
    main.cpp \
    mainwindow.cpp \
    markdowneditor.cpp \
    texteditor.cpp

HEADERS += \
    configmenu.h \
    documenthandler.h \
    emptyeditor.h \
    filebrowser.h \
    htmleditor.h \
    mainwindow.h \
    markdowneditor.h \
    texteditor.h

FORMS += \
    configmenu.ui \
    emptyeditor.ui \
    htmleditor.ui \
    mainwindow.ui \
    markdowneditor.ui \
    texteditor.ui

TRANSLATIONS += \
    SpaceTimeHeart_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
