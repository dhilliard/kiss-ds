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
*   Component: Fixed Size Block Pool Allocator
*   File: KISS_BLOCKPOOL.h
*   Description:    This file implements the logic for a Pool Allocator. It manages
*                   a collection of fixed size blocks from which consumers can
*                   allocate and free blocks.
*   Caution/Notes:  None
*=================================================================================*/
#include "KISS_BLOCKPOOL.h"

/* ===============================================================================
* Name: KISS_BLOCKPOOL_Create()
* Description: Initialise the block pool object.
* Parameters:   [O] pBLOCK - Pointer to the memory pool to initialise
*               [I] pPool - Pointer to storage to use for the memory pool
*               [I] NumBlocks - Number of blocks within the pool
*               [I] BlockSize - Size of each block within the block pool
* Return: None
* Caution/Notes:  pPool must point to a block of memory of at least 
*                    NumBlocks * BlockSize bytes
================================================================================== */
void KISS_BLOCKPOOL_Create(KISS_BLOCKPOOL* pBLOCK, void* pPool, KISS_UINT NumBlocks, KISS_UINT BlockSize) {
    KISS_ASSERT(pBLOCK != NULL, "BLOCKPOOL must be a valid pointer");
    pBLOCK->pPool = pPool;
    pBLOCK->pHead = NULL;
    pBLOCK->pTail = NULL;
    pBLOCK->MaxUsed = 0;
    pBLOCK->NumBlocks = NumBlocks;
    pBLOCK->BlocksUsed = 0;
    pBLOCK->BlockSize = BlockSize;
    KISS_MEMSET(pPool, 0xCDCDCDCD, (size_t)NumBlocks * BlockSize);
}

/* ===============================================================================
* Name: KISS_BLOCKPOOL_Delete()
* Description: Cleanup/Deallocate the provided block pool
* Parameters: [O] pBLOCK - Pointer to block pool to delete
* Return: None
* Caution/Notes: None
================================================================================== */
void KISS_BLOCKPOOL_Delete(KISS_BLOCKPOOL* pBLOCK) {
    KISS_ASSERT(pBLOCK != NULL, "BLOCKPOOL must be a valid pointer");
    pBLOCK->pPool = NULL;
    pBLOCK->pHead = NULL;
    pBLOCK->pTail = NULL;
    pBLOCK->MaxUsed = 0;
    pBLOCK->NumBlocks = 0;
    pBLOCK->BlocksUsed = 0;
    pBLOCK->BlockSize = 0;
}

