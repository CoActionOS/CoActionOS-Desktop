#-------------------------------------------------
#
# Project created by QtCreator 2010-11-25T12:14:12
#
#-------------------------------------------------


QT       += widgets
TARGET = "CoActionOS-Link"
TEMPLATE = app

DEFINES += __link
win32:DEFINES += __WINDOWS __win32
win64:DEFINES += __WINDOWS __win64
win32:QMAKE_CXXFLAGS=-mno-ms-bitfields
win64:QMAKE_CXXFLAGS=-mno-ms-bitfields


SOURCES += main.cpp\
        caoslink.cpp \
    CaosInterface.cpp \
    CaosTerminal.cpp \
    About.cpp \
    Monitor.cpp \
    Preferences.cpp

HEADERS  += caoslink.h \
    CaosInterface.h \
    CaosTerminal.h \
    About.h \
    Monitor.h \
    Preferences.h

FORMS    += caoslink.ui \
    KernelBuilder.ui \
    ApplicationBuilder.ui \
    CaosTerminal.ui \
    KernelBuilderFeatures.ui \
    About.ui \
    Monitor.ui \
    Preferences.ui \
    SystemImage.ui \
    ConsoleOutput.ui \
    Updater.ui \
    CaosInterface.ui


macx:ICON = coactionoslink.icns

unix {
    INSTALLPATH = /usr/local/CoActionOS
    INCLUDEPATH = /usr/local/include
}

win32 {
    INSTALLPATH = c:/CoActionOS-Compiler/CoActionOS
    RC_FILE += app.rc
}

LIBS += -L$$INSTALLPATH/lib -lCSdk -lapp_CoActionOS_link -lcaos_link

INCLUDEPATH += $$INSTALLPATH/include

OTHER_FILES += \
    symbols.txt \
    CStyle.css \
    fontawesome-webfont.ttf \
    CStyle-win32.css \
    fontawesome-ext-webfont.ttf

RESOURCES += \
    resources.qrc

