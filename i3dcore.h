#include "point.h"

#ifndef I3DCORE_H
#define I3DCORE_H

class i3d
{
    size_t m_w;
    size_t m_h;
    uint8 *m_image_data;
public:
    i3d(size_t w, size_t h, uint8 image_data[5][5]);
    ~i3d();
    size_t getWidth();
    size_t getHeight();
    uint8 getVoxel(size_t x, size_t y);
    uint8 getVoxel(Point p);
    void setVoxel(size_t x, size_t y, uint8 value);
    void setVoxel(Point p, uint8 value);

    void print();
};

#endif // I3DCORE_H
