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
*   File: KISS_ARRAY.c
*   Description:  This file implements the logic for a fixed size array object.
*   Caution/Notes:  None
*=================================================================================*/
#include "KISS_ARRAY.h"

/* Internal helper functions for moving elements in the array around */
static void __MakeSpaceInArray(uint8_t* pDst, const uint8_t* pSrc, KISS_UINT ElementSize, KISS_UINT Count);
static void __FillHoleInArray(uint8_t* pDst, const uint8_t* pSrc, KISS_UINT ElementSize, KISS_UINT Count);

/* ===============================================================================
* Name: KISS_ARRAY_Create()
* Description: Initialise the array with a fixed size / preallocated memory buffer
* Parameters: [O] pArray - Pointer to the Array to initialise
*               [I] ItemSize - The size of each element.
*               [I] ItemCapacity - The total number of elements the array can store
*               [I] pMemBuffer - Pointer to buffer to use for array storage
* Return: None
* Caution/Notes:    None
================================================================================== */
void KISS_ARRAY_Create(KISS_ARRAY* pARRAY, KISS_UINT ItemSize, KISS_UINT ItemCapacity, void* pMemBuffer) {
    KISS_ASSERT(pARRAY != NULL, "Array must be a valid pointer");
    KISS_ASSERT(ItemSize > 0, "Item Size is invalid");
    KISS_ASSERT(ItemCapacity > 0, "Capacity is invalid");
    KISS_ASSERT(pMemBuffer != NULL, "Memory Buffer must be valid");
    KISS_MEMSET(pARRAY, 0, sizeof(KISS_ARRAY));
    pARRAY->pBuffer = pMemBuffer;
    pARRAY->Capacity = ItemCapacity;
    pARRAY->ItemSize = ItemSize;
}

/* ===============================================================================
* Name: KISS_ARRAY_Delete()
* Description: Reset the array to the uninitialised state. 
* Parameters:   [O] pArray - Pointer to the Array
* Return: None
* Caution/Notes:    Frees memory if an allocator is assigned
================================================================================== */
void KISS_ARRAY_Delete(KISS_ARRAY* pARRAY) {
    KISS_ASSERT(pARRAY != NULL, "Array must be a valid pointer");
    KISS_MEMSET(pARRAY, 0, sizeof(KISS_ARRAY));
}

/* ===============================================================================
* Name: KISS_ARRAY_PutFront()
* Description: Insert an item into the designated array at the start of the array
* Parameters: [I/O] pArray - Pointer to the Array to modify
*               [I] pItem - Item data to copy into array. If NULL, the element will
*                   be zero initialised
* Return: void * - Pointer to the newly added item in the array or NULL if unsuccessful.
* Caution/Notes:    None
================================================================================== */
void* KISS_ARRAY_PutFront(KISS_ARRAY* pARRAY, const void* pItem)
{
    KISS_ASSERT(pARRAY != NULL, "Array must be a valid pointer");
    if (pARRAY->Count >= pARRAY->Capacity) {
        /* If a dynamically resizable array, resize to the new capacity */
        /* Otherwise immediately return because the array is full */
        return NULL;
    }
    uint8_t* pDstItem = (uint8_t*)pARRAY->pBuffer;
    /* Move elements in the array to make space at the start of the array */
    __MakeSpaceInArray(
        (uint8_t*)pARRAY->pBuffer + (1ull) * pARRAY->ItemSize,
        (uint8_t*)pDstItem,
        pARRAY->ItemSize,
        pARRAY->Count
    );
    pARRAY->Count++;
    if (pItem) {
        /* Copy in the item if the pointer is valid. */
        KISS_MEMCPY(pDstItem, pItem, pARRAY->ItemSize);
    }
    else {
        KISS_MEMSET(pDstItem, 0, pARRAY->ItemSize);
    }
    return pDstItem;
}

