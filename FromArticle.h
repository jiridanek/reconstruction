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

/** \file FromArticle.h
  \brief



  \author Jiří Daněk (dnk@mail.muni.cz) 2011

*/

#ifndef FROMARTICLE_H
#define FROMARTICLE_H

//const int WHITE = 255;
//const int BLACK = 0;

//#define BUCKET_QUEUE
//#define RESERVABLE_PRIORITY_QUEUE
#define HIERARCHICAL_PRIORITY_QUEUE

template <class T> void Reconstruction_by_dillatation_bucket(i3d::Image3d<T> & MARKER, i3d::Image3d<T> & MASK, i3d::Image3d<T> & reconstructed);
void timeAlg2();

/**

  Testy (obrázek GRAY8, 256x256)

    10: 0.28404688
    100: 0.281822065
    1000: 2.818387677
    10000: 28.168290442

opět si zatím ještě neporadí s nemezními hodnotami v markeru

při použití CHierarchicalQueue (odpovídající #define) vypadají časy
10: 0.86572095
100: 0.857521089
1000: 8.574211764
10000: 86.38366410
tedy ~4x tolik operací pro každý voxel, což asi docela sedí
 */

#endif // FROMARTICLE_H
