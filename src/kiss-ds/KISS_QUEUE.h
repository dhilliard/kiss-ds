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
*   File: KISS_QUEUE.h
*   Description:  This file implements the logic for a single ended queue object.
*   Caution/Notes:  None
*=================================================================================*/
#ifndef _KISS_QUEUE_H_
#define _KISS_QUEUE_H_

#include "KISS_Common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*  KISS_QUEUE allows the user to push/pop variable size elements to the end of the data structure */
typedef struct KISS_QUEUE {

    uint8_t* pBuffer; /* Pointer to memory region to use for storage */
    KISS_UINT TotalSize; /* Size (in bytes) of the storage region. */
    KISS_UINT NumElements;
    KISS_UINT oHead;
    KISS_UINT oTail;
    KISS_UINT16 UseCount;

} KISS_QUEUE;

typedef struct KISS_QUEUE_SRC_LIST {
    const void* pSrc;
    KISS_UINT Size;
} KISS_QUEUE_SRCLIST;

/* Size = Size in bytes of the data buffer */
void KISS_QUEUE_Create(KISS_QUEUE* pQ, void* pData, KISS_UINT Size);
void KISS_QUEUE_Delete(KISS_QUEUE* pQ);
void KISS_QUEUE_Clear(KISS_QUEUE* pQ);

int KISS_QUEUE_Put(KISS_QUEUE* pQ, const void* pSrc, KISS_UINT Size);
int KISS_QUEUE_PutEx(KISS_QUEUE* pQ, const KISS_QUEUE_SRCLIST* pSrc, KISS_UINT NumSrc);

int KISS_QUEUE_PeekPtr(const KISS_QUEUE* pQ, void** ppData);
int KISS_QUEUE_GetPtr(KISS_QUEUE* pQ, void** ppData);
void KISS_QUEUE_Purge(KISS_QUEUE* pQ);

KISS_BOOL KISS_QUEUE_IsInUse(const KISS_QUEUE* pQ);

int KISS_QUEUE_GetItemCnt(const KISS_QUEUE* pQ);
int KISS_QUEUE_GetItemSize(const KISS_QUEUE* pQ);

#ifdef __cplusplus
}
#endif

#endif //_KISS_QUEUE_H_