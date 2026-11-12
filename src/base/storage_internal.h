/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/base/storage_internal.h
 *
 * PURPOSE:
 *   Share the established flat-address overlap predicate among storage admission and archive consumers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/



#ifndef UMICOM_BASE_STORAGE_INTERNAL_H
#define UMICOM_BASE_STORAGE_INTERNAL_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* This predicate describes already-owned storage on supported flat-address
 * hosts. It does not prove a pointer is readable, writable or correctly aligned.
 * Comparing distances avoids forming an overflowing end address. Empty spans
 * do not overlap. Callers retain their own null-pointer admission policy. */
static inline bool UmiStorageOverlaps(const void *left, size_t left_size,
                                     const void *right, size_t right_size)
{
    if (left_size == 0U || right_size == 0U) return false;
    uintptr_t a = (uintptr_t)left, b = (uintptr_t)right;
    return a <= b ? b - a < left_size : a - b < right_size;
}
#endif
