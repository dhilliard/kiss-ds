#include "utest.h"
#include "../kiss-ds/KISS_ARRAY.h"

UTEST(KISS_ARRAY, CanCreate) {
    uint32_t storage[32] = { 0 };
    KISS_ARRAY a = { 0 };
    KISS_ARRAY_Create(&a, sizeof(uint32_t), 32, storage);

    ASSERT_EQ(KISS_ARRAY_GetItemCapacity(&a), 32);
    ASSERT_EQ(KISS_ARRAY_GetItemCount(&a), 0);
    ASSERT_EQ(KISS_ARRAY_GetItemSize(&a), sizeof(uint32_t));

    KISS_ARRAY_Delete(&a);
}

UTEST(KISS_ARRAY, CanPushItemsToBack) {
    int storage[32] = { 0 };
    KISS_ARRAY a = { 0 };
    KISS_ARRAY_Create(&a, sizeof(int), 32, storage);

    for (int i = 0; i < 32; ++i) {
        int * pItem = (int*)KISS_ARRAY_PutBack(&a, &i);
        ASSERT_NE(pItem, NULL);
        ASSERT_EQ(*pItem, i);
    }
    ASSERT_EQ(KISS_ARRAY_GetItemCount(&a), 32);
    KISS_ARRAY_Delete(&a);
}

UTEST(KISS_ARRAY, CanPushItemsToFront) {
    int storage[32] = { 0 };
    KISS_ARRAY a = { 0 };
    KISS_ARRAY_Create(&a, sizeof(int), 32, storage);

    for (int i = 0; i < 32; ++i) {
        int* pItem = (int*)KISS_ARRAY_PutFront(&a, &i);
        ASSERT_NE(pItem, NULL);
        ASSERT_EQ(*pItem, i);
    }
    ASSERT_EQ(KISS_ARRAY_GetItemCount(&a), 32);
    KISS_ARRAY_Clear(&a);

    KISS_ARRAY_Delete(&a);
}

UTEST(KISS_ARRAY, CanRemoveItems) {
    int storage[32] = { 0 };
    KISS_ARRAY a = { 0 };
    KISS_ARRAY_Create(&a, sizeof(int), 32, storage);
    for (int i = 0; i < 32; ++i) {
        KISS_ARRAY_PutBack(&a, &i);
    }
    ASSERT_EQ(storage[0], 0);
    ASSERT_EQ(storage[31], 31);
    ASSERT_EQ(KISS_ARRAY_GetItemCount(&a), 32);

    /* Remove the first element */
    KISS_ARRAY_Remove(&a, 0, 1);
    ASSERT_EQ(KISS_ARRAY_GetItemCount(&a), 31);
    ASSERT_EQ(storage[0], 31); /* First element should have been changed */
    ASSERT_EQ(storage[1], 1); /* Second element should not have been changed */
    
    /* Remove the first element */
    KISS_ARRAY_Remove(&a, 0, 1);
    ASSERT_EQ(KISS_ARRAY_GetItemCount(&a), 30);
    ASSERT_EQ(storage[0], 30); /* First element should have been changed */
    ASSERT_EQ(storage[1], 1); /* Second element should not have been changed */

    /* Remove the last element */
    KISS_ARRAY_Remove(&a, -1, 1);
    ASSERT_EQ(KISS_ARRAY_GetItemCount(&a), 29);
    ASSERT_EQ(storage[0], 30); /* First element should not have been changed */
    ASSERT_EQ(storage[1], 1); /* Second element should not have been changed */

    KISS_ARRAY_Delete(&a);

}

