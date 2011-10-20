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

/** This is version that cannot deal with values in marker that are higher than zero, but
 lower than the maximal values allowed in that place by mask. It will copy
 these pixels from marker to the output image and wont do reconstruction there.

 So pixels in the marker have to be either zero or values in mask
 that is what in the article is called strengthened precondition
 */

#include <i3d/image3d.h>
#include <i3d/vector3d.h>
#include "FromArticle.h"
#include "BucketQueue.h"
#include <vector>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <vector>

#include <queue>

#include "Timing.h"
#include "ReservablePriorityQueue.h"
#include "intervalqueue.h"
#include "ipriorityqueue.h"
#include "hierarchicalqueue.h"

class indexvalue {
    int m_index;
    int m_value;
public:
    indexvalue(int index, int value): m_index(index), m_value(value) {}
    int GetValue() const {
        return m_value;
    }
    int GetIndex() const{
        return m_index;
    }
    bool operator< (const indexvalue & p2) const {
        return m_value > p2.GetValue();
    }
};

template <class T> void Reconstruction_by_dillatation_bucket(i3d::Image3d<T> & MARKER, i3d::Image3d<T> & MASK, i3d::Image3d<T> & reconstructed) {

    T *marker_data = MARKER.begin();
    T *mask_data = MASK.begin();

    T max_value = MARKER.GetVoxelData().max();

    /** ints vs size_t */
#ifdef BUCKET_QUEUE
    BucketQueue q((int)MARKER.GetImageSize(), max_value);
#endif
#ifdef RESERVABLE_PRIORITY_QUEUE
    //reservable_priority_queue<indexvalue> q((int)MARKER.GetImageSize());
    //std::priority_queue<indexvalue_t<size_t, T> > q;
    IntervalQueue<indexvalue_t<size_t, T>, T> q (MARKER.GetImageSize(), 20);
    //std::priority_queue<indexvalue, std::deque<indexvalue> > q;
    std::vector<bool> visited(MARKER.GetImageSize()+1, false);
#endif
#ifdef HIERARCHICAL_PRIORITY_QUEUE
    CHierarchicalQueue<int, int> q (MARKER.GetImageSize(), max_value);
#endif
    for (int i = 0; i != (int)MARKER.GetImageSize(); ++i) {
        if ( marker_data[i] != WHITE) {
#ifdef BUCKET_QUEUE
            q.Enqueue(i, marker_data[i]);
            //cout << "pepa" << qfallowdouble[i];
#endif
#ifdef RESERVABLE_PRIORITY_QUEUE
            q.push(indexvalue_t<size_t, T>(i, marker_data[i]));
            visited[i] = true;
#endif
#ifdef HIERARCHICAL_PRIORITY_QUEUE
            q.push(i, marker_data[i]);
#endif
        }
    }

   // std::cout << q.isTailMark(q.heads[0])<<std::endl;
   // q.Enqueue(10, 0);
   // std::cout << q.Top()<<std::endl;

    while(! q.empty()) {
        int intensity;
        int index;


#ifdef BUCKET_QUEUE
        try {
            index = q.Top();
            intensity = q.TopPriority();

//            std::cout << intensity << " " << index << std::endl;
            q.Pop();
        } catch (...) {
            break;
        }
#endif
#ifdef RESERVABLE_PRIORITY_QUEUE
        indexvalue_t<size_t, T> iv = q.top();
        q.pop();
        intensity = iv.value;
        index = iv.index;
#endif
#ifdef HIERARCHICAL_PRIORITY_QUEUE
        intensity = q.top().priority;
        index = q.top().value;
        q.pop();
#endif
        //if (intensity > 200 ) {continue;}

       //reconstructed.SetVoxel(5,0,0,5);


        intensity = std::max((T)intensity, mask_data[index]);
        reconstructed.SetVoxel((size_t)index, (T)intensity);
       //std::cout << reconstructed_data[reconstructed.GetIndex(5, 0, 0)] << std::endl;

//        int xy = index % reconstructed.GetSliceSize();
//        int z = index / reconstructed.GetSliceSize();
//        int x = xy % reconstructed.GetHeight();
//        int y = xy / reconstructed.GetHeight();
        int x = reconstructed.GetX(index);
        int y = reconstructed.GetY(index);
        int z = reconstructed.GetZ(index);

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
#ifdef BUCKET_QUEUE
                if ( ! (q.EverEnqueued(nindex))) {

                    q.Enqueue(nindex, intensity);

                }
#endif
#ifdef RESERVABLE_PRIORITY_QUEUE
                if(visited[nindex] != true) {
                    q.push(indexvalue_t<size_t, T> (nindex, intensity));
                    visited[index] = true;
                }
#endif
#ifdef HIERARCHICAL_PRIORITY_QUEUE
                if ( ! (q.hasBeenDequeued(nindex))) {
                    q.push(nindex, intensity);
                }
#endif
            }
        }
    }
}

void timeAlg2() {
    i3d::Image3d<i3d::GRAY8> a("marker.tga", 0, false, -1, 0, 0);
    i3d::Image3d<i3d::GRAY8> b("mask.tga", 0, false, -1, 0, 0);
    i3d::Image3d<i3d::GRAY8> c("marker.tga", 0, false, -1, 0, 0);
    c.SetAllVoxels(i3d::GRAY8(WHITE));

     //! Testrun, trying not to measture startup time
     Reconstruction_by_dillatation_bucket(a, b, c);
//     c.SetAllVoxels(i3d::GRAY8(WHITE));

     const int BASE = 10;
     const int STEP = 10;
     const int UPTO = 10000;



     for (int stepper = BASE; stepper <= UPTO; stepper = stepper*STEP) {
         timespec stopwatch_start;
         timespec stopwatch_stop;
         timespec stopwatch_elapsed;

         int err;
         err = clock_gettime(CLOCK_REALTIME, &stopwatch_start);
         assert(err == 0);

         for (int count = 0; count < stepper; count++) {
             Reconstruction_by_dillatation_bucket(a, b, c);
//             //! Cleanup of the output image
//             c.SetAllVoxels(i3d::GRAY8(WHITE));
         }

         err = clock_gettime(CLOCK_REALTIME, &stopwatch_stop);
         assert(err == 0);

         stopwatch_elapsed = diff(stopwatch_start, stopwatch_stop);
         std::cout << stepper << ": " << stopwatch_elapsed.tv_sec << "." << stopwatch_elapsed.tv_nsec << std::endl;
      }

//     /// now we won't erase this so we can check it was still doing what it should
     Reconstruction_by_dillatation_bucket(a, b, c);
     c.SaveImage("reconstructed.tga", i3d::IMG_TARGA, false);
}
