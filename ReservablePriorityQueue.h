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