/* ===============================================================================
* Name: KISS_ARRAY_PutBack()
* Description: Insert an item into the designated array at end of the array
* Parameters: [I/O] pArray - Pointer to the Array to modify
*               [I] pItem - Item data to copy into array. If NULL, the element will
*                   be zero initialised
* Return: void * - Pointer to the newly added item in the array or NULL if unsuccessful.
* Caution/Notes: It is generally advisable to use this function in lieu of KISS_ARRAY_PutFront()
================================================================================== */
void* KISS_ARRAY_PutBack(KISS_ARRAY* pARRAY, const void* pItem) {
    KISS_ASSERT(pARRAY != NULL, "Array must be a valid pointer");
    if (pARRAY->Count >= pARRAY->Capacity) {
        /* If a dynamically resizable array, resize to the new capacity */
        /* Otherwise immediately return because the array is full */
        return NULL;
    }
    uint8_t* pDstItem = (uint8_t*)pARRAY->pBuffer + ((size_t)pARRAY->ItemSize * pARRAY->Count);
    pARRAY->Count++;
    if (pItem) {
        /* Copy in the item if the pointer is valid. */
        KISS_MEMCPY(pDstItem, pItem, pARRAY->ItemSize);
    } else {
        KISS_MEMSET(pDstItem, 0, pARRAY->ItemSize);
    }
    return pDstItem;
}

/* ===============================================================================
* Name: KISS_ARRAY_InsertAt()
* Description: Insert an item into the designated array at the specified location
* Parameters: [I/O] pArray - Pointer to the Array to modify
*               [I] Position - Index into array to insert.
*                    Negative values are relative to the end of the array.
*               [I] pItem - Item data to copy into array. If NULL, the element will
*                   be zero initialised
* Return: void * - Pointer to the newly added item in the array or NULL if unsuccessful.
* Caution/Notes:    Inserting an element at index 0 will always succeed for a valid array
*                   even when the array is empty.
================================================================================== */
void* KISS_ARRAY_InsertAt(KISS_ARRAY* pARRAY, KISS_INT Position, const void* pItem) {
    KISS_ASSERT(pARRAY != NULL, "Array must be a valid pointer");
    if (pARRAY->Count >= pARRAY->Capacity) {
        /* If a dynamically resizable array, resize to the new capacity */
        /* Otherwise immediately return because the array is full */
        return NULL;
    }
    
    /* Convert relative position into absolute position (if necessary) */
    if (Position < 0) {
        Position = pARRAY->Count + Position + 1;
    }
    /* Range check the position */
    if (Position >= 0 && (Position <= (int)pARRAY->Count)) {
        uint8_t* pDstItem = (uint8_t*)pARRAY->pBuffer + ((size_t)pARRAY->ItemSize * Position);
        /* Relocate existing items to make room for the new item */
        __MakeSpaceInArray(
            (uint8_t*)pARRAY->pBuffer + ((size_t)Position + 1) * pARRAY->ItemSize,
            pDstItem,
            pARRAY->ItemSize,
            pARRAY->Count - Position
        );
        pARRAY->Count++;
        /* Copy the item into the designated location */
        if (pItem) {
            /* Copy in the item if the pointer is valid. */
            KISS_MEMCPY(pDstItem, pItem, pARRAY->ItemSize);

        }
        else {
            KISS_MEMSET(pDstItem, 0, pARRAY->ItemSize);
        }

        return pDstItem;
    }

    return NULL;
    
}
/* ===============================================================================
* Name: KISS_ARRAY_Splice()
* Description: 
* Parameters:   [I/O] pArray - Pointer to the Array to modify
*               [I] Position - Index into array to insert.
*                    Negative values are relative to the end of the array.
*               [I] DeleteCount - Number of items to remove before insertion operation
*               [I] pItems - Array of items to copy into the destination array.
*               [I] ItemCount - Number of items stored in item array.
* Return: int - Returns new length of Array or negative number on error
* Caution/Notes:    
================================================================================== */
int KISS_ARRAY_Splice(KISS_ARRAY* pARRAY, KISS_INT StartIndex, KISS_UINT DeleteCount, const void* pItems, KISS_UINT ItemCount) {
    KISS_ASSERT(pARRAY != NULL, "Array must be a valid pointer");

    if (DeleteCount > pARRAY->Count)
    {
        /* We are trying to delete too many elements */
        return -1;
    }
    else if (((pARRAY->Count + ItemCount - DeleteCount) >= pARRAY->Capacity))
    {
        /* The new elements wont fit into the array */
        return -1;
    }
    if (StartIndex < 0) {
        /* Convert relative position into absolute position (if necessary) */
        StartIndex = pARRAY->Count + StartIndex + 1;
    }
    
    /* Range check the start index */
    if (StartIndex >= 0 && (StartIndex <= (int)pARRAY->Count)) {
        uint8_t* pDst = (uint8_t*)pARRAY->pBuffer + (size_t)pARRAY->ItemSize * StartIndex;
        if (DeleteCount > ItemCount) 
        {
            /* Need to remove extra elements to pack the array */
            const int delta = DeleteCount - ItemCount;
            uint8_t* pSrc = pDst + delta * (size_t)pARRAY->ItemSize;
            __FillHoleInArray(pDst, pSrc, pARRAY->ItemSize, pARRAY->Count - (StartIndex + delta));
            pARRAY->Count = pARRAY->Count - delta;
            
        }
        else if (ItemCount > DeleteCount) 
        {
            /* Need to make space for extra elements in the array */
            const int delta = ItemCount - DeleteCount;
            uint8_t* pSrc = pDst + delta * (size_t)pARRAY->ItemSize;
            __MakeSpaceInArray(pSrc, pDst, pARRAY->ItemSize, pARRAY->Count - (StartIndex + delta));
            pARRAY->Count = pARRAY->Count + delta;
        }
        /* Copy the new elements into the array at the correct location */
        for (int i = 0; i < ItemCount; ++i)
        {
            KISS_MEMCPY(pDst, (const uint8_t*)pItems + (size_t)pARRAY->ItemSize * i, pARRAY->ItemSize);
            pDst += pARRAY->ItemSize;
        }
        return pARRAY->Count;
    }

    return -1;
}

