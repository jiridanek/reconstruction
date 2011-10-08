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

#include "i3dcore.h"
#include "i3d/image3d.h"
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

                i3d::Vector3d<size_t> *ns = new i3d::Vector3d<size_t>(p.GetVector().x+a, p.GetVector().y+b, 0);
                vec.push_back(*ns);
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

#include <i3d/image3d.h>

int main() {
//    uint8 ctverec[5][5] = {{1,1,1,1,1},
//                             {2,1,0,1,0},
//                             {3,0,0,0,1},
//                             {4,0,0,0,1},
//                             {5,1,1,1,1}};
//    uint8 stred[5][5] = {{1,0,0,0,0},
//                         {0,0,0,0,0},
//                         {0,0,0,0,0},
//                         {0,0,0,0,0},
//                         {0,0,0,0,0}};

//    uint8 empty[5][5] = {{0,0,0,0,0},
//                         {0,0,0,0,0},
//                         {0,0,0,0,0},
//                         {0,0,0,0,0},
//                         {0,0,0,0,0}};

    //xi3d mask(5,5,ctverec);
    //xi3d marker(5,5,stred);
    //xi3d out(5,5,empty);

   //Reconstruction_by_dilatation_downhill(marker, mask, &out);

   //out.print();
   //std::cout << std::endl;
//   mask.print();
//   std::cout << std::endl;
//   marker.print();

   i3d::Image3d<i3d::GRAY8> a("marker.jpg", 0, false, -1, 0, 0);
   i3d::Image3d<i3d::GRAY8> b("mask.jpg", 0, false, -1, 0, 0);
   i3d::Image3d<i3d::GRAY8> c("marker.jpg", 0, false, -1, 0, 0);
   c.SetAllVoxels(i3d::GRAY8(WHITE));

   Point<size_t, i3d::GRAY8> v1 (i3d::Vector3d<size_t>(1,1,1), i3d::GRAY8(5));
   Point<size_t, i3d::GRAY8> v2 (i3d::Vector3d<size_t>(1,1,1), i3d::GRAY8(6));
   //v1.GetInt();
   //cout << (v1 < v2) << endl;

    Reconstruction_by_dilatation_downhill(a, b, c);

    c.SaveImage("reconstructed.tga", i3d::IMG_TARGA, false);
}
