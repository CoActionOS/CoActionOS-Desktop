#-------------------------------------------------
#
# Project created by QtCreator 2012-11-17T14:00:48
#
#-------------------------------------------------

QT += widgets

TARGET = CSdk
TEMPLATE = lib
CONFIG += staticlib
win32:DEFINES += __WINDOWS __win32
win64:DEFINES += __WINDOWS __win64
win32:QMAKE_CXXFLAGS=-mno-ms-bitfields
win64:QMAKE_CXXFLAGS=-mno-ms-bitfields

DEFINES += CSDK_LIBRARY __link

SOURCES += CSdk.cpp \
    CTerminal.cpp \
    CLineEdit.cpp \
    CNotify.cpp \
    CFileBrowser.cpp \
    CConnect.cpp \
    CSettings.cpp \
    CIconLabel.cpp \
    CCompress.cpp \
    CApplicationInstaller.cpp \
    CInstaller.cpp \
    CApplicationSettings.cpp \
    CKernelInstaller.cpp \
    CKernelSettings.cpp \
    CCheckbox.cpp \
    CLabel.cpp \
    CLock.cpp \
    CNotification.cpp \
    CDebug.cpp \
    CTextEdit.cpp \
    CListWidget.cpp \
    CIsp.cpp

HEADERS += CSdk.h\
    CSdk_global.h \
    CFont.h \
    CLink.h \
    CTerminal.h \
    CLineEdit.h \
    CNotify.h \
    CFileBrowser.h \
    CConnect.h \
    CSettings.h \
    CIconLabel.h \
    CCompress.h \
    CApplicationInstaller.h \
    CInstaller.h \
    CApplicationSettings.h \
    CKernelInstaller.h \
    CKernelSettings.h \
    CLinkPtr.h \
    CCheckbox.h \
    CLabel.h \
    CLock.h \
    CNotification.h \
    CDebug.h \
    CTextEdit.h \
    CListWidget.h \
    CIsp.h \
    CEventFilter.h

FORMS += \
    CTerminal.ui \
    CLineEdit.ui \
    CNotify.ui \
    CFileBrowser.ui \
    CConnect.ui \
    CIconLabel.ui \
    CApplicationInstaller.ui \
    CInstaller.ui \
    CKernelInstaller.ui \
    CNotification.ui \
    CDebug.ui \
    CIsp.ui

OTHER_FILES += \
    fontawesome-webfont.ttf \
    CStyle.css

unix {
    INSTALLBASE = /usr/local/CoActionOS
    QMAKE_INSTALL_FILE = install -m 644 -p
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,$$INSTALLBASE/lib/
}

win32 {
    INSTALLBASE = c:/CoActionOS-Compiler/CoActionOS
}

target.path = $$INSTALLBASE/lib
headers.path = $$INSTALLBASE/include/CSdk
doc.path = $$INSTALLBASE/share/doc/CSdk

headers.files = $$HEADERS
doc.files = $$OTHER_FILES

INCLUDEPATH += $$INSTALLBASE/include $$_PRO_FILE_PWD_/..
LIBS += -L $$INSTALLBASE/lib -lcaos_link
INSTALLS = target headers doc



