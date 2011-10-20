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

/** \file hierarchicalqueue.h
  \brief

  hierarchical queue (sometimes called Bucket queue) it can do all operations (with the exceptiono of constructor) in O(1) time

  it only works for integral values as both DATA_T and PRIORITY_T. Also,

  \author Jiří Daněk (dnk@mail.muni.cz) 2011

*/

#include <cstddef>
#include <cstdlib> //malloc
#include <cstring> //memset

#include "ipriorityqueue.h"

using std::size_t;

#ifndef HIERARCHICALQUEUE_H
#define HIERARCHICALQUEUE_H

template <class DATA_T, class PRIORITY_T>
class CHierarchicalQueue : public IPriorityQueue<DATA_T, PRIORITY_T> {

    struct item_t {
        DATA_T value;               //! can be computed from indices, does not have to be stored
        bool ever_enqueued;         //! used only in m_tails, always false in m_head
        /** items not in queue can sometimes have a priority set
          as a leftover. That doesn't break anything
         */
        PRIORITY_T priority;        //! cannot be computed, would require traversing the linked list
        /** these pointers should always be kept in consistent state
          like setting them to NULL and so on
         */
        struct item_t *previous;
        struct item_t *next;
    };

    PRIORITY_T m_current_priority;
    PRIORITY_T m_highest_priority;
    DATA_T m_highest_value;
    bool m_is_empty;

    /** These arrays are to be manipulated only using C functions (malloc, ...) */
    struct item_t * m_heads;
    struct item_t * m_tails;
public:

    /*!
     This queue also maintains a double linked list of its elements, so it can
     relatively efficiently remove random enqueed elements when needed

     Top priority is 0 (black color), lowest priority is m_highest_priority

     Allocates highest_value * sizeof(DATA_T) of space for the data plus
     highest_priority * sizeof(DATA_T) for indices

     Push will work correctly only if the priority of the new value is equal to or
     lower than the priority of the element in the queue with maximal priority.
     So e.g once the queue has been emptied, it will keep thinking it is empty.

     example, how the internal variables may look like after
     q.push(4, 0);  q.push(5, 0); q.push(13, 2);
     (memory adresses are completely made up and do not make sense)

     m_heads
     =======
     [0], at 0x500       [1]                 [2], at 0x600          ...
     item_t:             item_t:             item_t
       value = 5           value=0             value=13
       previous = NULL     previous = NULL     previous = NULL
       next = 0x1000       next = NULL         next = 0x2000
       ever_enqueued=false ever_enqueued=false ever_enqueued=false

     m_tails (items set completely to zero are left out)
     =======
     [3], at 0x750       [5], at 0x1000      [13], at 0x2000
     item_t:             item_t:             item_t:
       value=0             value=3             value = 0
       previous=0x1000     previous=0x500      previous=0x600
       next=NULL           next=0x750          next=NULL
       ever_enqueued=true  ever_enqueued=true  ever_enqueued=true
    */
    CHierarchicalQueue(DATA_T highest_value, PRIORITY_T highest_priority);
    ~CHierarchicalQueue();
    virtual void push(DATA_T value, PRIORITY_T priority);
    /*!
     Sets the m_current_priority to the next bucket containing data
    */
    virtual void pop();
    virtual struct IPriorityQueue<DATA_T, PRIORITY_T>::data_priority_t top() const;
    virtual bool empty() const;
    virtual bool everEnqueued(DATA_T value) const;
    bool hasBeenDequeued(DATA_T value) const;
    bool isInQueue(DATA_T value) const;
    PRIORITY_T getPriority(DATA_T value) const;
};

template <class DATA_T, class PRIORITY_T>
CHierarchicalQueue<DATA_T, PRIORITY_T>::CHierarchicalQueue(
        DATA_T highest_value
        , PRIORITY_T highest_priority) :
    m_current_priority(highest_priority)
  , m_highest_priority(highest_priority)
  , m_highest_value(highest_value)
  , m_is_empty(true) {

    m_heads = (struct item_t *) malloc((m_highest_priority+1) * sizeof(struct CHierarchicalQueue::item_t));
    m_tails = (struct item_t *) malloc((m_highest_value+1) * sizeof(struct CHierarchicalQueue::item_t));

    // initialize item_t structures to zeros/NULLs
    // we can do that as long as the structure contains only primitive types
    memset(m_heads,0,(m_highest_priority+1)*sizeof(struct CHierarchicalQueue::item_t));
    memset(m_tails,0,(m_highest_value+1)*sizeof(struct CHierarchicalQueue::item_t));
}

