#include <i3d/image3d.h>
#include <i3d/vector3d.h>
#include "FromArticle.h"
#include "BucketQueue.h"
#include <vector>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <vector>

#include "Timing.h"
#include "ReservablePriorityQueue.h"

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
    T *reconstructed_data = reconstructed.begin();

    T max_value = MARKER.GetVoxelData().max();

    /** ints vs size_t */
#ifndef RESERVABLE_PRIORITY_QUEUE
    BucketQueue q((int)MARKER.GetImageSize(), max_value);
#else
    reservable_priority_queue<indexvalue> q((int)MARKER.GetImageSize());
    std::vector<bool> visited(MARKER.GetImageSize()+1, false);
#endif
    for (int i = 0; i != (int)MARKER.GetImageSize(); ++i) {
        if ( marker_data[i] != WHITE) {
#ifndef RESERVABLE_PRIORITY_QUEUE
            q.Enqueue(i, marker_data[i]);
#else
            q.push(indexvalue(i, marker_data[i]));
            visited[i] = true;
#endif
        }
    }

   // std::cout << q.isTailMark(q.heads[0])<<std::endl;
   // q.Enqueue(10, 0);
   // std::cout << q.Top()<<std::endl;

    while(! q.empty()) {
        int intensity;
        int index;
#ifndef RESERVABLE_PRIORITY_QUEUE
        try {
            intensity = q.TopPriority();
            index = q.Top();
//            std::cout << intensity << " " << index << std::endl;
            q.Pop();
        } catch (...) {
            break;
        }
#else
        indexvalue iv = q.top();
        q.pop();
        intensity = iv.GetValue();
        index = iv.GetIndex();
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

                    if (y + b > (int)reconstructed.GetHeight() - 1) {
                        continue;
                    }

                }

                if (b == 0) {
                    if (x == 0 && a == -1) {
                        continue;
                    }

                    if (x + a > (int)reconstructed.GetWidth() - 1 ) {
                        continue;
                    }
                }
                size_t nx = x + a;
                size_t ny = y + b;
                int nindex = reconstructed.GetIndex(nx, ny, 0);
#ifndef RESERVABLE_PRIORITY_QUEUE
                if ( ! q.EverEnqueued(nindex)) {
                    q.Enqueue(nindex, intensity);
                }
#else
                if(visited[nindex] != true) {
                    q.push(indexvalue(nindex, intensity));
                    visited[index] = true;
                }
#endif
            }
        }
    }
}

void timeAlg2() {
    i3d::Image3d<i3d::GRAY8> a("marker.jpg", 0, false, -1, 0, 0);
    i3d::Image3d<i3d::GRAY8> b("mask.jpg", 0, false, -1, 0, 0);
    i3d::Image3d<i3d::GRAY8> c("marker.jpg", 0, false, -1, 0, 0);
    c.SetAllVoxels(i3d::GRAY8(WHITE));

     //! Testrun, trying not to measture startup time
     Reconstruction_by_dillatation_bucket(a, b, c);
//     c.SetAllVoxels(i3d::GRAY8(WHITE));

     const int BASE = 10;
     const int STEP = 10;
     const int UPTO = 100;



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
