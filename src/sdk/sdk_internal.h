/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/sdk_internal.h
 *
 * PURPOSE:
 *   Provide bounded string helpers shared by the second-generation SDK model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SDK_INTERNAL_H
#define UMICOM_SDK_INTERNAL_H

#include <stdio.h>
#include <string.h>
#include "umicom/base/status.h"

/**
 * Copy sdk into module-owned storage so callers keep ownership of their input values.
 */
static inline UmiStatus umi_sdk_copy(char *destination,
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
 * Provide the sdk text operation used by this module and its client applications.
 */
static inline int umi_sdk_text(const char *value)
{
    return value != NULL && value[0] != '\0';
}

#endif
