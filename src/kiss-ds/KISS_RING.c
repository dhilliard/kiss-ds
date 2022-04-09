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
*   Component: Ring Buffer
*   File: KISS_RING.c
*   Description:  This file implements the logic for a fixed size ring buffer.
*   Caution/Notes:  None
*=================================================================================*/
#include "KISS_RING.h"

/* ===============================================================================
* Name: KISS_RING_Create()
* Description: Create a ring buffer of fixed size items. 
* Parameters:   [O] pRB - Pointer to ring buffer
*               [I] sizeofMsg - Size of each individual message
*               [I] maxnofmsg - Number of messages in the buffer
*               [I] pBuffer - Pointer to buffer to use for storage
* Return: None
* Caution/Notes: Memory pointed to by pBuffer must be at least sizeofMsg * maxnofmsg bytes.
================================================================================== */
void KISS_RING_Create(KISS_RING* pRB, KISS_UINT16 sizeofMsg, KISS_UINT maxnofmsg, void* pBuffer) {
    KISS_ASSERT(pRB != NULL, "Ring Buffer must be a valid pointer");
    KISS_ASSERT(pBuffer != NULL, "Storage buffer must not be NULL");
    KISS_ASSERT(sizeofMsg > 0, "The size of each individual message cannot be zero");
    KISS_ASSERT(maxnofmsg > 0, "The buffer must have space for at least one element");
    KISS_MEMSET(pRB, 0, sizeof(KISS_RING));
    pRB->pBuffer = pBuffer;
    pRB->MaxCount = maxnofmsg;
    pRB->ItemSize = sizeofMsg;
    pRB->oHead = 0;
    pRB->oTail = 0;
    pRB->NumUsed = 0;
    pRB->UseCount = 0;

}

/* ===============================================================================
* Name: KISS_RING_Delete()
* Description: Deallocate and clear the ring buffer.
* Parameters: [I/O] pRB - Pointer to the Ring Buffer
* Return: None
* Caution/Notes: None
================================================================================== */
void KISS_RING_Delete(KISS_RING* pRB) {
    KISS_ASSERT(pRB != NULL, "Ring Buffer must be a valid pointer");
    pRB->pBuffer = NULL;
    pRB->MaxCount = 0;
    pRB->ItemSize = 0;
    pRB->oHead = 0;
    pRB->oTail = 0;
    pRB->NumUsed = 0;
    pRB->UseCount = 0;
}

/* ===============================================================================
* Name: KISS_RING_Clear()
* Description: Update the head of the ring buffer to remove any data contained therein.
* Parameters: [I/O] pRB - Pointer to the Ring Buffer
* Return: None
* Caution/Notes: Does not modify the memory associated with the slots
================================================================================== */
void KISS_RING_Clear(KISS_RING* pRB) {
    KISS_ASSERT(pRB != NULL, "Ring Buffer must be a valid pointer");
    pRB->oHead = pRB->oTail;
    pRB->NumUsed = 0;
}

/* ===============================================================================
* Name: KISS_RING_Put()
* Description:  Put the element to the end of the ring buffer. 
* Parameters:   [I/O] pRB - Pointer to ring buffer
*               [I] pElement - Pointer to the element to add at the back of the ring buffer.
* Return: KISS_BOOL - Returns 0 on success
* Caution/Notes: Ownership of pElement is borrowed until function returns.
================================================================================== */
KISS_BOOL KISS_RING_Put(KISS_RING* pRB, const void* pElement) {
    KISS_ASSERT(pRB != NULL, "Ring Buffer must be a valid pointer");

    if (pRB->NumUsed < pRB->MaxCount) {
        void* pDest = &pRB->pBuffer[pRB->oTail * pRB->ItemSize];
        KISS_MEMCPY(pDest, pElement, pRB->ItemSize);
        pRB->oTail = (pRB->oTail + 1) % pRB->MaxCount;
        pRB->NumUsed++;
        return 0;
    }

    return 1;
}
/*
* 
* 
*/
/* ===============================================================================
* Name: KISS_RING_Put1()
* Description:  Optimised version of Put for when dealing with single character ring buffer.
*               The element will be inserted at the end of the ring buffer.
* Parameters:   [I/O] pRB - Pointer to ring buffer
*               [I] pElement - Pointer to the element to add at the back of the ring buffer.
* Return: KISS_BOOL - Returns 0 on success
* Caution/Notes: Ownership of pElement is borrowed until function returns.
================================================================================== */
KISS_BOOL KISS_RING_Put1(KISS_RING* pRB, const char* pElement) {
    KISS_ASSERT(pRB != NULL, "Ring Buffer must be a valid pointer");

    if (pRB->NumUsed < pRB->MaxCount) {
        pRB->pBuffer[pRB->oTail * pRB->ItemSize] = (uint8_t)*pElement;
        pRB->oTail = (pRB->oTail + 1) % pRB->MaxCount;
        pRB->NumUsed++;
        return 0;
    }

    return 1;

}