/* ===============================================================================
* Name: KISS_ARRAY_Get()
* Description: Get the element at the specified position.
* Parameters:   [I] pArray - Pointer to the Array
*               [I] Position - Index into array.
*                    Negative values are relative to the end of the array.
*               [O] pData - Pointer to memory to copy the designated element into.
* Return: KISS_BOOL - Returns 0 if successful.
* Caution/Notes:   The memory pointed to by pData must be at least pARRAY->ItemSize bytes.
================================================================================== */
KISS_BOOL KISS_ARRAY_Get(KISS_ARRAY* pARRAY, KISS_INT Position, void* pData) {
    KISS_ASSERT(pARRAY != NULL, "Array must be a valid pointer");
    if (pARRAY->Count > 0)
    {
        /* Convert relative position into absolute position (if necessary) */
        if (Position < 0) {
            Position = pARRAY->Count + Position;
        }
        /* Range check the position*/
        if (Position >= 0 && (Position < (int)pARRAY->Count)) {
            /* Only assign to ppElement if it is valid */
            if (pData) {
                uint8_t* pSrcData = (uint8_t*)pARRAY->pBuffer + ((size_t)Position) * pARRAY->ItemSize;
                KISS_MEMCPY(pData, pSrcData, pARRAY->ItemSize);
            }
            return 0;
        }
    }
    return 1;
}

