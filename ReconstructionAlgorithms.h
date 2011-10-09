#ifndef RECONSTRUCTIONALGORITHMS_H
#define RECONSTRUCTIONALGORITHMS_H

#include <i3d/image3d.h>

//#define DEBUG
//#define RESERVABLE_PRIORITY_QUEUE
//#define OPTIMIZE_IMGLOOP
//    //#define OPTIMIZE_IMGLOOP_UNWIND
//    //#define OPTIMIZE_IMGLOOP_UNWIND_NOPOINTER
//#define OPTIMIZE_NEIGHBOURS
//    //#define OPTIMIZE_NEIGHBOURS_JUSTNOVECTOR
//    //#define OPTIMIZE_NEIGHBOURS_NODYNBOUNDS
//    #define OPTIMIZE_NEIGHBOURS_IFTREE

void Reconstruction_by_dilatation_downhill(i3d::Image3d<i3d::GRAY8> & MARKER, i3d::Image3d<i3d::GRAY8> & MASK, i3d::Image3d<i3d::GRAY8> & reconstructed);
void timeAlg1();

#endif // RECONSTRUCTIONALGORITHMS_H
