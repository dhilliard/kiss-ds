/*================================================================================
*   zlib/libpng license
*
*   Copyright (c) 2021. Denis Hilliard
*
*   This software is provided 'as-is', without any express or implied warranty.
*    In no event will the authors be held liable for any damages arising from the
*    use of this software.
*
*    Permission is granted to anyone to use this software for any purpose,
*    including commercial applications, and to alter it and redistribute it
*    freely, subject to the following restrictions:
*
*        1. The origin of this software must not be misrepresented; you must not
*        claim that you wrote the original software. If you use this software in a
*        product, an acknowledgment in the product documentation would be
*        appreciated but is not required.
*
*        2. Altered source versions must be plainly marked as such, and must not
*        be misrepresented as being the original software.
*
*        3. This notice may not be removed or altered from any source
*        distribution.
*
*   Component: Fixed Size Single Ended Queue
*   File: KISS_QUEUE.c
*   Description:  This file implements the logic for a single ended queue object.
*   Caution/Notes:  None
*=================================================================================*/
#include "KISS_QUEUE.h"

/* TODO: FIX KISS_QUEUE to use a single KISS_UINT16 to indicate the length of the record */
typedef struct {
    KISS_UINT oNext;
    KISS_UINT oSize;
} KISS_QUEUE_ITEM;



/* Internal function shared between KISS_QUEUE_Put and KISS_QUEUE_PutEx to allocate a new queue item within the buffer */
static void* __alloc_item(KISS_QUEUE* pQ, KISS_UINT Size);

