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
*   Component: Fixed Size Array
*   File: KISS_ARRAY.h
*   Description:  This file implements the logic for a fixed size array object.
*   Caution/Notes:  None
*=================================================================================*/
#ifndef _KISS_ARRAY_H_
#define _KISS_ARRAY_H_
#include "KISS_Common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kiss_array_t {
    void* pBuffer;
    KISS_UINT Capacity;
    KISS_UINT Count;
    KISS_UINT ItemSize;
} KISS_ARRAY;

/* Allocate the array with a fixed size memory buffer */
void KISS_ARRAY_Create(KISS_ARRAY* pARRAY, KISS_UINT ItemSize, KISS_UINT ItemCapacity, void * pMemBuffer);
/* Delete the array. */
void KISS_ARRAY_Delete(KISS_ARRAY* pARRAY);

void KISS_ARRAY_Clear(KISS_ARRAY* pARRAY);

void* KISS_ARRAY_PutFront(KISS_ARRAY* pARRAY, const void* pItem);

void* KISS_ARRAY_PutBack(KISS_ARRAY* pARRAY, const void* pItem);

void* KISS_ARRAY_InsertAt(KISS_ARRAY* pARRAY, KISS_INT Position, const void* pItem);

int KISS_ARRAY_Splice(KISS_ARRAY* pARRAY, KISS_INT StartIndex, KISS_UINT DeleteCount, void* pItems, KISS_UINT ItemCount);

KISS_BOOL KISS_ARRAY_Get(KISS_ARRAY* pARRAY, KISS_INT Position, void* pData);
KISS_BOOL KISS_ARRAY_GetPtr(KISS_ARRAY* pARRAY, KISS_INT Position, void ** ppElement);

/* Erase the element at the specified position and move other elements. NB: Preserves element order */
int KISS_ARRAY_Erase(KISS_ARRAY* pARRAY, KISS_INT Position, KISS_UINT Num);
/* Remove the element at the specified position. NB: Does not preserve element order */
int KISS_ARRAY_Remove(KISS_ARRAY* pARRAY, KISS_INT Position, KISS_UINT Num);

KISS_UINT KISS_ARRAY_GetItemSize(const KISS_ARRAY* pARRAY);
KISS_UINT KISS_ARRAY_GetItemCount(const KISS_ARRAY* pARRAY);
KISS_UINT KISS_ARRAY_GetItemCapacity(const KISS_ARRAY* pARRAY);


#ifdef __cplusplus
}
#endif
#endif //_KISS_ARRAY_H_
