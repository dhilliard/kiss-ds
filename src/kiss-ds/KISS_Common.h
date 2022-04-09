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
*   Component: Common Type Definitions
*   File: KISS_Common.h
*   Description:	This file contains all common definitions shared between all
*					data structures
*   Caution/Notes:  None
*=================================================================================*/
#ifndef _KISS_COMMON_H_
#define _KISS_COMMON_H_

#include <stdint.h>

/* Macros for standard functions */
#ifndef KISS_MEMCPY
#include <memory.h>
#define KISS_MEMCPY(dst, src, size) memcpy((dst), (src), size)
#endif
#ifndef KISS_HEAP_ALLOC
#include <stdlib.h>
#define KISS_HEAP_ALLOC(size) malloc(size)
#define KISS_HEAP_FREE(pMemBlock) free(pMemBlock)
#endif
#ifndef KISS_MEMCMP
#include <memory.h>
#define KISS_MEMCMP(dst, src, size) memcmp((dst), (src), size)
#endif
#ifndef KISS_MEMSET
#include <memory.h>
#define KISS_MEMSET(dst, v, size) memset(dst, v, size)
#endif
#ifndef KISS_ASSERT
#include <assert.h>
#define KISS_ASSERT(cond, msg) assert((cond) && (msg));
#endif

/* Helper macros for simple math */
#define KISS_MAX(a,b) ((a) >= (b) ? (a) : (b))
#define KISS_MIN(a,b) ((a) <= (b) ? (a) : (b))
#define KISS_CLAMP(min, b, max) (KISS_MIN(KISS_MAX(min,b),max))

/* Helper macros for pointer alignment */
#define KISS_IS_POW2(x) (((x) != 0) && ((x) & ((x)-1)) == 0)
#define KISS_ALIGN_DOWN(n, a) ((n) & ~((a) - 1))
#define KISS_ALIGN_UP(n, a) KISS_ALIGN_DOWN((n) + (a) - 1, (a))
#define KISS_ALIGN_DOWN_PTR(p, a) ((void *)KISS_ALIGN_DOWN((uintptr_t)(p), (a)))
#define KISS_ALIGN_UP_PTR(p, a) ((void *)KISS_ALIGN_UP((uintptr_t)(p), (a)))

/* Typedefs for convenience */
typedef uint32_t KISS_UINT;
typedef int32_t KISS_INT;
typedef uint16_t KISS_UINT16;
typedef char KISS_BOOL;

#endif // !_KISS_COMMON_H_