/* ===============================================================================
* Name: KISS_QUEUE_Create()
* Description: Initialise the array with a fixed size / preallocated memory buffer
* Parameters:   [O] pQ - Pointer to the Array to initialise
*               [I] pData - Pointer to buffer to use for queue storage
*               [I] Size - The size of each element.           
* Return: None
* Caution/Notes:    None
================================================================================== */
void KISS_QUEUE_Create(KISS_QUEUE* pQ, void* pData, KISS_UINT Size) {
    KISS_ASSERT(pQ != NULL, "Queue must be a valid pointer");
    pQ->pBuffer = pData;
    pQ->TotalSize = Size;
    pQ->oHead = 0;
    pQ->oTail = 0;
    pQ->NumElements = 0;
    pQ->UseCount = 0;
}
/* ===============================================================================
* Name: KISS_QUEUE_Delete()
* Description: Deallocate the queue
* Parameters:   [O] pQ - Pointer to the queue to deallocate
* Return: None
* Caution/Notes:    None
================================================================================== */
void KISS_QUEUE_Delete(KISS_QUEUE* pQ) {
    KISS_ASSERT(pQ != NULL, "Queue must be a valid pointer");
    pQ->pBuffer = NULL;
    pQ->TotalSize = 0;
    pQ->oHead = 0;
    pQ->oTail = 0;
    pQ->NumElements = 0;
    pQ->UseCount = 0;
}
/* ===============================================================================
* Name: KISS_QUEUE_Clear()
* Description: Reset the buffer to the default empty state.
* Parameters:   [O] pQ - Pointer to to the queue to modify
* Return: None
* Caution/Notes:    None
================================================================================== */
void KISS_QUEUE_Clear(KISS_QUEUE* pQ) {
    KISS_ASSERT(pQ != NULL, "Queue must be a valid pointer");
    pQ->oHead = 0;
    pQ->oTail = 0;
    pQ->NumElements = 0;
}
/* ===============================================================================
* Name: KISS_QUEUE_Put()
* Description: Put an item to the end of the queue.
* Parameters: [I/O] pQ - Pointer to the queue to modify
*               [I] pSrc - Pointer to data to add to queue
*               [I] Size - Size of data to add to queue
* Return: int - Returns 0 on success.
* Caution/Notes:    None
================================================================================== */
int KISS_QUEUE_Put(KISS_QUEUE* pQ, const void* pSrc, KISS_UINT Size) {
    KISS_ASSERT(pQ != NULL, "Queue must be a valid pointer");
    /* Attempt to add an item to the queue */
    void* pDest = __alloc_item(pQ, Size);
    if (pDest != NULL) {
        /* Copy the data from the source area into the new memory location */
        KISS_MEMCPY(pDest, pSrc, Size);
        return 0;
    }
    return 1;
}
/* ===============================================================================
* Name: KISS_QUEUE_PutEx()
* Description:  Put an item to the end of the queue, assembling it in place from
*               each source component.
* Parameters:   [I/O] pQ - Pointer to the queue to modify
*               [I] pSrcList - Array of source components to build the queue data from.
*               [I[ NumSrc - Number of elements in the source array.
* Return: int - Returns 0 on success.
* Caution/Notes:    None
================================================================================== */
int KISS_QUEUE_PutEx(KISS_QUEUE* pQ, const KISS_QUEUE_SRCLIST* pSrcList, KISS_UINT NumSrc) {

    KISS_ASSERT(pQ != NULL, "Queue must be a valid pointer");
    if (pSrcList != NULL) {
        KISS_UINT Size = 0;
        /* Compute the total size of the elements */
        for (KISS_UINT i = 0; i < NumSrc; ++i) {
            Size += pSrcList[i].Size;
        }
        /* Attempt to add an item to the queue */
        uint8_t* pDest = (uint8_t*)__alloc_item(pQ, Size);

        if (pDest != NULL) {
            /* Copy data from each chunk into the destination memory chunk */
            for (KISS_UINT i = 0; i < NumSrc; ++i) {
                KISS_MEMCPY(pDest, pSrcList[i].pSrc, pSrcList[i].Size);
                pDest += pSrcList[i].Size;
            }
            return 0;
        }
    }

    return 1;
}
/* ===============================================================================
* Name: KISS_QUEUE_GetPtr()
* Description: Get the pointer to first element stored in the queue
* Parameters:   [I/O] pQ - Pointer to the queue to retrieve data from.
*               [O] ppData - Pointer to pointer to receive the first 
                    element from the queue.
* Return: int - Returns 0 on success.
* Caution/Notes:    None
================================================================================== */
int KISS_QUEUE_GetPtr(KISS_QUEUE* pQ, void** ppData) {
    KISS_ASSERT(pQ != NULL, "Queue must be a valid pointer");
    if (pQ->NumElements > 0) {
        pQ->UseCount++;
        KISS_ASSERT(pQ->UseCount != 0, "Queue should be in use");
        *ppData = &pQ->pBuffer[(pQ->oHead + sizeof(KISS_QUEUE_ITEM))];
        return 0;
    }
    return 1;
}
/* ===============================================================================
* Name: KISS_QUEUE_Purge()
* Description: Remove the top most element from the queue.
* Parameters: [I/O] pQ - Pointer to the queue to modify
* Return: None
* Caution/Notes: The element should have been retrieved first by KISS_QUEUE_GetPtr()
================================================================================== */
void KISS_QUEUE_Purge(KISS_QUEUE* pQ) {
    KISS_ASSERT(pQ != NULL, "Queue must be a valid pointer");

    if (pQ->NumElements > 0) {
        if (pQ->UseCount > 0) {
            pQ->UseCount--;
            KISS_ASSERT(pQ->UseCount == 0, "Queue is currently in use");
        }
        /* Retrieve the next element offset from the current head & update the queue */
        KISS_QUEUE_ITEM* pHead = (KISS_QUEUE_ITEM*)&pQ->pBuffer[pQ->oHead];
        pQ->oHead = pHead->oNext;
        pQ->NumElements--;
    }
}
/* ===============================================================================
* Name: KISS_QUEUE_GetItemCnt()
* Description: Get the number of elements currently in the queue.
* Parameters: [I] pQ - Pointer to the queue
* Return: int - Returns number of elements in the queue
* Caution/Notes:    None
================================================================================== */
int KISS_QUEUE_GetItemCnt(const KISS_QUEUE* pQ) {
    KISS_ASSERT(pQ != NULL, "Queue must be a valid pointer");
    return pQ->NumElements;
}
/* ===============================================================================
* Name: KISS_QUEUE_IsInUse()
* Description: Gets whether the queue is currently in use. This condition is
*               required in order to subsequently purge the top most item.
* Parameters: [I] pQ - Pointer to the queue
* Return: KISS_BOOL - Returns true if the queue is in use.
* Caution/Notes:    None
================================================================================== */
KISS_BOOL KISS_QUEUE_IsInUse(const KISS_QUEUE* pQ) {
    KISS_ASSERT(pQ != NULL, "Queue must be a valid pointer");
    return pQ->UseCount != 0;
}
/* ===============================================================================
* Name: KISS_QUEUE_GetItemSize()
* Description: Gets the size of the first/top item in the queue.
* Parameters: [I] pQ - Pointer to the queue.
* Return: int - Returns the size on the top item in the queue.
* Caution/Notes:    None
================================================================================== */
int KISS_QUEUE_GetItemSize(const KISS_QUEUE* pQ) {
    KISS_ASSERT(pQ != NULL, "Queue must be a valid pointer");
    if (pQ->NumElements > 0) {
        const KISS_QUEUE_ITEM* pHead = (KISS_QUEUE_ITEM*)&pQ->pBuffer[pQ->oHead];
        return pHead->oSize;
    }
    return 0;
}
/* ===============================================================================
* Name: KISS_QUEUE_PeekPtr()
* Description:  Gets the pointer to the top-most item of the queue but does not modify
*               the underlying queue.
* Parameters:   [I] pQ - Pointer to the queue
*               [O] ppData - Pointer to pointer to top of queue.
* Return: int - Returns 0 on success.
* Caution/Notes:    None
================================================================================== */
int KISS_QUEUE_PeekPtr(const KISS_QUEUE* pQ, void** ppData) {
    KISS_ASSERT(pQ != NULL, "Queue must be a valid pointer");
    if (pQ->NumElements > 0) {
        *ppData = &pQ->pBuffer[(pQ->oHead + sizeof(KISS_QUEUE_ITEM))];
        return 0;
    }
    return 1;
}

