#include <cstring>
#include "i3d/vector3d.h"

#ifndef POINT_H
#define POINT_H

//template <class V, class T> class Point;
template <class V, class T> class Point {
    i3d::Vector3d<V> m_vec;
    T m_value;
public:
    Point(i3d::Vector3d<V> vec, T value): m_vec(vec), m_value(value) {}
    i3d::Vector3d<V> GetVector() const;
    T GetValue() const;
    //bool operator< (const Point <V, T> &p1, const Point<V, T> & p2) const;
    bool operator<(const Point& p2) const;
    int GetInt();
};

template <class V, class T> i3d::Vector3d<V> Point<V, T>::GetVector() const
{
    return m_vec;
}

template <class V, class T> T Point<V, T>::GetValue() const
{
    return m_value;
}


//template <class V, class T> bool Point<V,T>::operator< (const Point<V,T> &p1, const Point<V, T> & p2) const {
//    return p1.GetValue() < p2.GetValue();
//}


// lowest intensity is 255 - white
template <class V, class T> bool Point<V, T>::operator<(const Point<V, T> &p2) const {
    return this->GetValue() > p2.GetValue();
}

template <class V, class T> int Point<V, T>::GetInt() {
    return 1;
}


#endif // POINT_H
