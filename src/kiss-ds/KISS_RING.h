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
*   File: KISS_RING.h
*   Description:  This file implements the logic for a fixed size ring buffer.
*   Caution/Notes:  None
*=================================================================================*/
#ifndef _KISS_RING_H_
#define _KISS_RING_H_

#include "KISS_Common.h"
#ifdef __cplusplus
extern "C" {
#endif
/*
KISS_RING implements a ring buffer and can be used as a mailbox for fixed size messages
*/
typedef struct {
    uint8_t* pBuffer;
    KISS_UINT MaxCount;
    KISS_UINT NumUsed;
    KISS_UINT oHead;
    KISS_UINT oTail;
    KISS_UINT16 ItemSize;
    KISS_UINT16 UseCount;

} KISS_RING;

/* Create a ring buffer using the buffer as the backing storage */
void KISS_RING_Create(KISS_RING* pRB, KISS_UINT16 sizeofMsg, KISS_UINT maxnofmsg, void* pBuffer);
/* Clear the ring buffer to the uninitialised state */
void KISS_RING_Delete(KISS_RING* pRB);
/* Remove any pending messages in the ring buffer */
void KISS_RING_Clear(KISS_RING* pRB);
/* Get the number of items currently in the ring buffer */
int KISS_RING_GetItemCnt(const KISS_RING* pRB);

/* These functions all return 0 on success */
KISS_BOOL KISS_RING_Put(KISS_RING* pRB, const void* pElement);
KISS_BOOL KISS_RING_Put1(KISS_RING* pRB, const char* pElement);

KISS_BOOL KISS_RING_PutFront(KISS_RING* pRB, const void* pElement);
KISS_BOOL KISS_RING_PutFront1(KISS_RING* pRB, const char* pElement);

KISS_BOOL KISS_RING_Get(KISS_RING* pRB, void* pData);
KISS_BOOL KISS_RING_Get1(KISS_RING* pRB, char* pData);

KISS_BOOL KISS_RING_Peek(const KISS_RING* pRB, void* pDest);
KISS_BOOL KISS_RING_GetPtr(KISS_RING* pRB, void** ppDest);
/* Must be used with KISS_RING_GetPtr() to "unuse" the Ring Buffer */
void KISS_RING_Purge(KISS_RING* pRB);

#ifdef __cplusplus
}
#endif

#endif
