#include <unittest++/UnitTest++.h>

#include "../FirstIdeas/ipriorityqueue.h"
#include "../FirstIdeas/hierarchicalqueue.h"

SUITE(HierarchicalQueue) {
    TEST(CreateQueue) {
        CHierarchicalQueue<int, int> q(10, 5);
        CHECK(q.empty());
    }

    TEST(PushTopPop) {
        CHierarchicalQueue<int, int> q(10, 5);

        CHECK(q.empty());
        q.push(8, 2);
        CHECK_EQUAL(8, q.top().value);
        CHECK_EQUAL(2, q.top().priority);
        q.pop();
        CHECK(q.empty());
    }

    TEST(PushPushTopTopPopPop) {
        CHierarchicalQueue<int, int> q(10, 5);
        CHECK(q.empty());
        q.push(8, 5);
        CHECK_EQUAL(q.top().value, 8);
        CHECK_EQUAL(q.top().priority, 5);
        q.push(4, 4);
        CHECK_EQUAL(4, q.top().value);
        CHECK_EQUAL(4, q.top().priority);
        q.pop();
        CHECK_EQUAL(q.top().value, 8);
        CHECK_EQUAL(q.top().priority, 5);
        q.pop();
        CHECK(q.empty());

        //
        q.push(5,5);
        CHECK(! q.empty());
    }

    TEST(InsertElementTwiceSamePriority) {
        CHierarchicalQueue<int, int> q(10, 5);
        q.push(4, 3);
        CHECK_EQUAL(4, q.top().value);
        CHECK_EQUAL(3, q.top().priority);
        q.push(4, 3);
        CHECK_EQUAL(4, q.top().value);
        CHECK_EQUAL(3, q.top().priority);
        q.pop();
        CHECK(q.empty());
    }

    TEST(InsertElementTwiceDifferentPriority) {
        CHierarchicalQueue<int, int> q(10, 5);
        q.push(4, 3);
        CHECK_EQUAL(4, q.top().value);
        CHECK_EQUAL(3, q.top().priority);
        q.push(4, 2);
        CHECK_EQUAL(4, q.top().value);
        CHECK_EQUAL(2, q.top().priority);
        q.pop();
        CHECK(q.empty());
    }
    TEST(InsertTwiceHasToRebuildList) {
        CHierarchicalQueue<int, int> q(10, 5);
        q.push(7, 1);
        q.push(6, 1);
        q.push(5, 3);
        q.push(4, 1);
        q.push(2, 1);

        q.push(5, 1);

        q.push(3, 1);

        CHECK_EQUAL(3, q.top().value);
        CHECK_EQUAL(1, q.top().priority);
        q.pop();

        CHECK_EQUAL(5, q.top().value);
        CHECK_EQUAL(1, q.top().priority);
        q.pop();
        CHECK_EQUAL(2, q.top().value);
        CHECK_EQUAL(1, q.top().priority);
        q.pop();
        CHECK_EQUAL(4, q.top().value);
        CHECK_EQUAL(1, q.top().priority);
        q.pop();
        CHECK_EQUAL(6, q.top().value);
        CHECK_EQUAL(1, q.top().priority);
        q.pop();
        CHECK_EQUAL(7, q.top().value);
        CHECK_EQUAL(1, q.top().priority);
        q.pop();
        CHECK(q.empty());
    }

    TEST(FromMiddle) {
        CHierarchicalQueue<int, int> q(10, 5);
        q.push(6, 1);
        q.push(5, 1);
        q.push(4, 1);

        q.push(5, 3);

        CHECK_EQUAL(4, q.top().value);
        CHECK_EQUAL(1, q.top().priority);
        q.pop();
        CHECK(! q.empty());
        CHECK_EQUAL(6, q.top().value);
        CHECK_EQUAL(1, q.top().priority);
        q.pop();
        CHECK(! q.empty());
        CHECK_EQUAL(5, q.top().value);
        CHECK_EQUAL(3, q.top().priority);
        q.pop();

        CHECK(q.empty());
    }

    TEST(WeHaveToAlsoRelinkBack) {
        CHierarchicalQueue<int, int> q(10, 5);
        q.push(6, 1);
        q.push(5, 3);
        q.push(4, 1);

        q.push(5, 1);
        q.push(4, 2);

        CHECK_EQUAL(5, q.top().value);
        CHECK_EQUAL(1, q.top().priority);
        q.pop();
        CHECK(! q.empty());
        CHECK_EQUAL(6, q.top().value);
        CHECK_EQUAL(1, q.top().priority);
        q.pop();
        CHECK(! q.empty());
        CHECK_EQUAL(4, q.top().value);
        CHECK_EQUAL(2, q.top().priority);
        q.pop();

        CHECK(q.empty());
    }

    TEST(EverEnqueued) {
        CHierarchicalQueue<int, int> q(10, 5);
        q.push(6, 1);
        q.push(5, 3);
        q.push(4, 1);

        q.push(5, 1);
        q.push(4, 2);
        q.push(8, 0);

        CHECK(q.everEnqueued(6));
        CHECK(q.everEnqueued(5));
        CHECK(q.everEnqueued(4));
        CHECK(q.everEnqueued(8));

        CHECK(! q.hasBeenDequeued(6));
        CHECK(! q.hasBeenDequeued(5));
        CHECK(! q.hasBeenDequeued(4));
        CHECK(! q.hasBeenDequeued(8));

        q.pop();
        CHECK(q.hasBeenDequeued(8));
        CHECK(q.everEnqueued(8));
        q.push(8, 0);
        CHECK(!q.hasBeenDequeued(8));
        CHECK(q.everEnqueued(8));
        q.pop();
        q.pop();
        q.pop();
        q.pop();

        CHECK(q.empty());

        CHECK(q.everEnqueued(6));
        CHECK(q.everEnqueued(5));
        CHECK(q.everEnqueued(4));
        CHECK(q.everEnqueued(8));

        CHECK(q.hasBeenDequeued(6));
        CHECK(q.hasBeenDequeued(5));
        CHECK(q.hasBeenDequeued(4));
        CHECK(q.hasBeenDequeued(8));
    }

    TEST (BreakingPop) {
        CHierarchicalQueue<int, int> q(10, 5);
        q.push(7, 3);
        q.push(6, 3);
        q.pop(); // removes 6,3
        q.push(7, 2);
        q.pop();
        CHECK(q.hasBeenDequeued(6));
        CHECK(q.hasBeenDequeued(7));
        CHECK(q.empty());
    }

    TEST(StoredPriority1) {
        CHierarchicalQueue<int, int> q(10, 5);
        q.push(7, 3);
        CHECK_EQUAL(3, q.getPriority(7));
        q.push(6, 3);
        CHECK_EQUAL(3, q.getPriority(7));
        CHECK_EQUAL(3, q.getPriority(6));
        q.push(7,1);
        CHECK_EQUAL(1, q.getPriority(7));
        CHECK_EQUAL(3, q.getPriority(6));
    }

    TEST(getPrioritySlice) {
        CHierarchicalQueue<int, int> q(10, 5);
        q.push(7, 3);
        q.push(6, 3);
        q.push(5, 3);
        q.push(4, 3);
        q.push(3, 4);
        q.push(2, 4);
        q.push(1, 4);

        int *prioritySlice;
        int priority;
        size_t element_count;

        q.getPrioritySlice(&prioritySlice, &element_count, &priority);
        CHECK_EQUAL(3, priority);
        CHECK_EQUAL(4, element_count);
        int t1[] = {4,5,6,7};
        CHECK_ARRAY_EQUAL(t1, prioritySlice, 4);
        free(prioritySlice);

        q.getPrioritySlice(&prioritySlice, &element_count, &priority);
        CHECK_EQUAL(4, priority);
        CHECK_EQUAL(3, element_count);
        int t2[] = {1,2,3};
        CHECK_ARRAY_EQUAL(t2, prioritySlice, 3);
        free(prioritySlice);

        CHECK(q.empty());
    }

    TEST(PrioritySliceWouldTryToPopEmptyQueue) {
        CHierarchicalQueue<int, int> q(10, 5);
        q.push(1, 5);

        int *prioritySlice;
        int priority;
        size_t element_count;

        q.getPrioritySlice(&prioritySlice, &element_count, &priority);
        CHECK_EQUAL(5, priority);
        CHECK_EQUAL(1, element_count);
        int t1[] = {1};
        CHECK_ARRAY_EQUAL(t1, prioritySlice, 1);
        free(prioritySlice);

        CHECK(q.empty());
    }
}