/* This test case ensures the KISS_ARRAY_Erase() function works as intended. This
* function should preserve the order of elements inserted into the array
*/
UTEST(KISS_ARRAY, CanEraseItems) {
    int storage[32] = { 0 };
    KISS_ARRAY a = { 0 };
    KISS_ARRAY_Create(&a, sizeof(int), 32, storage);
    for (int i = 0; i < 32; ++i) {
        KISS_ARRAY_PutBack(&a, &i);
    }
    ASSERT_EQ(storage[0], 0);
    ASSERT_EQ(storage[1], 1);
    ASSERT_EQ(KISS_ARRAY_GetItemCount(&a), 32);

    /* Remove the first element */
    KISS_ARRAY_Erase(&a, 0, 1);
    ASSERT_EQ(KISS_ARRAY_GetItemCount(&a), 31);
    ASSERT_EQ(storage[0], 1); /* First element should have been changed */
    ASSERT_EQ(storage[1], 2); /* Second element should have been changed */

    /* Remove the last element */
    KISS_ARRAY_Erase(&a, -1, 1);
    ASSERT_EQ(KISS_ARRAY_GetItemCount(&a), 30);
    ASSERT_EQ(storage[0], 1); /* First element should not be changed */
    ASSERT_EQ(storage[1], 2); /* Second element should not be changed */

    KISS_ARRAY_Erase(&a, 0, 5);
    ASSERT_EQ(KISS_ARRAY_GetItemCount(&a), 25);
    ASSERT_EQ(storage[0], 6); /* First element should have been changed */
    ASSERT_EQ(storage[1], 7); /* Second element should have been changed */

    KISS_ARRAY_Delete(&a);
}

UTEST(KISS_ARRAY, CanInsertItemAt) {
    int storage[32] = { 0 };
    KISS_ARRAY a = { 0 };
    KISS_ARRAY_Create(&a, sizeof(int), 32, storage);
    int i = 1;
    KISS_ARRAY_InsertAt(&a, 0, &i);
    ASSERT_EQ(KISS_ARRAY_GetItemCount(&a), 1);
    ASSERT_EQ(storage[0], 1);
    i = 3;
    KISS_ARRAY_InsertAt(&a, -1, &i);
    ASSERT_EQ(KISS_ARRAY_GetItemCount(&a), 2);
    ASSERT_EQ(storage[0], 1);
    ASSERT_EQ(storage[1], 3);
    i = 2;
    KISS_ARRAY_InsertAt(&a, 1, &i);
    ASSERT_EQ(KISS_ARRAY_GetItemCount(&a), 3);
    ASSERT_EQ(storage[0], 1);
    ASSERT_EQ(storage[1], 2);
    ASSERT_EQ(storage[2], 3);

    KISS_ARRAY_Delete(&a);
}

UTEST(KISS_ARRAY, CanSpliceItems)
{
    int storage[32] = { 0 };
    KISS_ARRAY a = { 0 };
    KISS_ARRAY_Create(&a, sizeof(int), 32, storage);
    for (int i = 0; i < 32; ++i) {
        KISS_ARRAY_PutBack(&a, &i);
    }
    ASSERT_EQ(KISS_ARRAY_GetItemCount(&a), 32);
    ASSERT_EQ(storage[0], 0);
    ASSERT_EQ(storage[1], 1);
    ASSERT_EQ(storage[2], 2);
    /* Remove the second element from the array */
    KISS_ARRAY_Splice(&a, 1, 1, NULL, 0);
    ASSERT_EQ(KISS_ARRAY_GetItemCount(&a), 31);
    ASSERT_EQ(storage[0], 0);
    ASSERT_EQ(storage[1], 2);
    ASSERT_EQ(storage[2], 3);

    /* Remove elements using a relative index */
    KISS_ARRAY_Splice(&a, -6, 3, NULL, 0);
    ASSERT_EQ(KISS_ARRAY_GetItemCount(&a), 28);

    /* Add elements to the end of the array */
    uint32_t elems[3] = { 1,2,3 };
    KISS_ARRAY_Splice(&a, -1, 0, elems, 3);
    ASSERT_EQ(KISS_ARRAY_GetItemCount(&a), 31);
    KISS_ARRAY_Delete(&a);
}
