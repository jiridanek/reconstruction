SOURCES += \
    main.cc \
    ReconstructionAlgorithms.cc \
    FromArticle.cc \
    Timing.cc \
    DownhillReconstruction.cc

HEADERS += \
    BucketQueue.h \
    ReconstructionAlgorithms.h \
    Point.h \
    ReservablePriorityQueue.h \
    FromArticle.h \
    Timing.h \
    intervalqueue.h \
    ipriorityqueue.h \
    hierarchicalqueue.h \
    DownhillReconstruction.h \
    hierarchicalheapqueue.h

DCMAKE_VERBOSE_MAKEFILE=ON

unix|win32: LIBS += -L/usr/local/lib/ -li3dcore -li3dalgo -lrt -fopenmp
INCLUDEPATH += /usr/local/include
DEPENDPATH += /usr/local/include

#unix|win32: LIBS += -L/home/jirka/usr/local/lib/ -li3dcore -li3dalgo -lrt
#INCLUDEPATH += /home/jirka/usr/local/include
#DEPENDPATH += /home/jirka/usr/local/include

#unix|win32: LIBS += -L/home/jirka/usr/intel/lib/ -li3dcore -li3dalgo -lrt
#INCLUDEPATH += /home/jirka/usr/intel/include
#DEPENDPATH += /home/jirka/usr/intel/include


#QMAKE_CFLAGS_RELEASE = -O3 -fomit-frame-pointer
QMAKE_CXXFLAGS_RELEASE += -O3 -fomit-frame-pointer -fopenmp
#QMAKE_LFLAGS_RELEASE =



#QMAKE_CFLAGS_DEBUG = -funroll-loops
#QMAKE_CXXFLAGS_DEBUG = -funroll-loops
#QMAKE_LFLAGS_DEBUG =






















