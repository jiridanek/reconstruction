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

#include <i3d/image3d.h>
#include <i3d/vector3d.h>
//#ifndef RESERVABLE_PRIORITY_QUEUE
#include <queue>
//#else
#include "ReservablePriorityQueue.h"
//#endif
#include <cassert>

#include "ReconstructionAlgorithms.h"
#include "Point.h"
#include "Timing.h"

//! Implements morphological reconstruction by dilatation using unoptimized Downhill algorithm
/*!
  This function requires the output image to be completely white in the beginning

  1) Goes through all voxels in marker and enqueues all pixels with intensity != 0
  2) While queue is not empty
    1) values in reconstruction are set and the neighbouring pixels are enqueued
  \param marker
  \param mask
  \param reconstruction resulting image
  */
void Reconstruction_by_dilatation_downhill(i3d::Image3d<i3d::GRAY8> & MARKER, i3d::Image3d<i3d::GRAY8> & MASK, i3d::Image3d<i3d::GRAY8> & reconstructed) {

#ifdef DEBUG
    assert(&MARKER != &MASK);
    assert(&MARKER != &reconstructed);
    assert(&MASK != &reconstructed);

    assert(MARKER.GetWidth() == MASK.GetWidth());
    assert(MASK.GetWidth() == reconstructed.GetWidth());

    assert(MARKER.GetHeight() == MASK.GetHeight());
    assert(MASK.GetHeight() == reconstructed.GetHeight());

    assert(MARKER.GetNumSlices() == MASK.GetNumSlices());
    assert(MASK.GetNumSlices() == reconstructed.GetNumSlices());

    for (size_t x = 0; x < reconstructed.GetWidth(); x++) {
        for (size_t y = 0; y < reconstructed.GetHeight(); y++) {
            for (size_t z = 0; z < reconstructed.GetNumSlices(); z++) {
                // the output image needs to be empty beforehand
                assert(reconstructed.GetVoxel(x, y, z) == WHITE);
            }
        }
    }
#endif

    //Point<size_t, i3d::GRAY8> pepa;
#ifdef RESERVABLE_PRIORITY_QUEUE
    reservable_priority_queue< Point<size_t, i3d::GRAY8> > q(reconstructed.GetImageSize()); // 100: 1.8 -> 1.4
#else
    std::priority_queue< Point<size_t, i3d::GRAY8> > q;
#endif

/* Accesses the voxels with a function call */
#ifndef OPTIMIZE_IMGLOOP
    for (size_t z = 0; z < MARKER.GetNumSlices(); z++) {
        for (size_t y = 0; y < MARKER.GetHeight(); y++) {
            for (size_t x = 0; x < MARKER.GetWidth(); x++) {
                if (MARKER.GetVoxel(x, y, z) != WHITE ) {
                    q.push(Point<size_t, i3d::GRAY8>(i3d::Vector3d<size_t>(x,y,z), MARKER.GetVoxel(x, y, z)));
                    //reconstructed.SetVoxel(x, y, 0, MARKER.GetVoxel(x,y,0));
                }
            }
        }
    }
#elif defined(OPTIMIZE_IMGLOOP_UNWIND) /* OPTIMIZE_IMGLOOP */
    size_t overshot = (MARKER.end() - MARKER.begin()) % 4;
    i3d::GRAY8 *xyz;
    for (xyz = MARKER.begin(); xyz != MARKER.end() - overshot; xyz += 4) {
        if ((*xyz) != WHITE) {
            i3d::Vector3d<size_t> pos = MARKER.GetPos(xyz - MARKER.begin());
            q.push(Point<size_t, i3d::GRAY8>(pos, (*xyz)));
        }
        if ((*(xyz+1)) != WHITE) {
            i3d::Vector3d<size_t> pos = MARKER.GetPos((xyz+1) - MARKER.begin());
            q.push(Point<size_t, i3d::GRAY8>(pos, (*(xyz+1))));
        }
        if ((*(xyz+2)) != WHITE) {
            i3d::Vector3d<size_t> pos = MARKER.GetPos((xyz+2) - MARKER.begin());
            q.push(Point<size_t, i3d::GRAY8>(pos, (*(xyz+2))));
        }
        if ((*(xyz+3)) != WHITE) {
            i3d::Vector3d<size_t> pos = MARKER.GetPos((xyz+3) - MARKER.begin());
            q.push(Point<size_t, i3d::GRAY8>(pos, (*(xyz+3))));
        }
    }
    switch(overshot) {
    case 3:
        if ((*(xyz+3)) != WHITE) {
            i3d::Vector3d<size_t> pos = MARKER.GetPos((xyz+3) - MARKER.begin());
            q.push(Point<size_t, i3d::GRAY8>(pos, (*(xyz+3))));
        }
    case 2:
        if ((*(xyz+2)) != WHITE) {
            i3d::Vector3d<size_t> pos = MARKER.GetPos((xyz+2) - MARKER.begin());
            q.push(Point<size_t, i3d::GRAY8>(pos, (*(xyz+2))));
        }
    case 1:
        if ((*(xyz+1)) != WHITE) {
            i3d::Vector3d<size_t> pos = MARKER.GetPos((xyz+1) - MARKER.begin());
            q.push(Point<size_t, i3d::GRAY8>(pos, (*(xyz+1))));
        }
    case 0:
        break;
    }
#elif defined(OPTIMIZE_IMGLOOP_UNWIND_NOPOINTER)
    size_t overshot = (MARKER.end() - MARKER.begin()) % 4;
    i3d::GRAY8 *p_xyz;
    size_t v_xyz = 0;
    std::valarray<i3d::GRAY8> data = MARKER.GetVoxelData();
    for (p_xyz = MARKER.begin(); p_xyz != MARKER.end() - overshot; p_xyz += 4) {
        if ((*p_xyz) != WHITE) {
            i3d::Vector3d<size_t> pos = MARKER.GetPos(v_xyz);
            q.push(Point<size_t, i3d::GRAY8>(pos, data[v_xyz]));
        }
        if ((*(p_xyz+1)) != WHITE) {
            i3d::Vector3d<size_t> pos = MARKER.GetPos(v_xyz+1);
            q.push(Point<size_t, i3d::GRAY8>(pos, data[v_xyz+1]));
        }
        if ((*(p_xyz+2)) != WHITE) {
            i3d::Vector3d<size_t> pos = MARKER.GetPos(v_xyz+2);
            q.push(Point<size_t, i3d::GRAY8>(pos, data[v_xyz+2]));
        }
        if ((*(p_xyz+3)) != WHITE) {
            i3d::Vector3d<size_t> pos = MARKER.GetPos(v_xyz+3);
            q.push(Point<size_t, i3d::GRAY8>(pos, data[v_xyz+3]));
        }
        v_xyz += 4;
    }
    switch(overshot) {
    case 3:
        if ((*(p_xyz+3)) != WHITE) {
            i3d::Vector3d<size_t> pos = MARKER.GetPos(v_xyz+3);
            q.push(Point<size_t, i3d::GRAY8>(pos, data[v_xyz+3]));
        }
    case 2:
        if ((*(p_xyz+2)) != WHITE) {
            i3d::Vector3d<size_t> pos = MARKER.GetPos(v_xyz+2);
            q.push(Point<size_t, i3d::GRAY8>(pos, data[v_xyz+2]));
        }
    case 1:
        if ((*(p_xyz+1)) != WHITE) {
            i3d::Vector3d<size_t> pos = MARKER.GetPos(v_xyz+1);
            q.push(Point<size_t, i3d::GRAY8>(pos, data[v_xyz+1]));
        }
    case 0:
        break;
    }
#else /* OPTIMIZE_IMGLOOP */
    for (i3d::GRAY8 *xyz = MARKER.begin(); xyz != MARKER.end(); ++xyz) {
        if ((*xyz) != WHITE) {
            i3d::Vector3d<size_t> pos = MARKER.GetPos(xyz - MARKER.begin());
            q.push(Point<size_t, i3d::GRAY8>(pos, (*xyz)));
        }
    }
#endif /* OPTIMIZE_IMGLOOP */
    while (! q.empty()) {
        Point<size_t, i3d::GRAY8> p = q.top();
        q.pop();

        /*
           extended version, it might happen that we enqueued some low intensity value from marker and the place in reconstruction
           has already been set to some higher value. We could a) ensure there are no duplicities in our queue, which is something that stl queue does not support
           or each time check we are not lowering it. If we are, it means this pixel has been processed (anmd its neighbours enqueued)
          */

        if(reconstructed.GetVoxel(p.GetVector()) <= p.GetValue()) {
            continue;
        }



        reconstructed.SetVoxel(p.GetVector(), std::max(p.GetValue(), MASK.GetVoxel(p.GetVector())));

/* Creates a lot of temporary objects */
#ifndef OPTIMIZE_NEIGHBOURS
        std::vector<i3d::Vector3d<size_t> > vec;
        for (int a = -1; a <= 1; a++) {
            for (int b = -1; b <= 1; b++) {
                if (std::abs(a) == std::abs(b)) {
                    continue;
                }

                if (a == 0) {
                    if (p.GetVector().y == 0 && b == -1) {
                        continue;
                    }

                    if (p.GetVector().y + b >= reconstructed.GetHeight()) {
                        continue;
                    }

                }

                if (b == 0) {
                    if (p.GetVector().x == 0 && a == -1) {
                        continue;
                    }

                    if (p.GetVector().x + a >= reconstructed.GetWidth()) {
                        continue;
                    }
                }

                i3d::Vector3d<size_t> ns = i3d::Vector3d<size_t>(p.GetVector().x+a, p.GetVector().y+b, 0);
                vec.push_back(ns);
            }
        }

        for (std::vector<i3d::Vector3d<size_t> >::iterator it = vec.begin(); it != vec.end(); ++it) {
            if ( reconstructed.GetVoxel(*it) == WHITE && MASK.GetVoxel(*it) != WHITE) {
                q.push(Point<size_t, i3d::GRAY8>(i3d::Vector3d<size_t>((*it).x, (*it).y, (*it).z), reconstructed.GetVoxel(p.GetVector())));
            }
        }
#elif defined(OPTIMIZE_NEIGHBOURS_JUSTNOVECTOR)
        for (int a = -1; a <= 1; a++) {
            for (int b = -1; b <= 1; b++) {
                if (std::abs(a) == std::abs(b)) {
                    continue;
                }

                if (a == 0) {
                    if (p.GetVector().y == 0 && b == -1) {
                        continue;
                    }

                    if (p.GetVector().y + b > reconstructed.GetHeight() - 1) {
                        continue;
                    }

                }

                if (b == 0) {
                    if (p.GetVector().x == 0 && a == -1) {
                        continue;
                    }

                    if (p.GetVector().x + a > reconstructed.GetWidth() - 1 ) {
                        continue;
                    }
                }

                size_t x = p.GetVector().x + a;
                size_t y = p.GetVector().y + b;
                if ( reconstructed.GetVoxel(x, y, 0) == WHITE && MASK.GetVoxel(x, y, 0) != WHITE) {
                    q.push(Point<size_t, i3d::GRAY8>(i3d::Vector3d<size_t>(x, y, 0), reconstructed.GetVoxel(p.GetVector())));
                }
            }
        }
#elif defined(OPTIMIZE_NEIGHBOURS_NODYNBOUNDS)
        //! also slow, and with ints it is still so slow
        i3d::Vector3d<size_t> vec = p.GetVector(); // no point in changing this to direct access

        long long x = vec.x;
        long long y = vec.y;
        long long z = vec.z;

        /* could stay signed, but the compiler is throwing warnings*/
        long long w = reconstructed.GetWidth();
        long long h = reconstructed.GetHeight();
        long long d = reconstructed.GetNumSlices();

//        int c = 0;
        for (int a = -1; a <= 1; a++) {
            for (int b = -1; b <= 1; b++) {
                for (int c = -1; c <= 1; c++) {
                    if(x+a < 0) {
                        continue;
                    }

                    if (x+a > w) {
                        break;
                    }

                    if (y+b < 0) {
                        continue;
                    }

                    if (y+b > h) {
                        break;
                    }
                    if (z+c < 0) {
                        continue;
                    }
                    if (z+c > d) {
                        break;
                    }
                    if (reconstructed.GetVoxel(x+a,y+b,z+c) == WHITE && MASK.GetVoxel(x+a,y+b,z+c) != WHITE) {
                        q.push(Point<size_t, i3d::GRAY8>(i3d::Vector3d<size_t>(x+a, y+b, z+c), reconstructed.GetVoxel(p.GetVector())));
                    }
                }
            }
        }
#elif defined(OPTIMIZE_NEIGHBOURS_IFTREE)
        i3d::Vector3d<size_t> vec = p.GetVector(); // no point in changing this to direct access

        size_t x = vec.x;
        size_t y = vec.y;
        size_t z = vec.z;

        size_t w = reconstructed.GetWidth();
        size_t h = reconstructed.GetHeight();
        size_t d = reconstructed.GetNumSlices();

        i3d::GRAY8 currvalue = reconstructed.GetVoxel(p.GetVector()); // this helps: 100: 1.890 -> 1.830

        if(x == 0) {
            if(x != w-1) {
                if ( reconstructed.GetVoxel(x+1, y, z) == WHITE && MASK.GetVoxel(x+1, y, 0) != WHITE) {
                    q.push(Point<size_t, i3d::GRAY8>(i3d::Vector3d<size_t>(x+1, y, z), currvalue));
                }
            }
        } else if (x != w - 1) {
            if ( reconstructed.GetVoxel(x-1, y, z) == WHITE && MASK.GetVoxel(x-1, y, z) != WHITE) {
                q.push(Point<size_t, i3d::GRAY8>(i3d::Vector3d<size_t>(x-1, y, z), currvalue));
            }
            if ( reconstructed.GetVoxel(x+1, y, z) == WHITE && MASK.GetVoxel(x+1, y, z) != WHITE) {
                q.push(Point<size_t, i3d::GRAY8>(i3d::Vector3d<size_t>(x+1, y, z), currvalue));
            }
        } else if (x == w-1) {
            if ( reconstructed.GetVoxel(x-1, y, z) == WHITE && MASK.GetVoxel(x, y, z) != WHITE) {
                q.push(Point<size_t, i3d::GRAY8>(i3d::Vector3d<size_t>(x-1, y, z), currvalue));
            }
        }

        if (y == 0) {
            if (y != h-1) {
                if ( reconstructed.GetVoxel(x, y+1, z) == WHITE && MASK.GetVoxel(x, y, z) != WHITE) {
                    q.push(Point<size_t, i3d::GRAY8>(i3d::Vector3d<size_t>(x, y+1, z), currvalue));
                }
            }
        } else if (y != h-1) {
            if ( reconstructed.GetVoxel(x, y-1, z) == WHITE && MASK.GetVoxel(x, y-1, z) != WHITE) {
                q.push(Point<size_t, i3d::GRAY8>(i3d::Vector3d<size_t>(x, y-1, z), currvalue));
            }
            if ( reconstructed.GetVoxel(x, y+1, z) == WHITE && MASK.GetVoxel(x, y+1, z) != WHITE) {
                q.push(Point<size_t, i3d::GRAY8>(i3d::Vector3d<size_t>(x, y+1, z), currvalue));
            }
        } else if (y == h-1) {
            if ( reconstructed.GetVoxel(x, y-1, x) == WHITE && MASK.GetVoxel(x, y-1, x) != WHITE) {
                q.push(Point<size_t, i3d::GRAY8>(i3d::Vector3d<size_t>(x, y-1, z), currvalue));
            }
        }
#else /* OPTIMIZE_NEIGHBOURS */

        //! Extremely slow, about +3 sec

        i3d::Vector3d<size_t> vec = p.GetVector();
        size_t w = reconstructed.GetWidth();
        size_t h = reconstructed.GetHeight();
        size_t d = reconstructed.GetNumSlices();

        int mdx = (vec.x == 0) ? 0 : -1;
        int pdx = (vec.x == w-1) ? 0 :  1;

        int mdy = (vec.y == 0) ? 0 : -1;
        int pdy = (vec.y == h-1) ? 0 :  1;

//        int mdz = (vec.z == 0) ? 0 : -1;
//        int pdz = (vec.z == d-1) ? 0 :  1;

        size_t c = 0;

        i3d::GRAY8 currvalue = reconstructed.GetVoxel(p.GetVector());

        for (size_t a = vec.x + mdx; a <= vec.x + pdx; a++) {
            for (size_t b = vec.y + mdy; b <= vec.y + pdy; b++) {
//                for (size_t c = vec.z + mdz; c <= vec.z + pdz; c++) {
                    if (reconstructed.GetVoxel(a,b,c) == WHITE && MASK.GetVoxel(a,b,c) != WHITE) {
                        q.push(Point<size_t, i3d::GRAY8>(i3d::Vector3d<size_t>(a, b, c), currvalue));
//                    }
                }
            }
        }
#endif /* OPTIMIZE_NEIGHBOURS */
    }
    return;

}

