/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/components/registry.c
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
#include "umicom/ui/components/registry.h"

#include <string.h>

void umi_ui_component_registry_init(UmiUiComponentRegistry *registry)
{
    if (registry != NULL) {
        (void)memset(registry, 0, sizeof(*registry));
    }
}

UmiStatus umi_ui_component_registry_find(const UmiUiComponentRegistry *registry,
                                         const char *id,
                                         UmiUiComponentSpec *out_spec)
{
    if (registry == NULL || id == NULL || out_spec == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (size_t index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].id, id) == 0) {
            *out_spec = registry->items[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_ui_component_registry_add(UmiUiComponentRegistry *registry,
                                        const UmiUiComponentSpec *spec)
{
    UmiUiComponentSpec existing;

    if (registry == NULL || spec == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_ui_component_spec_validate(spec) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_ui_component_registry_find(registry, spec->id, &existing) ==
        UMI_STATUS_OK) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (registry->count >= UMI_UI_COMPONENT_REGISTRY_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    registry->items[registry->count] = *spec;
    ++registry->count;
    return UMI_STATUS_OK;
}
