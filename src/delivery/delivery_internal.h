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

/*
 * This private header avoids repeating safe fixed-buffer string-copy code in every delivery module.
 */

#ifndef SRC_DELIVERY_DELIVERY_INTERNAL_H
#define SRC_DELIVERY_DELIVERY_INTERNAL_H

#include <stdio.h>
#include <string.h>

#include "umicom/base/status.h"

/**
 * Provide the delivery copy text operation used by this module and its client
 * applications.
 */
static inline UmiStatus umi_delivery_copy_text(char *destination,
                                                size_t capacity,
                                                const char *source)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(destination, capacity, "%s", source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= capacity) {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

/**
 * Provide the delivery text equal operation used by this module and its client
 * applications.
 */
static inline int umi_delivery_text_equal(const char *left, const char *right)
{
    return left != NULL && right != NULL && strcmp(left, right) == 0;
}

#endif
