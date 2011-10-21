/*
 * i3dlib - image manipulation library
 *
 * Copyright (C) 2000-2006   Centre for Biomedical Image Analysis (CBIA)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <i3d/neighbours.h>

#ifndef DOWNHILLRECONSTRUCTION_H
#define DOWNHILLRECONSTRUCTION_H

//#define SEQUENTIAL
    //#define NEIGHBOURHOOD_DOITYOURSELF
    //#define NEIGHBOURHOOD_GETWINDOW
    //#define NEIGHBOURHOOD_4IFTREE
#define OMP
    #define NEIGHBOURHOOD_DOITYOURSELF

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

#define OMP
    #define NEIGHBOURHOOD_DOITYOURSELF
======================================

  GCC with OpenMP:
10: 0.237430805
100: 2.346749275
1000: 23.261433564

  GCC without OpenMP:
10: 0.122169906
100: 1.212497023
1000: 12.105648149

// so in parallel it is 2x slower :(
 */

template <class T>
void Reconstruction_by_dillatation(
        i3d::Image3d<T> & MARKER
      , i3d::Image3d<T> & MASK
      , i3d::Image3d<T> & reconstructed
      , const struct i3d::Neighbourhood & NEIGHBOURHOOD = i3d::nb3D_o6);
void timeAlg3();

#endif // DOWNHILLRECONSTRUCTION_H