/* ===============================================================================
* Name: KISS_ARRAY_GetPtr()
* Description: Get a pointer to the element in the array at the specified position
* Parameters:   [I] pArray - Pointer to the Array
*               [I] Position - Index into array.
*                    Negative values are relative to the end of the array.
*               [O] ppElement - Pointer to element pointer to store the computed address of.
* Return: KISS_BOOL - Returns 0 if successful.
* Caution/Notes:    None
================================================================================== */
KISS_BOOL KISS_ARRAY_GetPtr(KISS_ARRAY* pARRAY, KISS_INT Position, void** ppElement) {
    KISS_ASSERT(pARRAY != NULL, "Array must be a valid pointer");
    
    if(pARRAY->Count > 0)
    {
        /* Convert relative position into absolute position (if necessary) */
        if (Position < 0) {
            Position = pARRAY->Count + Position;
        }
        /* Range check the position*/
        if (Position >= 0 && (Position < (int)pARRAY->Count)) {
            /* Only assign to ppElement if it is valid */
            if (ppElement) {
                *ppElement = (uint8_t*)pARRAY->pBuffer + ((size_t)Position) * pARRAY->ItemSize;
            }
            return 0;
        }
    }
    return 1;
}

/* ===============================================================================
* Name: KISS_ARRAY_Erase()
* Description: Erase the element at the specified position and move other elements. 
* Parameters:   [I/O] pArray - Pointer to the Array
*               [I] Position - Index into array.
*                    Negative values are relative to the end of the array.
*               [I] Num - Number of elements to remove from the Array object
* Return: int - Returns new size of array 
* Caution/Notes: This function preserves element order 
================================================================================== */
int KISS_ARRAY_Erase(KISS_ARRAY* pARRAY, KISS_INT Position, KISS_UINT Num) {
    KISS_ASSERT(pARRAY != NULL, "Array must be a valid pointer");
    if (pARRAY->Count > 0)
    {
        /* Convert relative position into absolute position (if necessary) */
        if (Position < 0) {
            Position = pARRAY->Count + Position;
        }
        /* Range check the position*/
        if (Position >= 0 && (Position < (int)pARRAY->Count)) {
            {
                /* Clamp the (total) number of elements to relocate to be valid. */
                if (Position + Num > pARRAY->Count)
                {
                    Num = pARRAY->Count - Position;
                }
                else {
                    /* Calculate offsets for elements to move to fill the newly made hole */
                    uint8_t* pDestElements = (uint8_t*)pARRAY->pBuffer + ((size_t)Position) * pARRAY->ItemSize;
                    uint8_t* pSrcElements = (uint8_t*)pARRAY->pBuffer + ((size_t)(Position) + Num) * pARRAY->ItemSize;
                    __FillHoleInArray(pDestElements, pSrcElements, pARRAY->ItemSize, pARRAY->Count - (Position + Num));
                }
                
                uint8_t* pEnd = (uint8_t*)pARRAY->pBuffer + ((size_t)pARRAY->Count - Num) * pARRAY->ItemSize;
                /* Clear memory which has now been vacated */
                KISS_MEMSET(pEnd, 0, (size_t)pARRAY->ItemSize * Num);
                pARRAY->Count = pARRAY->Count - Num;
                return pARRAY->Count;
            }
        }
    }
    return 0;
}

/* ===============================================================================
* Name: KISS_ARRAY_Remove()
* Description: Remove the element(s) at/from the specified position.
* Parameters:   [I/O] pArray - Pointer to the Array
*               [I] Position - Index into array.
*                    Negative values are relative to the end of the array.
*               [I] Num - Number of elements to remove from the Array object
* Return: int - Returns new size of array
* Caution/Notes: This function does not preserve element order
================================================================================== */
int KISS_ARRAY_Remove(KISS_ARRAY* pARRAY, KISS_INT Position, KISS_UINT Num) {
    KISS_ASSERT(pARRAY != NULL, "Array must be a valid pointer");
    if (pARRAY->Count > 0)
    {
        /* Convert relative position into absolute position (if necessary) */
        if (Position < 0) {
            Position = pARRAY->Count + Position;
        }
        /* Range check the position*/
        if (Position >= 0 && (Position < (int)pARRAY->Count)) {
            {
                /* Clamp the (total) number of elements to relocate to be valid. */
                if (Position + Num > pARRAY->Count)
                {
                    Num = pARRAY->Count - Position;
                }
                /* Calculate offsets for elements to move to fill the newly made hole */
                uint8_t* pDestElements = (uint8_t*)pARRAY->pBuffer + ((size_t)Position) * pARRAY->ItemSize;
                uint8_t* pSrcElements = (uint8_t*)pARRAY->pBuffer + ((size_t)(pARRAY->Count - Num)) * pARRAY->ItemSize;
                if (pDestElements != pSrcElements) {
                    __FillHoleInArray(pDestElements, pSrcElements, pARRAY->ItemSize, Num);
                    
                }
                /* Clear memory which has now been vacated */
                KISS_MEMSET(pSrcElements, 0, (size_t)pARRAY->ItemSize * Num);
                /* Update the array count field */
                pARRAY->Count = pARRAY->Count - Num;
                
                return pARRAY->Count;
            }
        }
    }
    return 0;
}

