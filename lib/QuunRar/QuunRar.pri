isEmpty(QUUNRAR_PATH) {
    error(You have to set QUUNRAR_PATH to the path of QUUNRAR)
}

QUUNRAR_TARGET           = quunrar
QUUNRAR_VERSION          = 0.1.0
QUUNRAR_INCLUDEPATH      = $$PWD/include
QUUNRAR_LIBS             = -L$$QUUNRAR_PATH/src -l$$QUUNRAR_TARGET -L$$QUUNRAR_PATH/lib/unrar -lunrar
QUUNRAR_POST_TARGETDEPS  = $$QUUNRAR_PATH/src/lib$${QUUNRAR_TARGET}.a
