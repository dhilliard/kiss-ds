#include "utest.h"
#include "../kiss-ds/KISS_BLOCKPOOL.h"

/* Convenience macro for declaring memory pool storage */
#define DECLARE_POOL_MEMORY(Name, Count, Size) KISS_UINT Name[Count][Size / sizeof(KISS_UINT)]

/* This test ensures a Memory Pool can be created */
UTEST(KISS_BLOCKPOOL, Can_Be_Created) {
    DECLARE_POOL_MEMORY(pool, 32, 64);

    KISS_BLOCKPOOL mp = { 0 };
    KISS_BLOCKPOOL_Create(&mp, pool, 32, 64);
    EXPECT_EQ(KISS_BLOCKPOOL_GetNumBlocks(&mp), 32);
    EXPECT_EQ(KISS_BLOCKPOOL_GetBlockSize(&mp), 64);
    EXPECT_EQ(KISS_BLOCKPOOL_GetMaxUsed(&mp), 0);
    EXPECT_EQ(KISS_BLOCKPOOL_GetNumFreeBlocks(&mp), 32);
    KISS_BLOCKPOOL_Delete(&mp);

}

/* This test ensures a single item can be allocated, then freed & the maximum number of items allocated works as expected */
UTEST(KISS_BLOCKPOOL, Can_Allocate_Item) {
    DECLARE_POOL_MEMORY(pool,32,64);
    KISS_BLOCKPOOL mp = { 0 };
    KISS_BLOCKPOOL_Create(&mp, pool, 32, 64);

    {
        void* pAlloc = KISS_BLOCKPOOL_Alloc(&mp);
        ASSERT_NE(pAlloc, NULL);
        EXPECT_EQ(KISS_BLOCKPOOL_GetMaxUsed(&mp), 1);
        EXPECT_EQ(KISS_BLOCKPOOL_GetNumFreeBlocks(&mp), 31);
        EXPECT_TRUE(KISS_BLOCKPOOL_IsInPool(&mp, pAlloc));
        KISS_BLOCKPOOL_FreeEx(&mp, pAlloc);
        EXPECT_EQ(KISS_BLOCKPOOL_GetMaxUsed(&mp), 1);
        EXPECT_EQ(KISS_BLOCKPOOL_GetNumFreeBlocks(&mp), 32);
    }
    
    {
        void* pAlloc = KISS_BLOCKPOOL_Alloc(&mp);
        ASSERT_NE(pAlloc, NULL);
        EXPECT_EQ(KISS_BLOCKPOOL_GetMaxUsed(&mp), 1);
        EXPECT_EQ(KISS_BLOCKPOOL_GetNumFreeBlocks(&mp), 31);
        EXPECT_TRUE(KISS_BLOCKPOOL_IsInPool(&mp, pAlloc));
    }
    KISS_BLOCKPOOL_Delete(&mp);

}

/* Ensure that blocks can be allocated at returned to the pool repeatedly without breaking */
UTEST(KISS_BLOCKPOOL, Can_Alloc_and_Free_Repeatedly) {
    DECLARE_POOL_MEMORY(pool, 32, 64);
    KISS_BLOCKPOOL mp = { 0 };
    KISS_BLOCKPOOL_Create(&mp, pool, 32, 64);
    void* pAllocated[32] = { 0 };
    const int items = 16;
    for (int j = 0; j < 4; ++j) {
       
        for (int i = 0; i < items; ++i) {
            void* pAlloc = KISS_BLOCKPOOL_Alloc(&mp);
            ASSERT_NE(pAlloc, NULL);

            EXPECT_TRUE(KISS_BLOCKPOOL_IsInPool(&mp, pAlloc));
            pAllocated[i] = pAlloc;
        }

        //KISS_BLOCKPOOL_FreeEx(&mp, pAlloc);
        EXPECT_EQ(KISS_BLOCKPOOL_GetMaxUsed(&mp), items);
        EXPECT_EQ(KISS_BLOCKPOOL_GetNumFreeBlocks(&mp), items);

        for (int i = 0; i < items; ++i) {
            KISS_BLOCKPOOL_FreeEx(&mp, pAllocated[i]);
            pAllocated[i] = NULL;
            ASSERT_EQ(KISS_BLOCKPOOL_GetNumFreeBlocks(&mp), i + items + 1);
        }
    }


    KISS_BLOCKPOOL_Delete(&mp);

}

/* Additional Tests:
* -- Ensure Double Frees cannot occur
* -- Alloc a random number of elements, free a random number
* -- Out of order deallocations.
*/
