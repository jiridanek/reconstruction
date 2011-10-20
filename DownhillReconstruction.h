#include <i3d/neighbours.h>

#ifndef DOWNHILLRECONSTRUCTION_H
#define DOWNHILLRECONSTRUCTION_H

template <class T>
void Reconstruction_by_dillatation(
        i3d::Image3d<T> & MARKER
      , i3d::Image3d<T> & MASK
      , i3d::Image3d<T> & reconstructed
      , const struct i3d::Neighbourhood & NEIGHBOURHOOD = i3d::nb3D_o6);
void timeAlg3();

#endif // DOWNHILLRECONSTRUCTION_H
