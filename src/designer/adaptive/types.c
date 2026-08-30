/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/types.c
 *
 * PURPOSE:
 *   Implement bounded adaptive-designer helper operations shared by all authoring services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/designer/adaptive/types.h"
#include <string.h>

/* Copy bounded text and reject truncation so generated manifests stay deterministic. */
UmiStatus umi_designer_adaptive_copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) {
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

/* Compare canonical size classes using their stable ordered enum semantics. */
int umi_designer_adaptive_size_in_range(UmiDesignSizeClass value,
                                        UmiDesignSizeClass minimum,
                                        UmiDesignSizeClass maximum)
{
    return minimum <= maximum && value >= minimum && value <= maximum;
}
