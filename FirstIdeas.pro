SOURCES += \
    main.cc

HEADERS += \
    point.h \
    BucketQueue.h



unix|win32: LIBS += -L/usr/local/lib/ -li3dcore -li3dalgo -lrt

INCLUDEPATH += /usr/local/include
//DEPENDPATH += /usr/local/include

//QMAKE_CFLAGS_RELEASE = -O3 -fomit-frame-pointer
//QMAKE_CXXFLAGS_RELEASE = -O3 -fomit-frame-pointer
//QMAKE_LFLAGS_RELEASE =