void timeAlg1() {
    i3d::Image3d<i3d::GRAY8> a("marker.jpg", 0, false, -1, 0, 0);
    i3d::Image3d<i3d::GRAY8> b("mask.jpg", 0, false, -1, 0, 0);
    i3d::Image3d<i3d::GRAY8> c("marker.jpg", 0, false, -1, 0, 0);
    c.SetAllVoxels(i3d::GRAY8(WHITE));

    Point<size_t, i3d::GRAY8> v1 (i3d::Vector3d<size_t>(1,1,1), i3d::GRAY8(5));
    Point<size_t, i3d::GRAY8> v2 (i3d::Vector3d<size_t>(1,1,1), i3d::GRAY8(6));
    //v1.GetInt();
    //cout << (v1 < v2) << endl;

     //! Testrun, trying not to measture startup time
     Reconstruction_by_dilatation_downhill(a, b, c);
     c.SetAllVoxels(i3d::GRAY8(WHITE));

     const int BASE = 10;
     const int STEP = 10;
     const int UPTO = 1000;



     for (int stepper = BASE; stepper <= UPTO; stepper = stepper*STEP) {
         timespec stopwatch_start;
         timespec stopwatch_stop;
         timespec stopwatch_elapsed;

         int err;
         err = clock_gettime(CLOCK_REALTIME, &stopwatch_start);
         assert(err == 0);

         for (int count = 0; count < stepper; count++) {
             Reconstruction_by_dilatation_downhill(a, b, c);
             //! Cleanup of the output image
             c.SetAllVoxels(i3d::GRAY8(WHITE));
         }

         err = clock_gettime(CLOCK_REALTIME, &stopwatch_stop);
         assert(err == 0);

         stopwatch_elapsed = diff(stopwatch_start, stopwatch_stop);
         std::cout << stepper << ": " << stopwatch_elapsed.tv_sec << "." << stopwatch_elapsed.tv_nsec << std::endl;
      }

     /// now we won't erase this so we can check it was still doing what it should
     Reconstruction_by_dilatation_downhill(a, b, c);
     c.SaveImage("reconstructed.tga", i3d::IMG_TARGA, false);
}
