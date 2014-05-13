cache()

QuunRar.subdir              =   lib/QuunRar
QuunRar.depends             =
QPersistence.subdir         =   lib/QPersistence
QPersistence.depends        =
QSerienJunkies.subdir       =   lib/QSerienJunkies
QSerienJunkies.depends      =
src.subdir                  =   src
src.depends                 =   QuunRar \
                                QPersistence \
                                QSerienJunkies

TEMPLATE = subdirs
SUBDIRS     =   QuunRar \
                QPersistence \
                QSerienJunkies \
                src

