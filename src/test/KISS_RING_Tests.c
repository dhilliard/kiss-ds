#include "utest.h"
#include "../kiss-ds/KISS_RING.h"




UTEST(KISS_RING,PutCharactersThenGetCharacters) {
    KISS_RING mb;
    static char buffer[4096];
    const char input[] = "Retrieve the characters directly from the buffer!";
    char output[100] = { 0 };
    const size_t len = strlen(input);
    int i;
    KISS_RING_Create(&mb, 1, sizeof(buffer), buffer);
    EXPECT_EQ(KISS_RING_GetItemCnt(&mb), 0);

    for (i = 0; i < len; ++i) {
        char ch = input[i];
        int r = KISS_RING_Put1(&mb, &ch);
        ASSERT_EQ(r, 0);
    }

    EXPECT_EQ(len, KISS_RING_GetItemCnt(&mb));

    i = 0;
    while (KISS_RING_GetItemCnt(&mb) > 0) {
        char ch = 0;
        int r = KISS_RING_Get1(&mb, &ch);
        output[i] = ch;
        ASSERT_EQ(r, 0);
        i++;
    }
    EXPECT_STREQ(input, output);

    KISS_RING_Delete(&mb);
}
UTEST(KISS_RING, GetStringByPointer) {
    KISS_RING mb;
    static char buffer[4096];
    const char input[] = "Retrieve a string pointer from the mailbox!";
    char output[100] = { 0 };
    const size_t len = strlen(input);
    int i;
    KISS_RING_Create(&mb, 1, sizeof(buffer), buffer);
    EXPECT_EQ(KISS_RING_GetItemCnt(&mb), 0);

    for (i = 0; i < len; ++i) {
        char ch = input[i];
        int r = KISS_RING_Put1(&mb, &ch);
        ASSERT_EQ(r, 0);
    }

    EXPECT_EQ(len, KISS_RING_GetItemCnt(&mb));

    i = 0;
    while (KISS_RING_GetItemCnt(&mb) > 0) {
        char * ch = NULL;
        int r = KISS_RING_GetPtr(&mb, &ch);
        ASSERT_NE(ch, NULL);
        output[i] = *ch;
        ASSERT_EQ(r, 0);
        i++;
        KISS_RING_Purge(&mb);
    }
    EXPECT_STREQ(input, output);

    KISS_RING_Delete(&mb);
}

UTEST(KISS_RING, FullRingBuffer) {
    KISS_RING mb;
    static char buffer[16];

    int i;
    KISS_RING_Create(&mb, 1, sizeof(buffer), buffer);
    EXPECT_EQ(KISS_RING_GetItemCnt(&mb), 0);

    for (i = 0; i < sizeof(buffer); ++i) {
        char ch = i;
        int r = KISS_RING_Put1(&mb, &ch);
        ASSERT_EQ(r, 0);
    }
    EXPECT_EQ(KISS_RING_GetItemCnt(&mb), sizeof(buffer));
    {
        /* The queue is now full. So no more data can be inserted */
        char ch = 0xFF;
        int r = KISS_RING_Put1(&mb, &ch);
        EXPECT_NE(r, 0);
    }
    EXPECT_EQ(KISS_RING_GetItemCnt(&mb), sizeof(buffer));

    KISS_RING_Delete(&mb);
}

UTEST(KISS_RING, RingBufferShouldWrapAround) {
    KISS_RING mb;
    static char buffer[16];

    KISS_RING_Create(&mb, 1, sizeof(buffer), buffer);
    ASSERT_EQ(KISS_RING_GetItemCnt(&mb), 0);

    for (int i = 0; i < sizeof(buffer); ++i) {
        char ch = i;
        int r = KISS_RING_Put1(&mb, &ch);
        ASSERT_EQ(r, 0);
    }
    EXPECT_EQ(KISS_RING_GetItemCnt(&mb), sizeof(buffer));
    /* Drain half the queue*/
    for(int i = 0; i < sizeof(buffer) / 2; ++i)
    {
       
        char ch = 0xFF;
        int r = KISS_RING_Get1(&mb, &ch);
        EXPECT_EQ(r, 0);
    }
    EXPECT_EQ(KISS_RING_GetItemCnt(&mb), sizeof(buffer) / 2);
    /* Try adding more items */
    for (int i = 0; i < sizeof(buffer) / 2; ++i)
    {
        char ch = i;
        int r = KISS_RING_Put1(&mb, &ch);
        ASSERT_EQ(r, 0);
    }
    KISS_RING_Delete(&mb);
}
UTEST(KISS_RING, PutFrontShouldWork) {
    KISS_RING mb;
    static char buffer[4096];
    const char input[] = "Push string character by character to front of mailbox!";
    char output[100] = { 0 };
    const size_t len = strlen(input);
    int i;
    KISS_RING_Create(&mb, 1, sizeof(buffer), buffer);
    EXPECT_EQ(KISS_RING_GetItemCnt(&mb), 0);

    for (i = 0; i < len; ++i) {
        char ch = input[len - i - 1];
        int r = KISS_RING_PutFront1(&mb, &ch);
        ASSERT_EQ(r, 0);
    }

    ASSERT_EQ(len, KISS_RING_GetItemCnt(&mb));

    i = 0;
    while (KISS_RING_GetItemCnt(&mb) > 0) {
        char* ch = NULL;
        int r = KISS_RING_GetPtr(&mb, &ch);
        ASSERT_NE(ch, NULL);
        EXPECT_NE(*ch, '\0');
        output[i] = *ch;
        ASSERT_EQ(r, 0);
        i++;
        KISS_RING_Purge(&mb);
    }
    EXPECT_STREQ(input, output);

    KISS_RING_Delete(&mb);
}