/* ===============================================================================
* Name: KISS_RING_PutFront1()
* Description: Insert an element at the start of the ring buffer. 
* Parameters:   [I/O] pRB - Pointer to ring buffer
*               [I] pElement - Pointer to data to add to front of ring buffer
* Return: KISS_BOOL - Returns 0 on success
* Caution/Notes: Ownership of pElement is borrowed until function returns.
================================================================================== */
KISS_BOOL KISS_RING_PutFront(KISS_RING* pRB, const void* pElement) {
    KISS_ASSERT(pRB != NULL, "Ring Buffer must be a valid pointer");

    if (pRB->NumUsed < pRB->MaxCount) {
        /* Instead of adjusting the tail offset KISS_RING_PutFront1 adjusts the head offset */
        pRB->oHead = (pRB->oHead + pRB->MaxCount - 1) % pRB->MaxCount;
        void* pDest = &pRB->pBuffer[pRB->oHead * pRB->ItemSize];
        KISS_MEMCPY(pDest, pElement, pRB->ItemSize);
        pRB->NumUsed++;
        return 0;
    }
    return 1;
}

/* ===============================================================================
* Name: KISS_RING_PutFront1()
* Description: Optimised version of KISS_RING_PutFront for single character ring buffers.
* Parameters:   [I/O] pRB - Pointer to ring buffer
*               [I] pElement - Pointer to element to insert.
* Return: KISS_BOOL - Returns 0 on success.
* Caution/Notes: None
================================================================================== */
KISS_BOOL KISS_RING_PutFront1(KISS_RING* pRB, const char* pElement) {
    KISS_ASSERT(pRB != NULL, "Ring Buffer must be a valid pointer");

    if (pRB->NumUsed < pRB->MaxCount) {
        /* Instead of adjusting the tail offset KISS_RING_PutFront1 adjusts the head offset */
        pRB->oHead = (pRB->oHead + pRB->MaxCount - 1) % pRB->MaxCount;
        pRB->pBuffer[pRB->oHead * pRB->ItemSize] = (uint8_t)*pElement;
        pRB->NumUsed++;
        return 0;
    }
    return 1;
}

