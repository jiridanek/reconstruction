/*!

 */

#include <i3d/image3d.h>
#include <i3d/vector3d.h>
#include <i3d/neighbours.h>

#include <cassert>
#include <algorithm>
#include <iostream>

#include "Timing.h"
#include "ipriorityqueue.h"
#include "hierarchicalqueue.h"

using std::vector;
using std::max;
using i3d::GetWindow;
using std::size_t;

template <class T> void Reconstruction_by_dillatation(i3d::Image3d<T> & MARKER, i3d::Image3d<T> & MASK, i3d::Image3d<T> & reconstructed, const i3d::Neighbourhood & NEIGHBOURHOOD = i3d::nb3D_o6) {

    T *marker_data = MARKER.begin();
    T *mask_data = MASK.begin();

    T max_value = MARKER.GetVoxelData().max();


    CHierarchicalQueue<size_t, T> q(MARKER.GetImageSize(), max_value);

    for (int i = 0; i != (int)MARKER.GetImageSize(); ++i) {
        if ( marker_data[i] != WHITE) {
            q.push(i, max(marker_data[i], mask_data[i]));
        }
    }

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


        std::vector<T *> window;
        window.reserve(NEIGHBOURHOOD.size());

        GetWindow(MASK, x, y, z, NEIGHBOURHOOD, window);

        /// na řádku 2235 v souboru morphology.cc zde delaji cosi s ukazateli
        /// proč?
        for (typename vector<T*>::iterator it= window.begin(); it != window.end(); ++it) {
            const size_t position = (*it)-MASK.begin();
            if(!q.hasBeenDequeued(position)) {
                q.push(position, max(intensity, *(*it)));
            }
        }


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
     const int UPTO = 0;



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
