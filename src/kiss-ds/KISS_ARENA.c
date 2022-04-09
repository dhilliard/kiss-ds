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
*   File: KISS_ARENA.c
*   Description:  This file implements the logic for a fixed size arena allocator
*   Caution/Notes:  None
*=================================================================================*/
#include "KISS_ARENA.h"

/* ===============================================================================
* Name: KISS_ARENA_Create()
* Description: Create an arena allocator using the provided buffer
* Parameters:   [O] pA - Pointer to arena object to initialise
*               [I] pBuffer - Pointer to memory to use with Arena Allocator
*               [I] Size - Total size (in bytes) of the memory buffer to use.
* Return: None
* Caution/Notes: None
================================================================================== */
void KISS_ARENA_Create(KISS_ARENA* pA, void* pBuffer, KISS_UINT Size) {
    KISS_ASSERT(pA != NULL, "Arena must be a valid pointer");
    pA->ArenaSize = Size;
    pA->pArena = (uint8_t*)pBuffer + Size - 1;
    pA->pArenaTop = pA->pArena;

}

/* ===============================================================================
* Name: KISS_ARENA_Delete()
* Description: Reset the arena allocator to the unallocated state
* Parameters: [O] pA - Pointer to arena to modify
* Return: None
* Caution/Notes: None
================================================================================== */
void KISS_ARENA_Delete(KISS_ARENA* pA) {
    KISS_ASSERT(pA != NULL, "Arena must be a valid pointer");
    pA->ArenaSize = 0;
    pA->pArenaTop = NULL;
    pA->pArena = NULL;
}

/* ===============================================================================
* Name: KISS_ARENA_Clear()
* Description: Reset the arena to the default empty state.
* Parameters: [O] pA - Pointer to arena object to modify.
* Return: None
* Caution/Notes: None
================================================================================== */
void KISS_ARENA_Clear(KISS_ARENA* pA) {
    KISS_ASSERT(pA != NULL, "Arena must be a valid pointer");
    pA->pArenaTop = pA->pArena;
}

/* ===============================================================================
* Name: KISS_ARENA_BytesAllocated()
* Description: Get the number of bytes allocated in the arena
* Parameters: [I] pA - Pointer to arena object
* Return: int - Returns number of bytes allocated in array.
* Caution/Notes: None
================================================================================== */
int KISS_ARENA_BytesAllocated(const KISS_ARENA* pA) {
    KISS_ASSERT(pA != NULL, "Arena must be a valid pointer");
    return (KISS_UINT)((uint8_t*)pA->pArena - (uint8_t*)pA->pArenaTop);
}

/* ===============================================================================
* Name: KISS_ARENA_Alloc()
* Description: Allocate memory in the Arena without any specific alignment requirement
* Parameters:   [I/O] pA - Pointer to array object to modify
*               [I] Size - Amount of memory to allocate.
* Return: void * - Returns newly allocate memory or NULL if not successful.
* Caution/Notes: None
================================================================================== */
void* KISS_ARENA_Alloc(KISS_ARENA* pA, KISS_UINT Size) {
    KISS_ASSERT(pA != NULL, "Arena must be a valid pointer");
    void* pResult = (uint8_t*)pA->pArenaTop - Size;
    if ((uint8_t*)pResult >= ((uint8_t*)pA->pArena - pA->ArenaSize)) {
        pA->pArenaTop = pResult;
        return pResult;
    }

    return NULL;
}
/* ===============================================================================
* Name: KISS_ARENA_AllocEx()
* Description: Allocate an item in the Arena with the specified alignment
* Parameters:   [I/O] pA - Pointer to array object to modify
*               [I] Size - Amount of memory to allocate.
*               [I] Alignment - Alignment of memory to allocate. Must be a power of 2.
* Return: void * - Returns newly allocated memory or NULL if not successful.
* Caution/Notes: None
================================================================================== */
void* KISS_ARENA_AllocEx(KISS_ARENA* pA, KISS_UINT Size, KISS_UINT Alignment) {
    KISS_ASSERT(pA != NULL, "Arena must be a valid pointer");
    void* pResult = KISS_ALIGN_DOWN_PTR((uint8_t*)pA->pArenaTop - Size,Alignment);
    if ((uint8_t*)pResult >= ((uint8_t*)pA->pArena - pA->ArenaSize)) {
        pA->pArenaTop = pResult;
        return pResult;
    }
    return NULL;
}