/* ===============================================================================
* Name: KISS_BLOCKPOOL_Alloc()
* Description: Allocate a single fixed size block from the memory pool
* Parameters: [I/O] pBlock - Pointer to block pool to allocate from.
* Return: void* - Returns pointer to freshly allocated memory buffer or 
                    NULL if unsuccessful.
* Caution/Notes: None
================================================================================== */
void* KISS_BLOCKPOOL_Alloc(KISS_BLOCKPOOL* pBLOCK) 
{
    KISS_ASSERT(pBLOCK != NULL, "BLOCKPOOL must be a valid pointer");
    void* pResult = NULL;
    
    if (pBLOCK->BlocksUsed < pBLOCK->NumBlocks) {
        
        if ((pBLOCK->MaxUsed - pBLOCK->BlocksUsed) > 0 && pBLOCK->pHead != NULL) {
            pBLOCK->BlocksUsed++;
            pResult = pBLOCK->pHead;
            if (pBLOCK->pHead != pBLOCK->pTail) {
                pBLOCK->pHead = *((uintptr_t**)pResult);
            } else {
                pBLOCK->pHead = pBLOCK->pTail = NULL;
            }
            

        }
        else if ((pBLOCK->BlocksUsed + 1) > pBLOCK->MaxUsed) {
            pBLOCK->BlocksUsed++;
            pResult = &pBLOCK->pPool[pBLOCK->MaxUsed * pBLOCK->BlockSize];
            KISS_MEMSET(pResult, 0, pBLOCK->BlockSize);
            pBLOCK->MaxUsed = pBLOCK->BlocksUsed;
        }
    }
    return pResult;
}
/* ===============================================================================
* Name: KISS_BLOCKPOOL_FreeEx()
* Description: Free the specified block contained within the block pool
* Parameters:   [I/O] pBLOCK - Pointer to memory block to free block from
*               [I] pMemBlock - The memory block to free.
* Return: None
* Caution/Notes: None
================================================================================== */
void KISS_BLOCKPOOL_FreeEx(KISS_BLOCKPOOL* pBLOCK, void* pMemBlock) {
    KISS_ASSERT(pBLOCK != NULL, "BLOCKPOOL must be a valid pointer");
    
    /* Rather than prebuilding a list of unused blocks, the free list is built as blocks are returned */
    if ((pBLOCK->BlocksUsed > 0) && KISS_BLOCKPOOL_IsInPool(pBLOCK, pMemBlock)) {
        
        *((uintptr_t**)pMemBlock) = NULL;
        if ((pBLOCK->MaxUsed - pBLOCK->BlocksUsed) > 0) {
            *((uintptr_t**)pBLOCK->pTail) = pMemBlock;
            pBLOCK->pTail = pMemBlock;
        }
        else {
            pBLOCK->pHead = pBLOCK->pTail = pMemBlock;
        }
        pBLOCK->BlocksUsed--;
    }
}
/* ===============================================================================
* Name: KISS_BLOCKPOOL_GetNumBlocks()
* Description: Get the total number of blocks the pool has.
* Parameters: [I] pBLOCK - Pointer to the block pool to query
* Return: int - Total capacity of the block pool in blocks.
* Caution/Notes: None
================================================================================== */
int KISS_BLOCKPOOL_GetNumBlocks(const KISS_BLOCKPOOL* pBLOCK) {
    KISS_ASSERT(pBLOCK != NULL, "BLOCKPOOL must be a valid pointer");
    return pBLOCK->NumBlocks;
}
/* ===============================================================================
* Name: KISS_BLOCKPOOL_GetBlockSize()
* Description: Get the size of each block the block pool can allocate
* Parameters: [I] pBLOCK - Pointer to the block pool to query
* Return: int - Returns the allocated block size.
* Caution/Notes:
================================================================================== */
int KISS_BLOCKPOOL_GetBlockSize(const KISS_BLOCKPOOL* pBLOCK) {
    KISS_ASSERT(pBLOCK != NULL, "BLOCKPOOL must be a valid pointer");
    return pBLOCK->BlockSize;
}
/* ===============================================================================
* Name: KISS_BLOCKPOOL_GetNumFreeBlocks()
* Description: Get the remaining free capacity within the block pool
* Parameters: [I] pBLOCK - Pointer to the block pool to query
* Return: int - Returns the number of unallocated blocks within the block pool
* Caution/Notes: None
================================================================================== */
int KISS_BLOCKPOOL_GetNumFreeBlocks(const KISS_BLOCKPOOL* pBLOCK) {
    KISS_ASSERT(pBLOCK != NULL, "BLOCKPOOL must be a valid pointer");
    return pBLOCK->NumBlocks - pBLOCK->BlocksUsed;
}
/* ===============================================================================
* Name: KISS_BLOCKPOOL_GetMaxUsed()
* Description: Get the maximum number of blocks used in the block pool
* Parameters: [I] pBLOCK - Pointer to the block pool to query
* Return: int - Returns the high watermark for the block pool
* Caution/Notes: None
================================================================================== */
int KISS_BLOCKPOOL_GetMaxUsed(const KISS_BLOCKPOOL* pBLOCK) {
    KISS_ASSERT(pBLOCK != NULL, "BLOCKPOOL must be a valid pointer");
    return pBLOCK->MaxUsed;
}
/* ===============================================================================
* Name: KISS_BLOCKPOOL_IsInPool()
* Description: Check whether the allocated memory is within the specified block pool
* Parameters:   [I] pBLOCK - Pointer to the block pool to query
*               [I] pMemBlock - Pointer to the memory block to validate
* Return: KISS_BOOL - Returns true if the allocation is within the Block Pool.
* Caution/Notes: None
================================================================================== */
KISS_BOOL KISS_BLOCKPOOL_IsInPool(const KISS_BLOCKPOOL* pBLOCK, const void* pMemBlock) {
    KISS_ASSERT(pBLOCK != NULL, "BLOCKPOOL must be a valid pointer");
    const size_t Size = (size_t)pBLOCK->BlockSize * pBLOCK->NumBlocks;
    const size_t Offset = (uint8_t*)pMemBlock - pBLOCK->pPool;
    return ((uint8_t*)pMemBlock >= pBLOCK->pPool) && (Offset < Size);
}
