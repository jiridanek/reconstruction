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

#include <omp.h>

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

using std::size_t;
using std::vector;
using i3d::Image3d;
using i3d::Neighbourhood;

using std::max;
using std::abs;
using i3d::GetWindow;

template <class T> void Reconstruction_by_dillatation(
        Image3d<T> & MARKER
      , Image3d<T> & MASK
      , Image3d<T> & reconstructed
      , const Neighbourhood & NEIGHBOURHOOD) {

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
#ifdef SEQUENTIAL
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
                if (abs(a) == abs(b)) {
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
#endif /* SEQUENTIAL */
#ifdef OMP
        size_t *currPrioritySlice;
        size_t noValues;
        T currPriority;
        q.getPrioritySlice(&currPrioritySlice, &noValues, &currPriority);

        /// there should be one vector per thread
        //vector<typename IPriorityQueue<size_t, T>::data_priority_t> neighboursToVisit;
        vector<size_t> neighboursToVisit;
        // lets take all the memory we could possibly need right now
        neighboursToVisit.reserve(noValues*NEIGHBOURHOOD.offset.size());

        //copied the schedule clause from http://software.intel.com/en-us/articles/optimization-of-image-processing-algorithms-a-case-study/
        //no idea how does it do it, but happens to make it ~2 times faster
        //private (colin,colout)
#pragma omp parallel
        {
            //vector<size_t> neighboursToVisit;
            //neighboursToVisit.reserve(noValues*NEIGHBOURHOOD.offset.size()/4);
#pragma omp for schedule(dynamic, 3*MASK.GetWidth())

        for(size_t i=0; i < noValues; i++) {
            const size_t index = currPrioritySlice[i];
            // hopefully this won't cause problems, because no to threads
            // will modify the same voxel
            reconstructed.SetVoxel(currPrioritySlice[i], currPriority);
#ifdef NEIGHBOURHOOD_DOITYOURSELF

            const size_t x = reconstructed.GetX(index);
            const size_t y = reconstructed.GetY(index);
            const size_t z = reconstructed.GetZ(index);

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

                //size_t position = index+neig.x + neig.y*MASK.GetWidth() + neig.z*MASK.GetHeight()*MASK.GetWidth();
                size_t position = MASK.GetIndex(newx, newy, newz);
                /// checking q.getPriority(position) != neigh_intensity here only slows it down
                // const method, no race conditions on q
                if(!q.hasBeenDequeued(position)) {
// now this is no longer critical, one vector per thread
    #pragma omp critical
                    {
                        //neighboursToVisit.push_back((typename IPriorityQueue<size_t, T>::data_priority_t) {position, max(currPriority, mask_data[position])});
                        //q.push(position, max(currPriority, mask_data[position]));
                        neighboursToVisit.push_back(position);

                    }
                }
            }
#endif /* NEIGHBOURHOOD_DOITYOURSELF */

        }
        //for(typename vector< typename IPriorityQueue<size_t, T>::data_priority_t >::const_iterator it = neighboursToVisit.begin(); it != neighboursToVisit.end(); ++it) {
#pragma omp single
        {
        for(vector<size_t>::const_iterator it = neighboursToVisit.begin(); it != neighboursToVisit.end(); ++it) {
            //q.push((*it).value, (*it).priority);
            // but this is
            const T inte = max(currPriority, mask_data[(*it)]);
           // #pragma omp critical
            {
            q.push((*it),inte);
            }
        }
        }
        }
        free(currPrioritySlice);
#endif
    }
}

using i3d::GRAY8;

void timeAlg3() {
    Image3d<GRAY8> a("marker.tga", 0, false, -1, 0, 0);
    Image3d<GRAY8> b("mask.tga", 0, false, -1, 0, 0);
    Image3d<GRAY8> c("marker.tga", 0, false, -1, 0, 0);
    c.SetAllVoxels(GRAY8(WHITE));

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