/* ===============================================================================
* Name: KISS_RING_Get()
* Description: Get the top most element of the ring buffer. Will copy data into supplied buffer
* Parameters:   [I/O] pRB - Pointer to the ring buffer.
*               [O] pData - Pointer to memory to receive the top most element.
* Return: KISS_BOOL - Returns 0 on success
* Caution/Notes: This function copies the top-most element to the memory provided.
*                As such the memory pointed to by pDest should be at least 
*                ItemSize bytes long
================================================================================== */
KISS_BOOL KISS_RING_Get(KISS_RING* pRB, void* pData) {
    KISS_ASSERT(pRB != NULL, "Ring Buffer must be a valid pointer");

    if (pRB->NumUsed > 0 && pData != NULL) {
        int oNext = (pRB->oHead + 1) % pRB->MaxCount;
        void* pSrc = &pRB->pBuffer[pRB->oHead * pRB->ItemSize];
        KISS_MEMCPY(pData, pSrc, pRB->ItemSize);
        pRB->oHead = oNext;
        pRB->NumUsed--;
        return 0;
    }
    return 1;
}
/* ===============================================================================
* Name: KISS_RING_Get1()
* Description: Get the front most element from the ring buffer.
* Parameters:   [I/O] pRB - Pointer to the ring buffer
*               [O] pData - Pointer to memory to receive the top most element.
* Return: KISS_BOOL - Returns 0 on success.
* Caution/Notes:    This function is a specialised version of KISS_RING_Get() which 
*                    is optimised for character ring buffers
================================================================================== */
KISS_BOOL KISS_RING_Get1(KISS_RING* pRB, char* pData) {
    KISS_ASSERT(pRB != NULL, "Ring Buffer must be a valid pointer");

    if (pRB->NumUsed > 0 && pData != NULL) {
        int oNext = (pRB->oHead + 1) % pRB->MaxCount;
        *pData = (char)pRB->pBuffer[pRB->oHead * pRB->ItemSize];
        pRB->oHead = oNext;
        pRB->NumUsed--;
        return 0;
    }
    return 1;
}
/*
* 
*/
/* ===============================================================================
* Name: KISS_RING_Peek()
* Description: Get the top-most element of the ring buffer without removing it.
* Parameters:   [I] pRB - Pointer to the ring buffer.
*               [O] pDest - Pointer to buffer to receive the element.
* Return: KISS_BOOL - Returns 0 on success.
* Caution/Notes: This function copies the top-most element to the memory provided.
*                As such the memory pointed to by pDest should be at least 
*                ItemSize bytes long
================================================================================== */
KISS_BOOL KISS_RING_Peek(const KISS_RING* pRB, void* pDest) {
    KISS_ASSERT(pRB != NULL, "Ring Buffer must be a valid pointer");
    if (pRB->NumUsed > 0) {
        void* pSrc = &pRB->pBuffer[pRB->oHead * pRB->ItemSize];
        KISS_MEMCPY(pDest, pSrc, pRB->ItemSize);
        return 0;
    }
    return 1;
}

/* ===============================================================================
* Name: KISS_RING_GetPtr()
* Description: Get a pointer to the message at the front of the ring buffer. 
* Parameters: [I] pRB - Pointer to the ring buffer
*             [O] ppDest - Pointer to pointer to the first element.
* Return: KISS_BOOL - Returns 0 on success
* Caution/Notes: This function locks the front of the ring buffer and it remains locked
*                until KISS_RING_Purge() is called to remove the element.
================================================================================== */
KISS_BOOL KISS_RING_GetPtr(KISS_RING* pRB, void** ppDest) {
    KISS_ASSERT(pRB != NULL, "Ring Buffer must be a valid pointer");
    if (pRB->NumUsed > 0 && ppDest != NULL) {
        pRB->UseCount++;
        *ppDest = &pRB->pBuffer[pRB->oHead * pRB->ItemSize];
        return 0;
    }
    return 1;
}

/* ===============================================================================
* Name: KISS_RING_Purge()
* Description: Remove the front element from the RING Buffer. Used in conjunction with KISS_RING_GetPtr()
* Parameters: [I/O] pRB - Pointer to the ring buffer
* Return: None
* Caution/Notes: KISS_RING_GetPtr() should be called before this function.
================================================================================== */
void KISS_RING_Purge(KISS_RING* pRB) {
    KISS_ASSERT(pRB != NULL, "Ring Buffer must be a valid pointer");
    if (pRB->UseCount > 0) {
        pRB->UseCount--;
    }
    if (pRB->UseCount == 0) {
        if (pRB->NumUsed > 0) {
            pRB->oHead = (pRB->oHead + 1) % pRB->MaxCount;
            pRB->NumUsed--;
        }
    }
}

/* ===============================================================================
* Name: KISS_RING_GetItemCnt()
* Description: Get the number of elements in the ring buffer.
* Parameters: [I] pRB - Pointer to the ring buffer
* Return: int - Number of elements in the ring buffer
* Caution/Notes: None
================================================================================== */
int KISS_RING_GetItemCnt(const KISS_RING* pRB) {
    KISS_ASSERT(pRB != NULL, "Ring Buffer must be a valid pointer");
    return pRB->NumUsed;
}
