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

/** \file ReservablePriorityQueue.h
  \brief



  \author Jiří Daněk (dnk@mail.muni.cz) 2011

*/

#ifndef RESERVABLEPRIORITYQUEUE_H
#define RESERVABLEPRIORITYQUEUE_H

#include <queue>

/*!

  code from http://stackoverflow.com/questions/3666387/c-priority-queue-underlying-vector-container-capacity-resize
  */
template <class T>
class reservable_priority_queue: public std::priority_queue<T>
{
public:
    typedef typename std::priority_queue<T>::size_type size_type;
    reservable_priority_queue(size_type capacity = 0) {
        reserve(capacity);
    }
    void reserve(size_type capacity) { this->c.reserve(capacity); }
    size_type capacity() const { return this->c.capacity(); }
};


#endif // RESERVABLEPRIORITYQUEUE_H
