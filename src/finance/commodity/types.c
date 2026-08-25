/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/types.c
 *
 * PURPOSE:
 *   Implement fixed-buffer helpers used throughout the commodity platform.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/commodity/types.h"

#include <string.h>

/* Copy a string with explicit capacity checking to keep domain records bounded. */
UmiStatus umi_commodity_copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || source == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length >= capacity) {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Validate identifiers without imposing application-specific naming schemes. */
bool umi_commodity_text_valid(const char *text)
{
    return text != NULL && text[0] != '\0';
}
