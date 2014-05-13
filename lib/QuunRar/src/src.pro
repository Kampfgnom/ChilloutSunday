QUUNRAR_PATH = ..
include($$QUUNRAR_PATH/QuunRar.pri)

### General config ###

TARGET          = $$QUUNRAR_TARGET
VERSION         = $$QUUNRAR_VERSION
TEMPLATE        = lib
QT              +=
QT              -= gui
CONFIG          += static c++11 warn_on
INCLUDEPATH     += $$QUUNRAR_INCLUDEPATH


### Unrar DLL ###

DEFINES += RARDLL _UNIX SILENT
INCLUDEPATH += $$QUUNRAR_PATH/lib
LIBS += -L$$QUUNRAR_PATH/lib/unrar -Lunrar
POST_TARGETDEPS += $$QUUNRAR_PATH/lib/unrar/libunrar.a

### Files ###

HEADERS += \
    quunrar.h \
    quunrarjob.h \
    quunrarfileinfo.h \
    quunrar_p.h

SOURCES += \
    quunrar.cpp \
    quunrarjob.cpp \
    quunrarfileinfo.cpp
