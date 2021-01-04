#-------------------------------------------------
#
# Project created by QtCreator 2020-10-27T15:19:07
#
#-------------------------------------------------

QT       += core gui charts datavisualization uitools

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Atlantis_Availability
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
QMAKE_CXXFLAGS += -std=c++0x
LIBS += -lboost_system -lboost_filesystem

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++14

SOURCES += \
    main.cpp \
    nmfMainWindow.cpp

HEADERS += \
    nmfMainWindow.h

FORMS += \
    nmfMainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    qdarkstyle/style.qrc \
    resource.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../builds/build-nmfUtilities-Desktop_Qt_5_15_1_GCC_64bit-Release/release/ -lnmfUtilities
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../builds/build-nmfUtilities-Desktop_Qt_5_15_1_GCC_64bit-Release/debug/ -lnmfUtilities
else:unix: LIBS += -L$$PWD/../../../builds/build-nmfUtilities-Desktop_Qt_5_15_1_GCC_64bit-Release/ -lnmfUtilities

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/nmfUtilities
DEPENDPATH += $$PWD/../../nmfSharedUtilities/nmfUtilities

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../builds/build-nmfCharts-Desktop_Qt_5_15_1_GCC_64bit-Release/release/ -lnmfCharts
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../builds/build-nmfCharts-Desktop_Qt_5_15_1_GCC_64bit-Release/debug/ -lnmfCharts
else:unix: LIBS += -L$$PWD/../../../builds/build-nmfCharts-Desktop_Qt_5_15_1_GCC_64bit-Release/ -lnmfCharts

INCLUDEPATH += $$PWD/../../nmfSharedUtilities/nmfCharts
DEPENDPATH += $$PWD/../../nmfSharedUtilities/nmfCharts

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../builds/build-Atlantis_Avail_GuiSetup-Desktop_Qt_5_15_1_GCC_64bit-Release/release/ -lAtlantis_Avail_GuiSetup
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../builds/build-Atlantis_Avail_GuiSetup-Desktop_Qt_5_15_1_GCC_64bit-Release/debug/ -lAtlantis_Avail_GuiSetup
else:unix: LIBS += -L$$PWD/../../../builds/build-Atlantis_Avail_GuiSetup-Desktop_Qt_5_15_1_GCC_64bit-Release/ -lAtlantis_Avail_GuiSetup

INCLUDEPATH += $$PWD/../Atlantis_Avail_GuiSetup
DEPENDPATH += $$PWD/../Atlantis_Avail_GuiSetup

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../builds/build-Atlantis_Avail_ModelAPI-Desktop_Qt_5_15_1_GCC_64bit-Release/release/ -lAtlantis_Avail_ModelAPI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../builds/build-Atlantis_Avail_ModelAPI-Desktop_Qt_5_15_1_GCC_64bit-Release/debug/ -lAtlantis_Avail_ModelAPI
else:unix: LIBS += -L$$PWD/../../../builds/build-Atlantis_Avail_ModelAPI-Desktop_Qt_5_15_1_GCC_64bit-Release/ -lAtlantis_Avail_ModelAPI

INCLUDEPATH += $$PWD/../Atlantis_Avail_ModelAPI
DEPENDPATH += $$PWD/../Atlantis_Avail_ModelAPI

DISTFILES += \
    qdarkstyle/rc/Hmovetoolbar.png \
    qdarkstyle/rc/Hsepartoolbar.png \
    qdarkstyle/rc/Vmovetoolbar.png \
    qdarkstyle/rc/Vsepartoolbar.png \
    qdarkstyle/rc/branch_closed-on.png \
    qdarkstyle/rc/branch_closed.png \
    qdarkstyle/rc/branch_open-on.png \
    qdarkstyle/rc/branch_open.png \
    qdarkstyle/rc/checkbox_checked.png \
    qdarkstyle/rc/checkbox_checked_disabled.png \
    qdarkstyle/rc/checkbox_checked_focus.png \
    qdarkstyle/rc/checkbox_indeterminate.png \
    qdarkstyle/rc/checkbox_indeterminate_disabled.png \
    qdarkstyle/rc/checkbox_indeterminate_focus.png \
    qdarkstyle/rc/checkbox_unchecked.png \
    qdarkstyle/rc/checkbox_unchecked_disabled.png \
    qdarkstyle/rc/checkbox_unchecked_focus.png \
    qdarkstyle/rc/close-hover.png \
    qdarkstyle/rc/close-pressed.png \
    qdarkstyle/rc/close.png \
    qdarkstyle/rc/down_arrow.png \
    qdarkstyle/rc/down_arrow_disabled.png \
    qdarkstyle/rc/left_arrow.png \
    qdarkstyle/rc/left_arrow_disabled.png \
    qdarkstyle/rc/radio_checked.png \
    qdarkstyle/rc/radio_checked_disabled.png \
    qdarkstyle/rc/radio_checked_focus.png \
    qdarkstyle/rc/radio_unchecked.png \
    qdarkstyle/rc/radio_unchecked_disabled.png \
    qdarkstyle/rc/radio_unchecked_focus.png \
    qdarkstyle/rc/right_arrow.png \
    qdarkstyle/rc/right_arrow_disabled.png \
    qdarkstyle/rc/sizegrip.png \
    qdarkstyle/rc/stylesheet-branch-end.png \
    qdarkstyle/rc/stylesheet-branch-more.png \
    qdarkstyle/rc/stylesheet-vline.png \
    qdarkstyle/rc/transparent.png \
    qdarkstyle/rc/undock.png \
    qdarkstyle/rc/up_arrow.png \
    qdarkstyle/rc/up_arrow_disabled.png \
    qdarkstyle/style.qss


