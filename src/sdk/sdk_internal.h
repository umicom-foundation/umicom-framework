/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/sdk_internal.h
 *
 * PURPOSE:
 *   Provide bounded string helpers shared by the second-generation SDK model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SDK_INTERNAL_H
#define UMICOM_SDK_INTERNAL_H

#include <stdio.h>
#include <string.h>
#include "umicom/base/status.h"

static inline UmiStatus umi_sdk_copy(char *destination,
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

static inline int umi_sdk_text(const char *value)
{
    return value != NULL && value[0] != '\0';
}

#endif
