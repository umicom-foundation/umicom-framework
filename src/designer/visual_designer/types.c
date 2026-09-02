/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/types.c
 *
 * PURPOSE:
 *   Define stable toolkit-neutral visual designer identifiers, limits and shared enums.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/types.h"
#include <string.h>

/* Provide the visual designer copy text operation used by this module and its client applications. */
UmiStatus umi_rad_copy_text(char *destination, size_t capacity, const char *source) {
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Check that visual designer id satisfies its contract before another service relies on it. */
int umi_rad_id_valid(const char *identifier) {
    return identifier != NULL && identifier[0] != '\0' && strlen(identifier) < UMI_RAD_ID_CAPACITY;
}
