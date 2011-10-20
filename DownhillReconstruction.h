#include <i3d/neighbours.h>

#ifndef DOWNHILLRECONSTRUCTION_H
#define DOWNHILLRECONSTRUCTION_H

//#define NEIGHBOURHOOD_DOITYOURSELF
//#define NEIGHBOURHOOD_GETWINDOW
#define NEIGHBOURHOOD_4IFTREE

/**
  NEIGHBOURHOOD_DOITYOURSELF
  ==========================


  GCC:
10: 0.105658734
100: 1.43132715
1000: 10.429082062

  INTEL:
10: 0.99927179
100: 1.41155
1000: 9.999664731
 */

template <class T>
void Reconstruction_by_dillatation(
        i3d::Image3d<T> & MARKER
      , i3d::Image3d<T> & MASK
      , i3d::Image3d<T> & reconstructed
      , const struct i3d::Neighbourhood & NEIGHBOURHOOD = i3d::nb3D_o6);
void timeAlg3();

#endif // DOWNHILLRECONSTRUCTION_H
