#include <unittest++/UnitTest++.h>
#include <exception>

#include "../FirstIdeas/BucketQueue.h"

SUITE(OldBucketQueue) {
    TEST(Create) {
        BucketQueue q = BucketQueue(10,5);
    }

    TEST(PushPop) {
        BucketQueue q = BucketQueue(10,5);
        q.Enqueue(8,5);
        CHECK(! q.empty());
        CHECK_EQUAL(8, q.Top());
        CHECK_EQUAL(5, q.TopPriority());
        q.Pop();

        // yeah, I know, it should've know by now, its empty
        CHECK(! q.empty());
        CHECK_THROW(q.Top(), std::exception);

        // but now it works
        CHECK(q.empty());
    }
}
