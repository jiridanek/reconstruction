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

/** \file hierarchicalheapqueue.h
  \brief

  hierarchical queue (sometimes called Bucket queue) it can do all operations (with the exceptiono of constructor) in O(1) time

  it only works for integral values as both DATA_T and PRIORITY_T. Also,

  \author Jiří Daněk (dnk@mail.muni.cz) 2011

*/

/**
 Data structure proposed in "Revisiting Priority Queues for Image Analysis" by
 Cris L. Luengo Hendriks.

 It comprises of several (parameter number_of_intervals in constructior)
 individual heaps and

 TODO:

 For use in downhill morphological reconstruction algorithm it needs to be
 able to deal with situation when there is one value inserted twice with
 different priorities and dequeue only the one with the highest priority
 (forgetting the other priorities)
 */
/** Optional part of the project, not to be completed, i presume ;-) */

#include <cstddef>
#include <queue>
#include <vector>

#include "ipriorityqueue.h"

using std::priority_queue;
using std::vector;
using std::size_t;

#ifndef CHIERARCHICALHEAPQUEUE_H
#define CHIERARCHICALHEAPQUEUE_H

template <class DATA_T, class PRIORITY_T>
class CHierarchicalHeapQueue : public IPriorityQueue<DATA_T, PRIORITY_T> {
public:
    struct item_t {
        DATA_T value;
        PRIORITY_T priority;
        bool operator > (const struct item_t & i2) {
            // descending order
            return this->priority > i2.priority;
        }
    };
private:
    priority_queue<DATA_T> *m_queues;
public:
    CHierarchicalHeapQueue(PRIORITY_T maximal_priority, size_t number_of_intervals);
    ~CHierarchicalHeapQueue();
};

template <class DATA_T, class PRIORITY_T>
CHierarchicalHeapQueue<DATA_T, PRIORITY_T>::CHierarchicalHeapQueue(PRIORITY_T maximal_priority, size_t number_of_intervals) {

    m_queues = new std::priority_queue<struct item_t>[number_of_intervals];
}

#endif // CHIERARCHICALHEAPQUEUE_H

