SOURCES += \
    main.cc

HEADERS += \
    point.h



unix|win32: LIBS += -L/usr/local/lib/ -li3dcore -li3dalgo

INCLUDEPATH += /usr/local/include
//DEPENDPATH += /usr/local/include

CFLAGS += -Wall




