/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/sdk_v2_internal.h
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
#ifndef UMICOM_SDK_V2_INTERNAL_H
#define UMICOM_SDK_V2_INTERNAL_H

#include <stdio.h>
#include <string.h>
#include "umicom/base/status.h"

static inline UmiStatus umi_sdk_v2_copy(char *destination,
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

static inline int umi_sdk_v2_text(const char *value)
{
    return value != NULL && value[0] != '\0';
}

#endif
