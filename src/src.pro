### General config ###

TARGET          = ChilloutSunday
VERSION         = 0.0.0
TEMPLATE        = app
#ICON            = resources/icon.icns
QT              += sql widgets printsupport
CONFIG          += c++11
DEFINES         += APP_VERSION=\\\"$$VERSION\\\"
QMAKE_CXXFLAGS   = -Weverything \
                   -Wno-c++98-compat \
                   -Wno-padded  \
                   -Wno-undefined-reinterpret-cast  \
                   -Wno-pragmas  \
                   -Wno-unknown-warning-option


### Info.plist ###

QMAKE_INFO_PLIST    =  resources/Info.plist
OTHER_FILES         += resources/Info.plist


### QPersistence ###

QPERSISTENCE_PATH = ../lib/QPersistence
include($$QPERSISTENCE_PATH/QPersistence.pri)

INCLUDEPATH     += $$QPERSISTENCE_INCLUDEPATH
LIBS            += $$QPERSISTENCE_LIBS
POST_TARGETDEPS += $$QPERSISTENCE_POST_TARGETDEPS


### QtAwesome ###

include(../lib/QtAwesome/QtAwesome/QtAwesome.pri)


### Files ###

SOURCES += main.cpp
FORMS +=
HEADERS +=
