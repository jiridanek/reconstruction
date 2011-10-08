/*
  Paralelně: v každém cyklu vždycky provést pixely s nejvyšší prioritou z fronty. Možná taky trochu iterativně - klidně některou hodnotu přepisovat vícekrát
  iterativní djikstrův algoritmus z článku, zkusit blokování (může být těžké) a rozbalování smyček (mělo by být easy)
  */

/*!
  \todo FIND OUT:   const int WHITE?
                    templating of this method
 */

#include <cstring>
#include <queue>
#include <algorithm>
#include <iostream>
#include <cstdio>
#include <cassert>
/// to time the algorithms
#include <ctime>

#include <i3d/image3d.h>
#include "point.h"

using namespace std;
#define DEBUG

const int WHITE = 255;
const int BLACK = 0;

//! Implements morphological reconstruction by dilatation using unoptimized Downhill algorithm
/*!
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
    std::priority_queue< Point<size_t, i3d::GRAY8> > q;

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

//            //reconstruction->setVoxel(0,0,5);
        }
//        //reconstruction->print();
    }
    return;

}

timespec diff(timespec start, timespec end)
{
        timespec temp;
        if ((end.tv_nsec-start.tv_nsec)<0) {
                temp.tv_sec = end.tv_sec-start.tv_sec-1;
                temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
        } else {
                temp.tv_sec = end.tv_sec-start.tv_sec;
                temp.tv_nsec = end.tv_nsec-start.tv_nsec;
        }
        return temp;
}

int main() {

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
        for (int count = 0; count < stepper; count++) {
            Reconstruction_by_dilatation_downhill(a, b, c);
            //! Cleanup of the output image
            c.SetAllVoxels(i3d::GRAY8(WHITE));
        }
        err = clock_gettime(CLOCK_REALTIME, &stopwatch_stop);
        stopwatch_elapsed = diff(stopwatch_start, stopwatch_stop);
        std::cout << stepper << ": " << stopwatch_elapsed.tv_sec << "." << stopwatch_elapsed.tv_nsec << endl;
     }

    /// now we won't erase this so we can check it was still doing what it should
    Reconstruction_by_dilatation_downhill(a, b, c);
    c.SaveImage("reconstructed.tga", i3d::IMG_TARGA, false);
}
