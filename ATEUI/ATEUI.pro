QT       += core network
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = QATE_V1.0.33
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_MESSAGELOGCONTEXT

#DEFINES += my_feature

#客户工具
#DEFINES += Release_Customer


# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cdlrun.cpp \
    cgfc.cpp \
    logondialog.cpp \
    main.cpp \
    ateuimainwindow.cpp \
    mychannelnumdlg.cpp \
    mycombobox.cpp \
    myinputmessbox.cpp \
    mymessagebox.cpp \
    mypathlossdlg.cpp \
    myquerymessagebox.cpp \
    mytoolversion.cpp \
    qlogoutput.cpp \
    tradgoldlogondialog.cpp

HEADERS += \
    AutoWidthComboBox.h \
    Global_Header.h \
    TrianglePushButton.h \
    TriangleWidget.h \
    ate_controller.h \
    ate_controller_defines.h \
    ate_controller_defines_ext.h \
    ateuimainwindow.h \
    cdlrun.h \
    cgfc.h \
    logondialog.h \
    mychannelnumdlg.h \
    mycombobox.h \
    myinputmessbox.h \
    mymessagebox.h \
    mypathlossdlg.h \
    myquerymessagebox.h \
    mytoolversion.h \
    qlogoutput.h \
    tradgoldlogondialog.h \
    ts_lan.h

TRANSLATIONS += chinese.ts english.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#RC_FILE = ate.rc
RC_ICONS = ate.ico

# 版本信息

DEFINES += APP_VERSION=\\\"$${VERSION}\\\"

# 公司名称
QMAKE_TARGET_COMPANY = ""

# 版权信息
QMAKE_TARGET_COPYRIGHT = "Copyright 2023  Ltd. All rights reserved."
QMAKE_TARGET_PRODUCTVERSION = "1.0.16"

QMAKE_LFLAGS_WINDOWS += -Wl,--stack,10000000
#QMAKE_CXXFLAGS += /Zm300

# MinGW 编译器（GCC）
# 根据编译器自动选择配置
win32 {
    !contains(QT_ARCH, i386) {
        # 64位无需处理
    } else {
        # 32位需启用大地址
        win32-msvc* {
            QMAKE_LFLAGS += /LARGEADDRESSAWARE
        }
        win32-g++* {
            QMAKE_LFLAGS += -Wl,--large-address-aware
        }
    }
}


CONFIG(release, debug|release){
RC_FILE += res.rc
}

DISTFILES += \
    res.rc


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../release/ -late_controller -lts_lan -lPsapi
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../debug/ -late_controller -lts_lan -lPsapi

INCLUDEPATH += $$PWD/../Release
DEPENDPATH += $$PWD/../Release

LIBS += \
-lUser32 \
-lDbghelp

