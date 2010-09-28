INCLUDEPATH += $$PWD/.. $$PWD/gl
DEPENDPATH += $$PWD/..

unix::QMAKE_CFLAGS_WARN_ON -= -W
win32::QMAKE_CFLAGS += -Zm400

HEADERS += $$PWD/idna.h \
    $$PWD/gunidecomp.h \
    $$PWD/gunicomp.h \
    $$PWD/stringprep.h \
    $$PWD/punycode.h \
    $$PWD/gl/gettext.h

SOURCES +=  $$PWD/stringprep.c \
    $$PWD/toutf8.c \
    $$PWD/rfc3454.c \
    $$PWD/punycode.c \
    $$PWD/profiles.c \
    $$PWD/nfkc.c \
    $$PWD/idna.c \
    $$PWD/gl/strverscmp.c

