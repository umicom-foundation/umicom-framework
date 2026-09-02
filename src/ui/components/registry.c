/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/components/registry.c
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
#include "umicom/ui/components/registry.h"

#include <string.h>

/*
 * Initialise ui component registry from caller-provided values so later operations receive
 * a known state.
 */
void umi_ui_component_registry_init(UmiUiComponentRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry != NULL) {
        (void)memset(registry, 0, sizeof(*registry));
    }
}

/*
 * Find ui component registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_component_registry_find(const UmiUiComponentRegistry *registry,
                                         const char *id,
                                         UmiUiComponentSpec *out_spec)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL || out_spec == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (size_t index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].id, id) == 0) {
            *out_spec = registry->items[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Add ui component registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ui_component_registry_add(UmiUiComponentRegistry *registry,
                                        const UmiUiComponentSpec *spec)
{
    UmiUiComponentSpec existing;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || spec == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ui_component_spec_validate(spec) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ui_component_registry_find(registry, spec->id, &existing) ==
        UMI_STATUS_OK) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_UI_COMPONENT_REGISTRY_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    registry->items[registry->count] = *spec;
    ++registry->count;
    return UMI_STATUS_OK;
}
