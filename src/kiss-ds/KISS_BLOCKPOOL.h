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
#ifndef _KISS_BLOCKPOOL_H_
#define _KISS_BLOCKPOOL_H_

#include "KISS_Common.h"
#ifdef __cplusplus
extern "C" {
#endif


/* Memory Pool. Each block must be at least 8 bytes in size */
typedef struct {
    uint8_t* pPool;
#if 0
    KISS_UINT oFreeHead;
    KISS_UINT oFreeTail;
#else
    void* pHead;
    void* pTail;
#endif
    KISS_UINT MaxUsed;
    KISS_UINT NumBlocks;
    KISS_UINT BlocksUsed;
    KISS_UINT BlockSize;

} KISS_BLOCKPOOL;
/* Create Memory Pool which uses a preallocated block of memory */
void KISS_BLOCKPOOL_Create(KISS_BLOCKPOOL* pBLOCK, void* pPool, KISS_UINT NumBlocks, KISS_UINT BlockSize);

void KISS_BLOCKPOOL_Delete(KISS_BLOCKPOOL* pBLOCK);
/* Allocate a single fixed size block from the memory pool */
void* KISS_BLOCKPOOL_Alloc(KISS_BLOCKPOOL* pBLOCK);
void KISS_BLOCKPOOL_FreeEx(KISS_BLOCKPOOL* pBLOCK, void* pMemBlock);
int KISS_BLOCKPOOL_GetNumBlocks(const KISS_BLOCKPOOL* pBLOCK);
int KISS_BLOCKPOOL_GetBlockSize(const KISS_BLOCKPOOL* pBLOCK);
int KISS_BLOCKPOOL_GetNumFreeBlocks(const KISS_BLOCKPOOL* pBLOCK);
int KISS_BLOCKPOOL_GetMaxUsed(const KISS_BLOCKPOOL* pBLOCK);

KISS_BOOL KISS_BLOCKPOOL_IsInPool(const KISS_BLOCKPOOL* pBLOCK, const void* pMemBlock);

#ifdef __cplusplus
}
#endif

#endif //_KISS_BLOCKPOOL_H_