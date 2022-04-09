#include "utest.h"
#include "../kiss-ds/KISS_QUEUE.h"

/* This test validates that the can be created and GetItem* functions work as expected on an empty queue */
UTEST(KISS_QUEUE, CreationDeletion) {
    static char buffer[4096] = { 0 };
    KISS_QUEUE q = { 0 };
    KISS_QUEUE_Create(&q, buffer, sizeof(buffer));
    ASSERT_EQ(KISS_QUEUE_GetItemCnt(&q), 0);
    ASSERT_EQ(KISS_QUEUE_GetItemSize(&q), 0);

    KISS_QUEUE_Delete(&q);
}
/* This test validates that the empty queue validates the size of items to be pushed into the queue. */
UTEST(KISS_QUEUE, PutLargeItem) {
    static char buffer[1024] = { 0 };
    KISS_QUEUE q = { 0 };
    KISS_QUEUE_Create(&q, buffer, sizeof(buffer));

    char src[1024];
    int r = KISS_QUEUE_Put(&q, &src, sizeof(src));
    /* This operation should fail. */
    ASSERT_NE(r, 0);
    r = KISS_QUEUE_Put(&q, &src, sizeof(src) / 2);
    ASSERT_EQ(r, 0);
    KISS_QUEUE_Delete(&q);
}

/* This test validates that a single structure can be pushed and popped from the queue */
UTEST(KISS_QUEUE, PushPopItem) {
    typedef struct {
        uint32_t a;
        uint32_t b;
        uint32_t c;
    } obj_t;
    static char buffer[4096] = { 0 };
    KISS_QUEUE q = { 0 };
    KISS_QUEUE_Create(&q, buffer, sizeof(buffer));

    obj_t src = { 99, 22, 88 };
    obj_t* pDst = NULL;
    KISS_QUEUE_Put(&q, &src, sizeof(src));
    ASSERT_EQ(KISS_QUEUE_GetItemCnt(&q), 1);
    ASSERT_EQ(KISS_QUEUE_GetItemSize(&q), sizeof(src));
    int r = KISS_QUEUE_GetPtr(&q, &pDst);
    ASSERT_EQ(r, 0);
    r = KISS_MEMCMP(&src, pDst, sizeof(src));
    ASSERT_EQ(r, 0);
    KISS_QUEUE_Purge(&q);
    ASSERT_EQ(KISS_QUEUE_GetItemCnt(&q), 0);
    KISS_QUEUE_Delete(&q);
}

/* This test validates that multiple items can be pushed to the queue then popped from the queue */
UTEST(KISS_QUEUE, PushPopMany) {
    static char buffer[4096] = { 0 };
    KISS_QUEUE q = { 0 };
    KISS_QUEUE_Create(&q, buffer, sizeof(buffer));

    
    int r = 0;
    for (int i = 0; i < 10; i++) {
        r = KISS_QUEUE_Put(&q, &i, sizeof(i));
        EXPECT_EQ(r, 0);
    }
    int cnt = KISS_QUEUE_GetItemCnt(&q);
    ASSERT_EQ(cnt, 10);

    for (int i = 0; i < cnt; i++) {
        int * v_ptr = NULL;
        /* Retrieve items from the queue. Checking each one */
        r = KISS_QUEUE_GetItemSize(&q);
        EXPECT_EQ(r, sizeof(i));
        r = KISS_QUEUE_GetPtr(&q, &v_ptr);
        ASSERT_NE(v_ptr, NULL);
        EXPECT_EQ(i, *v_ptr);
        KISS_QUEUE_Purge(&q);
    }

    KISS_QUEUE_Delete(&q);
}
/* This test validates that the Queue allows items to be pushed then directly popped */
UTEST(KISS_QUEUE, PushPopInterleaved) {
    static char buffer[4096] = { 0 };
    KISS_QUEUE q = { 0 };
    KISS_QUEUE_Create(&q, buffer, sizeof(buffer));

    int r = 0;
    for (int i = 0; i < 10; i++) {
        r = KISS_QUEUE_Put(&q, &i, sizeof(i));
        EXPECT_EQ(r, 0);
        int* v_ptr = NULL;
        /* Retrieve item from the queue. */
        r = KISS_QUEUE_GetItemSize(&q);
        EXPECT_EQ(r, sizeof(i));
        r = KISS_QUEUE_GetPtr(&q, &v_ptr);
        ASSERT_NE(v_ptr, NULL);
        EXPECT_EQ(i, *v_ptr);
        KISS_QUEUE_Purge(&q);
    }
    int cnt = KISS_QUEUE_GetItemCnt(&q);
    ASSERT_EQ(cnt, 0);

    KISS_QUEUE_Delete(&q);
}

/* This test validates that a queue should not overflow its storage when placing elements of the same size */
UTEST(KISS_QUEUE, FullQueue) {
    KISS_QUEUE q;
    static char buffer[sizeof(KISS_UINT) * 3 * 16] = { 0 };

    int i;
    KISS_QUEUE_Create(&q, buffer, sizeof(buffer));
    EXPECT_EQ(KISS_QUEUE_GetItemCnt(&q), 0);

    for (i = 0; i < 16; ++i) {
        KISS_UINT ch = i;
        int r = KISS_QUEUE_Put(&q, &i, sizeof(ch));
        ASSERT_EQ(r, 0);
    }
    EXPECT_EQ(KISS_QUEUE_GetItemCnt(&q), 16);
    {
        /* The queue is now full. So no more data can be inserted */
        int ch = 0xFF;
        int r = KISS_QUEUE_Put(&q, &ch, sizeof(ch));
        EXPECT_NE(r, 0);
    }
    EXPECT_EQ(KISS_QUEUE_GetItemCnt(&q), 16);

    KISS_QUEUE_Delete(&q);
}

/* This test validates that when the queue is full it will wrap around and start filling from the beginning (if space permits) */
UTEST(KISS_QUEUE, QueueShouldWraparound) {
    KISS_QUEUE q = { 0 };
    char buffer[sizeof(KISS_UINT) * 3 * 16] = { 0 };

    int i;
    KISS_QUEUE_Create(&q, buffer, sizeof(buffer));
    EXPECT_EQ(KISS_QUEUE_GetItemCnt(&q), 0);

    for (i = 0; i < 16; ++i) {
        KISS_UINT ch = i;
        int r = KISS_QUEUE_Put(&q, &i, sizeof(ch));
        ASSERT_EQ(r, 0);
    }
    EXPECT_EQ(KISS_QUEUE_GetItemCnt(&q), 16);
    /* Remove half the items from the queue */
    for(int i = 0; i < 8; ++i)
    {
        KISS_UINT* ptr = NULL;
        int r = KISS_QUEUE_GetPtr(&q, &ptr);
        EXPECT_EQ(r, 0);
        ASSERT_NE(ptr, NULL);
        KISS_QUEUE_Purge(&q);
       
    }
    EXPECT_EQ(KISS_QUEUE_GetItemCnt(&q), 8);
    for (i = 0; i < 8; ++i) {
        KISS_UINT ch = i;
        int r = KISS_QUEUE_Put(&q, &i, sizeof(ch));
        ASSERT_EQ(r, 0);
    }
    EXPECT_EQ(KISS_QUEUE_GetItemCnt(&q), 16);
    

    KISS_QUEUE_Delete(&q);
}



