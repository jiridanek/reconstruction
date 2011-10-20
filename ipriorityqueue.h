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

/** \file ipriorityqueue.h
  \brief

  This file contains the main  template and closely related
  functions.

  \author Jiří Daněk (dnk@mail.muni.cz) 2011

*/

#ifndef IPRIORITYQUEUE_H
#define IPRIORITYQUEUE_H

template <class DATA_T, class PRIORITY_T> class IPriorityQueue
{
public:

    struct data_priority_t{
        DATA_T value;
        PRIORITY_T priority;
    };

    virtual void push(DATA_T value, PRIORITY_T priority) = 0;
    virtual void pop() = 0;
    virtual struct data_priority_t top() const = 0;
    virtual bool empty() const = 0;
    virtual bool everEnqueued(DATA_T value) const = 0;
    virtual ~IPriorityQueue() {}
};

#endif // IPRIORITYQUEUE_H
