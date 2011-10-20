/*!

 */

#include <i3d/image3d.h>
#include <i3d/vector3d.h>
#include <i3d/neighbours.h>

#include <cassert>
#include <algorithm>
#include <iostream>

#include "DownhillReconstruction.h"
#include "Timing.h"
#include "ipriorityqueue.h"
#include "hierarchicalqueue.h"

using std::vector;
using std::max;
using std::abs;
using i3d::GetWindow;
using std::size_t;

template <class T> void Reconstruction_by_dillatation(i3d::Image3d<T> & MARKER, i3d::Image3d<T> & MASK, i3d::Image3d<T> & reconstructed, const i3d::Neighbourhood & NEIGHBOURHOOD) {

    T *marker_data = MARKER.begin();
    T *mask_data = MASK.begin();

    T max_value = MARKER.GetVoxelData().max();


    CHierarchicalQueue<size_t, T> q(MARKER.GetImageSize(), max_value);

    for (int i = 0; i != (int)MARKER.GetImageSize(); ++i) {
        if ( marker_data[i] != WHITE) {
            q.push(i, max(marker_data[i], mask_data[i]));
        }
    }

#ifdef NEIGHBOURHOOD_GETWINDOW
    std::vector<T *> window;
    window.reserve(NEIGHBOURHOOD.size());
#endif

    while(! q.empty()) {
        const struct IPriorityQueue<size_t, T>::data_priority_t datapriority =
                 q.top();
        const size_t index = datapriority.value;
        const T intensity = datapriority.priority;

        q.pop();

        reconstructed.SetVoxel(index, (T)intensity);

        const size_t x = reconstructed.GetX(index);
        const size_t y = reconstructed.GetY(index);
        const size_t z = reconstructed.GetZ(index);

#ifdef NEIGHBOURHOOD_GETWINDOW
        GetWindow(MASK, x, y, z, NEIGHBOURHOOD, window);

        /// na řádku 2235 v souboru morphology.cc zde delaji cosi s ukazateli
        /// proč?
        for (typename vector<T*>::iterator it= window.begin(); it != window.end(); ++it) {
            const size_t position = (*it)-MASK.begin();
            if(!q.hasBeenDequeued(position)) {
                q.push(position, max(intensity, *(*it)));
            }
        }
#endif
#ifdef NEIGHBOURHOOD_DOITYOURSELF
        for(typename i3d::VectContainer::const_iterator it=NEIGHBOURHOOD.offset.begin(); it!=NEIGHBOURHOOD.offset.end(); ++it) {
            const i3d::Vector3d<int> neig = *it;
            size_t newx;
            size_t newy;
            size_t newz;
            if(neig.x < 0) {
                if(abs(neig.x) > x) {
                    continue;
                } else {
                    newx = x+neig.x;
                }
            } else {
                /// neig.x >= 0
                if(neig.x + x > MASK.GetWidth()-1) {
                    continue;
                } else {
                    newx = x+neig.x;
                }
            }
            if(neig.y < 0) {
                if(abs(neig.y) > y) {
                    continue;
                } else {
                    newy = y+neig.y;
                }
            } else {
                /// neig.y >= 0
                if(neig.y + y > MASK.GetHeight()-1) {
                    continue;
                } else {
                    newy = y+neig.y;
                }
            }
            if(neig.z < 0) {
                if(abs(neig.z) > z) {
                    continue;
                } else {
                    newz = z+neig.z;
                }
            } else {
                /// neig.z >= 0
                if(neig.z + z > MASK.GetSizeZ()-1) {
                    continue;
                } else {
                    newz = z+neig.z;
                }
            }

            size_t position = index+neig.x + neig.y*MASK.GetWidth() + neig.z*MASK.GetHeight()*MASK.GetWidth();
            //size_t position = MASK.GetIndex(newx, newy, newz);
            /// checking q.getPriority(position) != neigh_intensity here only slows it down
            if(!q.hasBeenDequeued(position)) {
                q.push(position, max(intensity, mask_data[position]));
            }
        }
#endif
#ifdef NEIGHBOURHOOD_4IFTREE
        for (int a = -1; a <= 1; a++) {
            for (int b = -1; b <= 1; b++) {
                if (std::abs(a) == std::abs(b)) {
                    continue;
                }

                if (a == 0) {
                    if (y == 0 && b == -1) {
                        continue;
                    }

                    if (y + b >= (int)reconstructed.GetHeight() ) {
                        continue;
                    }

                }

                if (b == 0) {
                    if (x == 0 && a == -1) {
                        continue;
                    }

                    if (x + a >= (int)reconstructed.GetWidth()  ) {
                        continue;
                    }
                }
                size_t nx = x + a;
                size_t ny = y + b;
                int nindex = reconstructed.GetIndex(nx, ny, 0);
                if(!q.hasBeenDequeued(nindex)) {
                    q.push(nindex, max(intensity, mask_data[nindex]));
                }
            }
        }
#endif
    }
}

void timeAlg3() {
    i3d::Image3d<i3d::GRAY8> a("marker.tga", 0, false, -1, 0, 0);
    i3d::Image3d<i3d::GRAY8> b("mask.tga", 0, false, -1, 0, 0);
    i3d::Image3d<i3d::GRAY8> c("marker.tga", 0, false, -1, 0, 0);
    c.SetAllVoxels(i3d::GRAY8(WHITE));

     //! Testrun, trying not to measture startup time
     Reconstruction_by_dillatation(a, b, c);
//     c.SetAllVoxels(i3d::GRAY8(WHITE));

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
             Reconstruction_by_dillatation(a, b, c);
//             //! Cleanup of the output image
//             c.SetAllVoxels(i3d::GRAY8(WHITE));
         }

         err = clock_gettime(CLOCK_REALTIME, &stopwatch_stop);
         assert(err == 0);

         stopwatch_elapsed = diff(stopwatch_start, stopwatch_stop);
         std::cout << stepper << ": " << stopwatch_elapsed.tv_sec << "." << stopwatch_elapsed.tv_nsec << std::endl;
      }

//     /// now we won't erase this so we can check it was still doing what it should
     Reconstruction_by_dillatation(a, b, c, i3d::nb2D_4);
     c.SaveImage("reconstructed.tga", i3d::IMG_TARGA, false);
}
