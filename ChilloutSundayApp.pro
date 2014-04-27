cache()

TEMPLATE = subdirs

SUBDIRS     =   lib/QPersistence \
                lib/QSerienJunkies \
                src


QPersistence.subdir         =   lib/QPersistence
QSerienJunkies.subdir       =   lib/QSerienJunkies

src.subdir               =  src
src.depends              =  QPersistence \
                            QSerienJunkies
