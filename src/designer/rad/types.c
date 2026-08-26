/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/types.c
 *
 * PURPOSE:
 *   Define stable toolkit-neutral RAD designer identifiers, limits and shared enums.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/types.h"
#include <string.h>

UmiStatus umi_rad_copy_text(char *destination, size_t capacity, const char *source) {
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

int umi_rad_id_valid(const char *identifier) {
    return identifier != NULL && identifier[0] != '\0' && strlen(identifier) < UMI_RAD_ID_CAPACITY;
}
