#-------------------------------------------------
#
# Project created by QtCreator 2012-11-19T12:41:09
#
#-------------------------------------------------

QT       += widgets
TEMPLATE = app
DEFINES += __link


TARGET = CoActionOS-ISP

SOURCES += main.cpp\
        CaosIsp.cpp \
    IspTerminal.cpp \
    Preferences.cpp \
    Settings.cpp \
    Installer.cpp

HEADERS  += CaosIsp.h \
    IspTerminal.h \
    Preferences.h \
    Settings.h \
    Installer.h

FORMS    += CaosIsp.ui \
    IspTerminal.ui \
    Preferences.ui \
    Installer.ui \
    Console.ui

macx {
    ICON = ./CoActionOS-ISP-Logo.icns
    INCLUDEPATH = /usr/local/CoActionOS/include /usr/local/include
    LIBS += -L/usr/local/CoActionOS/lib
}

win32 {
    LIBS += -Lc:/CoActionOS/msys/1.0/local/CoActionOS/lib
    RC_FILE += app.rc
    INCLUDEPATH += c:/CoActionOS/msys/1.0/local/CoActionOS/include
}

LIBS += -lapp_CoActionOS_link -lCSdk -lcaos_link

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    CStyle.css \
    fontawesome-webfont.ttf \
    CoActionOS-ISP-Logo.ico \
    CoActionOS-ISP-Logo.icns