/*!

 m_heads[]                          m_tails

 beginning |             |            |           |          end

            << .previous                   .next >>
 */

template <class DATA_T, class PRIORITY_T>
void CHierarchicalQueue<DATA_T, PRIORITY_T>::push(DATA_T value, PRIORITY_T priority) {
    if (m_current_priority > priority) {
        m_current_priority = priority;
    }

    // item in m_heads array, head of the linked list for given priority
    struct item_t & head = m_heads[priority];
    //
    struct item_t & tail = m_tails[value];

    // if this item is part of some other linked list for different priority,
    // we need to move it into the new priority and relink the list for the old priority

    // if we are inserting the item to the same priority as it is already in,
    // the function just returns, leaving queue unmodified

    /*!
      beginning                end

      previous <>     tail    <> next
                (take this out)
    */

    if (tail.previous != NULL) { // could call isInQueue(...) instead
        /// this check saves more time here, than in code that calls q.push
        if (m_tails[value].priority == priority) { // could call getPriority(...)
            // this value,priority pair is already in queue, nothing to do
            return;
        }
        //this removes the item from the linked list it is in
        struct item_t & previous = *(tail.previous);
        //relink from beginning -> end
        previous.value = tail.value;
        previous.next = tail.next;
        if (tail.next != NULL) {
            struct item_t & next = *(tail.next);
            //TEST(WeHaveToAlsoRelinkBack) {
            next.previous = &previous;
        } else {
            //previous.value = 0;
        }
    }

    tail = head;
    tail.previous = & head;

    // when the item is the second item in the list, it points to the head
    // in its .previous pointer. When we move the item further by appending
    // to the beginning, we need to point it to the predecessor in tails.
    if (tail.next != NULL) {
        tail.next->previous = &tail;
    }

    head.value = value;
    head.next = & tail;

    m_is_empty = false;
    tail.ever_enqueued = true;
    tail.priority = priority;
}

template <class DATA_T, class PRIORITY_T>
void CHierarchicalQueue<DATA_T, PRIORITY_T>::pop() {

    DATA_T index = m_heads[m_current_priority].value;
    /// don't forget to set both pointers in tail to NULL,
    /// so hasBeenDequeued(...) will work
    /// also set the previous pointer to NULL in head
    m_tails[index].previous = NULL;
    m_heads[m_current_priority] = m_tails[index];
    m_tails[index].next = NULL;

    /// keep it linked, see TEST(BreakingPop)
    if(m_heads[m_current_priority].next != NULL) {
        m_heads[m_current_priority].next->previous = & m_heads[m_current_priority];
    }

    // try to move to next nonempty bucket
    while(m_heads[m_current_priority].next == NULL && m_current_priority != m_highest_priority) {
        ++m_current_priority;
    }

    // is the queue empty?
    if (m_heads[m_current_priority].next == NULL) {
        m_is_empty = true;
    }
}

template <class DATA_T, class PRIORITY_T>
struct IPriorityQueue<DATA_T, PRIORITY_T>::data_priority_t CHierarchicalQueue<DATA_T, PRIORITY_T>::top() const{
    struct IPriorityQueue<DATA_T, PRIORITY_T>::data_priority_t out =
        {m_heads[m_current_priority].value, m_current_priority};
    return out;
}

template <class DATA_T, class PRIORITY_T>
bool CHierarchicalQueue<DATA_T, PRIORITY_T>::empty() const {
    return m_is_empty;
}

template <class DATA_T, class PRIORITY_T>
bool CHierarchicalQueue<DATA_T, PRIORITY_T>::everEnqueued(DATA_T value) const {
    return m_tails[value].ever_enqueued;
}

template <class DATA_T, class PRIORITY_T>
bool CHierarchicalQueue<DATA_T, PRIORITY_T>::hasBeenDequeued(DATA_T value) const {
    return this->everEnqueued(value) && m_tails[value].previous == NULL;
}

/*!
 if enqueued, it has ALWAYS a previous item (could be in m_heads)
 */
template <class DATA_T, class PRIORITY_T>
bool CHierarchicalQueue<DATA_T, PRIORITY_T>::isInQueue(DATA_T value) const {
    return m_tails[value].previous != NULL;
}

template <class DATA_T, class PRIORITY_T>
PRIORITY_T CHierarchicalQueue<DATA_T, PRIORITY_T>::getPriority(DATA_T value) const {
    return m_tails[value].priority;
}

template <class DATA_T, class PRIORITY_T>
CHierarchicalQueue<DATA_T, PRIORITY_T>::~CHierarchicalQueue() {
    free (m_heads);
    free (m_tails);
}

#endif // HIERARCHICALQUEUE_H
