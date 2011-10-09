#include <cstdlib>
#include <cstring>
#include <exception>

#ifndef BUCKETQUEUE_H
#define BUCKETQUEUE_H

class QueueEmpty {
    virtual const char* what() const throw()
      {
        return "The BucketQueue is empty";
      }
} QueueEmpty;


/*!
  Both the priority and the value has to be of the same _integral_ type, like size_t or int

  Queue can be operated only ether by using just functions pop and top with one parameter, or
  their versions without parameters and all other functions, because the versions with parameter can put the object
  into inconsistent state;
  */
class BucketQueue {
public:
    int current_priority;
    int highest_priority;
    /** These arrays are to be manipulated only using C functions (malloc, ...) */
    int * heads;
    int * tails;
public:
    BucketQueue(int max_no_items, int max_priority);
    ~BucketQueue();
    void Enqueue(int value, int priority);
    int Top(int priority);
    void Pop(int priority);
    int TopPriority();
    int Top();
    void Pop();
    bool EverEnqueued(int value);
    bool isTailMark(const int & value) const;
    bool empty();

};

BucketQueue::BucketQueue(int max_no_items, int max_priority) {

    heads = (int*) malloc((max_priority+1) * sizeof(int));
    tails = (int*) malloc((max_no_items+1) * sizeof(int));
    memset(heads,0xfe,(max_priority+1)*sizeof(int));               // 0xfe will mark the tail of a list
    memset(tails,0xff,(max_no_items+1)*sizeof(int));           // 0xff will mark unfinalised pixels

    highest_priority = max_priority;
    current_priority = 0;
}

BucketQueue::~BucketQueue() {
    free(tails);
    free(heads);
}

//0<=intenzita <= maxval
//index < w*h
void BucketQueue::Enqueue(int value, int priority) {
    int old_index = heads[priority];
    tails[value] = old_index;
    heads[priority] = value;
}

bool BucketQueue::empty() {
    return current_priority > highest_priority;
}

int BucketQueue::Top(int priority) {
    if (empty()) {
        throw QueueEmpty;
    }
    int index = heads[priority];
    return index;
}

void BucketQueue::Pop(int priority) {
    int index = heads[priority];
    heads[priority] = tails[index];
}

int BucketQueue::TopPriority() {
    return current_priority;
}

int BucketQueue::Top() {
    int out = Top(current_priority);
    while (isTailMark(out)) {
        current_priority += 1;
        out = Top(current_priority);
    }
    return out;
}

bool BucketQueue::isTailMark(const int & value) const {
    return value == (int)0xfefefefe;
}

void BucketQueue::Pop() {
    Pop(current_priority);
}

/*!
  Once a value has been enqueued, it's value (
        or 0xfefefefe, in case that this value was the first of its priority)
  will be forever kept at tails[value]. So we can do simple compare
  */
bool BucketQueue::EverEnqueued(int index){
    return tails[index] != (int)0xffffffff;
}

#endif // BUCKETQUEUE_H
