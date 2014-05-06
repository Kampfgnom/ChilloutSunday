### General config ###

TARGET          = ChilloutSunday
VERSION         = 0.0.0
TEMPLATE        = app
#ICON            = resources/icon.icns
QT              += sql widgets printsupport network
CONFIG          += c++11
DEFINES         += APP_VERSION=\\\"$$VERSION\\\"
QMAKE_CXXFLAGS   = \
                   -Wno-c++98-compat \
                   -Wno-padded  \
                   -Wno-undefined-reinterpret-cast  \
                   -Wno-pragmas  \
                   -Wno-unknown-warning-option
LIBPATH = ../lib


### Info.plist ###

QMAKE_INFO_PLIST    =  resources/Info.plist
OTHER_FILES         += resources/Info.plist


### QPersistence ###

QPERSISTENCE_PATH = $$LIBPATH/QPersistence
include($$QPERSISTENCE_PATH/QPersistence.pri)
LIBS            += $$QPERSISTENCE_LIBS
INCLUDEPATH     += $$QPERSISTENCE_INCLUDEPATH
POST_TARGETDEPS += $$QPERSISTENCE_POST_TARGETDEPS

### QSerienJunkies ###

QSERIENJUNKIES_PATH = $$LIBPATH/QSerienJunkies
include($$QSERIENJUNKIES_PATH/QSerienJunkies.pri)
LIBS            += $$QSERIENJUNKIES_LIBS
INCLUDEPATH     += $$QSERIENJUNKIES_INCLUDEPATH
POST_TARGETDEPS += $$QSERIENJUNKIES_POST_TARGETDEPS


### QtAwesome ###

QTAWESOME_PATH = $$LIBPATH/QtAwesome/QtAwesome
include($$QTAWESOME_PATH/QtAwesome.pri)


### Files ###

SOURCES += main.cpp \
    ui/mainwindow.cpp \
    ui/downloads/downloadspage.cpp \
    database.cpp \
    ui/downloads/packagesmodel.cpp \
    ui/downloads/packagedelegate.cpp \
    model/downloadpart.cpp \
    model/downloadpackage.cpp \
    ui/downloads/partsmodel.cpp

FORMS += \
    ui/mainwindow.ui \
    ui/downloads/downloadspage.ui

HEADERS += \
    ui/mainwindow.h \
    ui/downloads/downloadspage.h \
    database.h \
    ui/downloads/packagesmodel.h \
    ui/downloads/packagedelegate.h \
    model/downloadpart.h \
    model/downloadpackage.h \
    ui/downloads/partsmodel.h

