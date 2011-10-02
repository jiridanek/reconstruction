#include <cstring>

#ifndef POINT_H
#define POINT_H

typedef unsigned int uint8;

class Point {
    size_t m_x;
    size_t m_y;
    uint8 m_value;
public:
    Point(size_t x, size_t y, uint8 value): m_x(x), m_y(y), m_value(value) {}
    size_t getX();
    size_t getY();
    uint8 getValue() const;
    bool operator< (const Point & p2) const {
        return getValue() < p2.getValue();
    }
};

#endif // POINT_H