/* ===============================================================================
* Name: __alloc_item()
* Description:  Internal function shared between KISS_QUEUE_Put and KISS_QUEUE_PutEx 
                to allocate a new queue item within the buffer
* Parameters:   [O] pQ - Pointer to queue to modify
*               [I] Size - Size of item to add to the queue
* Return: void* - Pointer to the newly allocated item in the queue.
* Caution/Notes:  
================================================================================== */
static void* __alloc_item(KISS_QUEUE* pQ, KISS_UINT Size) {
    KISS_QUEUE_ITEM* pLast = (KISS_QUEUE_ITEM*)&pQ->pBuffer[pQ->oTail];
    KISS_UINT oNext = pLast->oNext;
    const KISS_UINT oNextNext = (oNext + sizeof(KISS_QUEUE_ITEM) + Size);

    if ((oNextNext <= pQ->TotalSize) || (oNextNext <= pQ->oHead)) {
        /* End of buffer hasn't been reached or oNext has already been wrapped around: nothing to do. */
    }
    else if ((0 + sizeof(KISS_QUEUE_ITEM) + Size) < pQ->oHead) {
        /* Wrap oNext around to the beginning of the buffer */
        oNext = 0;
        pLast->oNext = oNext;
    }
    else {
        /* Unable to fit the item in the queue */
        return NULL;
    }
    KISS_QUEUE_ITEM* pNext = (KISS_QUEUE_ITEM*)&pQ->pBuffer[oNext];
    void* pDest = ((uint8_t*)pNext) + sizeof(KISS_QUEUE_ITEM);
    pNext->oNext = oNext + sizeof(KISS_QUEUE_ITEM) + Size;
    pNext->oSize = Size;

    /* Update variables in the queue object */
    pQ->oTail = oNext;
    pQ->NumElements++;

    return pDest;
}
