QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    frmmain.cpp \
    psBlockingTcpServer.cpp \
    psTcpServer.cpp \
    trendingThread.cpp

HEADERS += \
    frmmain.h \
    psBlockingTcpServer.h \
    psTcpServer.h \
    trendingThread.h

FORMS += \
    frmmain.ui

#QWT
unix {
LIBS += -lqwt-qt5
INCLUDEPATH += /usr/include/qwt
} win32 {
LIBS += -L../3rd-party/qwt-6.1.4/lib/ -lqwt
#LIBS += "../3rd-party/qwt-6.1.4/lib/libqwtd.a"
#LIBS += "../../../3rd-party/qwt-6.1.4/lib/qwtd.dll"
#LIBS += "../3rd-party/qwt-6.1.4/lib/qwt.dll"
INCLUDEPATH += "../3rd-party/qwt-6.1.4/include/qwt"
INCLUDEPATH += "../3rd-party/qwt-6.1.4/include"
}


unix:{
DESTDIR = ../bin/linux/
} win32:{
DESTDIR = ../bin/win32/
}


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    img.qrc
