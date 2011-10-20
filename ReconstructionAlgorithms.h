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

/** \file ReconstructionAlgorithms.h
  \brief



  \author Jiří Daněk (dnk@mail.muni.cz) 2011

*/

#ifndef RECONSTRUCTIONALGORITHMS_H
#define RECONSTRUCTIONALGORITHMS_H

#include <i3d/image3d.h>

//#define DEBUG
//#define RESERVABLE_PRIORITY_QUEUE
#define OPTIMIZE_IMGLOOP
//    #define OPTIMIZE_IMGLOOP_UNWIND
//    #define OPTIMIZE_IMGLOOP_UNWIND_NOPOINTER
#define OPTIMIZE_NEIGHBOURS
//    #define OPTIMIZE_NEIGHBOURS_JUSTNOVECTOR
//    #define OPTIMIZE_NEIGHBOURS_NODYNBOUNDS
    #define OPTIMIZE_NEIGHBOURS_IFTREE

/**
  vybral jsem typické výsledky na mém počítači:

  parametry gcc: -O3 -fomit-frame-pointer
  při použití kompilátoru od Intelu byly časy srovnatelné, občas dokonce trochu horší, možná
  je potřeba ho nějak lépe nastavit

bez optimalizací:
    10: 0.189506235
    100: 1.915067138

RESERVABLE_PRIORITY_QUEUE:
    10: 0.177340154
    100: 1.762260763

OPTIMIZE_IMGLOOP:
    10: 0.185237198
    100: 1.864575407
OPTIMIZE_IMGLOOP - OPTIMIZE_IMGLOOP_UNWIND
    10: 0.185019599
    100: 1.838493481

OPTIMIZE_IMGLOOP - OPTIMIZE_IMGLOOP_UNWIND_NOPOINTER
    10: 0.184869524
    100: 1.835944705

OPTIMIZE_NEIGHBOURS
    10: 0.347464042
    100: 3.469271403

OPTIMIZE_NEIGHBOURS - OPTIMIZE_NEIGHBOURS_JUSTNOVECTOR
    10: 0.126946920
    100: 1.266595963

OPTIMIZE_NEIGHBOURS - OPTIMIZE_NEIGHBOURS_NODYNBOUNDS
    10: 0.506633655
    100: 5.63499414

OPTIMIZE_NEIGHBOURS - OPTIMIZE_NEIGHBOURS_IFTREE
    10: 0.110668517
    100: 1.103565404

OPTIMIZE_NEIGHBOURS - OPTIMIZE_NEIGHBOURS_IFTREE a zároveň
OPTIMIZE_IMGLOOP
    10: 0.109330177
    100: 1.90798996
    1000: 10.889949904

co mě překvapilo byl jednak výsledek u toho iftree, protože byl dobrý, a potom ty dva, kde se čas
pro 100násobné opakování vyšplhal na několik sekund. Také je zajímavé, že při zapnutí
dvou těchto alternativ se celkový čas deseti běhů zrychlí a sta zpomalí, ale 1000 zase vypadá dobře
(a to je stabilní výsledek).

Součástí každého běhu algoritmu je tady i smazání výstupního obrázku.
A toto funguje jen pro 2d obrázky, se sousedností 4 (napevno) a pixely v markeru musí být buď
bílé, nebo mít hodnoty těch v masce (neumím mazat z této prioritní fronty)
*/

void Reconstruction_by_dilatation_downhill(i3d::Image3d<i3d::GRAY8> & MARKER, i3d::Image3d<i3d::GRAY8> & MASK, i3d::Image3d<i3d::GRAY8> & reconstructed);
void timeAlg1();

#endif // RECONSTRUCTIONALGORITHMS_H
