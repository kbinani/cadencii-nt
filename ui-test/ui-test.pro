######################################################################
# Automatically generated by qmake (2.01a) Wed Sep 26 02:59:52 2012
######################################################################

CONFIG += qtestlib
TEMPLATE = app
TARGET = ui-test
DEPENDPATH += .
INCLUDEPATH += . ../

include(../qt-solutions/qtpropertybrowser/src/qtpropertybrowser.pri)
include(../cadencii_common.pri)

# Input
HEADERS += \
    Test.hpp

SOURCES += \
    main.cpp \
    Test.cpp

DESTDIR = bin
OBJECTS_DIR = $$DESTDIR
MOC_DIR = $$DESTDIR
RCC_DIR = $$DESTDIR
UI_DIR = $$DESTDIR