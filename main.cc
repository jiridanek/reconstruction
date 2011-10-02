#include <cstring>
#include <queue>
#include <algorithm>
#include <iostream>
#include <cstdio>

#include "i3dcore.h"
#include "point.h"

//! Implements morphological reconstruction by dilatation using unoptimized Downhill algorithm
/*!
  \param marker
  \param mask
  \param reconstruction resulting image
  */
void Reconstruction_by_dilatation_downhill(i3d marker, i3d mask, i3d *reconstruction) {

    std::priority_queue<Point> q;

    for (size_t y = 0; y < marker.getHeight(); y++) {
        for (size_t x = 0; x < marker.getWidth(); x++) {
            if (marker.getVoxel(x, y) != 0 ) {
                q.push(Point(x,y, marker.getVoxel(x, y)));
                //reconstruction->setVoxel(x, y, marker.getVoxel(x,y));
            }
        }
    }
    while (! q.empty()) {
        Point p = q.top();
        q.pop();

        reconstruction->setVoxel(p, std::min(p.getValue(), mask.getVoxel(p)));

        std::vector<Point> vec;
        for (int a = -1; a <= 1; a++) {
            for (int b = -1; b <= 1; b++) {
                if (std::abs(a) == std::abs(b)) {
                    continue;
                }

                if (a == 0) {
                    if (p.getY() == 0 && b == -1) {
                        continue;
                    }

                    if (p.getY() + b >= reconstruction->getHeight()) {
                        continue;
                    }

                }

                if (b == 0) {
                    if (p.getX() == 0 && a == -1) {
                        continue;
                    }

                    if (p.getX() + a >= reconstruction->getWidth()) {
                        continue;
                    }
                }
                Point *ns = new Point(p.getX()+a, p.getY()+b, 0);
                vec.push_back(*ns);
            }
        }

        for (std::vector<Point>::iterator it = vec.begin(); it != vec.end(); ++it) {
            if ( reconstruction->getVoxel(*it) ==  0 && mask.getVoxel(*it) != 0) {
                q.push(Point((*it).getX(), (*it).getY(), reconstruction->getVoxel(p)));
            }

            //reconstruction->setVoxel(0,0,5);
        }
        //reconstruction->print();
    }
    return;

}

int main() {
    uint8 ctverec[5][5] = {{1,1,1,1,1},
                             {2,1,0,1,0},
                             {3,0,0,0,1},
                             {4,0,0,0,1},
                             {5,1,1,1,1}};
    uint8 stred[5][5] = {{1,0,0,0,0},
                         {0,0,0,0,0},
                         {0,0,0,0,0},
                         {0,0,0,0,0},
                         {0,0,0,0,0}};

    i3d mask(5,5,ctverec);
    i3d marker(5,5,stred);
    i3d out(5,5,stred);

   Reconstruction_by_dilatation_downhill(marker, mask, &out);

   out.print();
   std::cout << std::endl;
   mask.print();
   std::cout << std::endl;
   marker.print();
}
