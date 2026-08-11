/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/components/accessibility.c
 *
 * PURPOSE:
 *   Implement one toolkit-neutral reusable UI component concern.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
#include "umicom/ui/components/accessibility.h"

#include <string.h>

static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    if (destination == NULL || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    const size_t length = strlen(source);
    if (length >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_component_accessibility_set(UmiUiComponentAccessibility *value,
                                             const char *name,
                                             const char *description)
{
    if (value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiStatus status = copy_text(value->name,
                                 sizeof(value->name),
                                 name != NULL ? name : "");
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return copy_text(value->description,
                     sizeof(value->description),
                     description != NULL ? description : "");
}