/* ===============================================================================
* Name: KISS_ARRAY_Clear()
* Description: Reset the array to the default empty state
* Parameters:   [O] pArray - Pointer to the Array
* Return: None
* Caution/Notes:    None
================================================================================== */
void KISS_ARRAY_Clear(KISS_ARRAY* pARRAY)
{
    KISS_ASSERT(pARRAY != NULL, "Array must be a valid pointer");
    pARRAY->Count = 0;
}

/* ===============================================================================
* Name: KISS_ARRAY_GetItemSize()
* Description: Get the size of each item stored in the array.
* Parameters:   [I] pArray - Pointer to the Array
* Return: KISS_UINT - Returns size of each element in the array.
* Caution/Notes:    None
================================================================================== */
KISS_UINT KISS_ARRAY_GetItemSize(const KISS_ARRAY* pARRAY) {
    KISS_ASSERT(pARRAY != NULL, "Array must be a valid pointer");
    return pARRAY->ItemSize;
}
/* ===============================================================================
* Name: KISS_ARRAY_GetItemCount()
* Description: Gets the number of elements currently in the array.
* Parameters:   [I] pArray - Pointer to the Array
* Return: KISS_UINT - Returns number of elements currently in the array
* Caution/Notes:    None
================================================================================== */
KISS_UINT KISS_ARRAY_GetItemCount(const KISS_ARRAY* pARRAY) {
    KISS_ASSERT(pARRAY != NULL, "Array must be a valid pointer");
    return pARRAY->Count;
}
/* ===============================================================================
* Name: KISS_ARRAY_GetItemCapacity()
* Description: Get the total number of elements the array can store.
* Parameters:   [I] pArray - Pointer to the Array
* Return: KISS_UINT - Returns the number of elements the array can store.
* Caution/Notes:    None
================================================================================== */
KISS_UINT KISS_ARRAY_GetItemCapacity(const KISS_ARRAY* pARRAY) {
    KISS_ASSERT(pARRAY != NULL, "Array must be a valid pointer");
    return pARRAY->Capacity;
}


/* Relocate elements in an array where pDst and pSrc may overlap */
static void __MakeSpaceInArray(uint8_t* pDst, const uint8_t* pSrc, KISS_UINT ElementSize, KISS_UINT Count) {
    KISS_ASSERT(pDst >= (pSrc + ElementSize), "Source and Destination Memory must not overlap");
    for (int idx = Count; idx > 0; --idx) {
        const KISS_UINT offset = ElementSize * (idx - 1);
        KISS_MEMCPY(pDst + offset, pSrc + offset, ElementSize);
    }
}
static void __FillHoleInArray(uint8_t* pDst, const uint8_t* pSrc, KISS_UINT ElementSize, KISS_UINT Count) {
    KISS_ASSERT(pSrc >= (pDst + ElementSize), "Source and Destination Memory must not overlap");
    for (int idx = 0; idx < (int)Count; ++idx) {
        const KISS_UINT offset = ElementSize * idx;
        KISS_MEMCPY((void*)(pDst + offset), (void*)(pSrc + offset), ElementSize);
    }
}
