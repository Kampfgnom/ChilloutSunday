TEMPLATE = subdirs

unrar.subdir    =   lib/unrar
unrar.depends   =
src.subdir      =   src
src.depends     =   unrar

SUBDIRS = unrar src
