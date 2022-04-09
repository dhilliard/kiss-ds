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
*   Component: Fixed Size Arena allocator
*   File: KISS_ARENA.h
*   Description:  This file declares the functions for a fixed size arena allocator
*   Caution/Notes:  None
*=================================================================================*/
#include "KISS_Common.h"
#ifndef _KISS_ARENA_H_
#define _KISS_ARENA_H_

#ifdef __cplusplus
extern "C" {
#endif

/* KISS_ARENA implements a linear/arena allocator */
typedef struct KISS_ARENA {
    void* pArena;
    void* pArenaTop;
    KISS_UINT ArenaSize;
} KISS_ARENA;

/* Create a stack allocator using the provided buffer */
void KISS_ARENA_Create(KISS_ARENA* pA, void* pBuffer, KISS_UINT Size);
/* Reset the stack allocator to the unallocated state */
void KISS_ARENA_Delete(KISS_ARENA* pA);
/* Reset the stack pointer to the bottom of the stack. */
void KISS_ARENA_Clear(KISS_ARENA* pA);
/* Get the number of items pushed onto the stack */
int KISS_ARENA_BytesAllocated(const KISS_ARENA* pA);
/* Allocate a frame on the stack */
void* KISS_ARENA_Alloc(KISS_ARENA* pA, KISS_UINT Size);

void* KISS_ARENA_AllocEx(KISS_ARENA* pA, KISS_UINT Size, KISS_UINT Alignment);

#ifdef __cplusplus
}
#endif

#endif
