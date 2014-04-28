cache()

QPersistence.subdir         =   lib/QPersistence
QPersistence.depends        =
QSerienJunkies.subdir       =   lib/QSerienJunkies
QSerienJunkies.depends      =
src.subdir                  =  src
src.depends                 =   QPersistence \
                                QSerienJunkies

TEMPLATE = subdirs
SUBDIRS     =   QPersistence \
                QSerienJunkies \
                src

