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
};

#endif // IPRIORITYQUEUE_H
