#include "utest.h"
#include "../kiss-ds/KISS_ARENA.h"
#include <stdio.h>

UTEST(KISS_ARENA, Can_Create) {
    KISS_UINT buffer[1024 / sizeof(KISS_UINT)] = { 0 };
    KISS_ARENA arena;
    KISS_ARENA_Create(&arena, buffer, sizeof(buffer));
    EXPECT_EQ(KISS_ARENA_BytesAllocated(&arena), 0);
    KISS_ARENA_Delete(&arena);
}

UTEST(KISS_ARENA, Can_Allocate) {
    KISS_UINT buffer[1024 / sizeof(KISS_UINT)] = { 0 };
    KISS_ARENA arena;
    KISS_ARENA_Create(&arena, buffer, sizeof(buffer));
    for (int i = 0; i < 20; ++i) {
        KISS_UINT* pValue = (KISS_UINT*)KISS_ARENA_Alloc(&arena, sizeof(KISS_UINT));
        ASSERT_NE(pValue, NULL);
        *pValue = i;
    }
    EXPECT_EQ(KISS_ARENA_BytesAllocated(&arena), 20 * sizeof(KISS_UINT));

    KISS_ARENA_Clear(&arena);
    EXPECT_EQ(KISS_ARENA_BytesAllocated(&arena), 0);

    KISS_ARENA_Delete(&arena);
}
UTEST(KISS_ARENA, WillNotOverflow) {
    KISS_UINT buffer[64 / sizeof(KISS_UINT)] = { 0 };
    KISS_ARENA arena;
    KISS_ARENA_Create(&arena, buffer, sizeof(buffer));
    const KISS_UINT cnt = 64 / sizeof(KISS_UINT);
    for (KISS_UINT i = 0; i < cnt; ++i) {
        KISS_UINT* pValue = (KISS_UINT*)KISS_ARENA_Alloc(&arena, sizeof(KISS_UINT));
        if (pValue == NULL) {
            printf("Value is null: %d\n", i);
        }
        ASSERT_NE(pValue, NULL);

        *pValue = i;
    }
    EXPECT_EQ(KISS_ARENA_BytesAllocated(&arena), 64);

    KISS_UINT* pValue = (KISS_UINT*)KISS_ARENA_Alloc(&arena, sizeof(KISS_UINT));
    ASSERT_EQ(pValue, NULL);

    KISS_ARENA_Delete(&arena);
}

