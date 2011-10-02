#include <cstring>
#include <cstdlib>
#include <iostream>

#include "i3dcore.h"
#include "point.h"

i3d::i3d(size_t w, size_t h, uint8 image_data[5][5]) :
    m_w(w), m_h(h)
{
    (this->m_image_data) = (uint8*)malloc(w*h*sizeof(uint8));
    for (size_t y = 0; y < h; y++) {
        for (size_t x = 0; x < w; x++) {
            uint8 data = image_data[y][x];
            (this->m_image_data)[x+y*h] = data;
        }
    }

}

size_t i3d::getWidth() {
    return m_w;
}

size_t i3d::getHeight() {
    return m_h;
}

uint8 i3d::getVoxel(size_t x, size_t y) {
    return this->m_image_data[x+y*(this->getHeight())];
}

uint8 i3d::getVoxel(Point p) {
    return getVoxel(p.getX(), p.getY());
}


void i3d::setVoxel(size_t x, size_t y, uint8 value) {
    this->m_image_data[x+y*(this->getHeight())] = value;
}

void i3d::setVoxel(Point p, uint8 value) {
    this->setVoxel(p.getX(), p.getY(), value);
}

void i3d::print() {
    for (size_t y = 0; y < this->getHeight(); y++) {
        for (size_t x = 0; x < this->getWidth(); x++) {

            std::cout << (int)this->getVoxel(x,y);
            //printf("x:%d, y:%d %d", x,y,im2.getVoxel(x,y));

        }
            std::cout << std::endl << std::flush;
    }
}

i3d::~i3d() {
    //free(m_image_data);
}
