/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/components/command.c
 *
 * PURPOSE:
 *   Implement one toolkit-neutral reusable UI component concern.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
#include "umicom/ui/components/command.h"

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

UmiStatus umi_ui_component_command_bind(UmiUiComponentCommandBinding *binding,
                                        const char *component_id,
                                        const char *command_id)
{
    if (binding == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiStatus status = copy_text(binding->component_id,
                                 sizeof(binding->component_id),
                                 component_id);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return copy_text(binding->command_id,
                     sizeof(binding->command_id),
                     command_id);
}
