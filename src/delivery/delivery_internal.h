/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/delivery_internal.h
 *
 * PURPOSE:
 *   Provide small private helpers shared by delivery implementation files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This private header avoids repeating safe fixed-buffer string-copy code in every delivery module.
 */

#ifndef SRC_DELIVERY_DELIVERY_INTERNAL_H
#define SRC_DELIVERY_DELIVERY_INTERNAL_H

#include <stdio.h>
#include <string.h>

#include "umicom/base/status.h"

static inline UmiStatus umi_delivery_copy_text(char *destination,
                                                size_t capacity,
                                                const char *source)
{
    int written;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(destination, capacity, "%s", source);
    if (written < 0 || (size_t)written >= capacity) {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

static inline int umi_delivery_text_equal(const char *left, const char *right)
{
    return left != NULL && right != NULL && strcmp(left, right) == 0;
}

#endif
