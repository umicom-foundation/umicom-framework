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
#include "umicom/base/text.h"
#include "../../base/storage_internal.h"

/* Public records can be copied or restored by clients. Admit every active row
 * before lookup, so an early match cannot conceal corrupt later storage. */
UmiStatus umi_ui_component_registry_validate(const UmiUiComponentRegistry *registry)
{
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (registry->count > UMI_UI_COMPONENT_REGISTRY_CAPACITY)
        return UMI_STATUS_INVALID_STATE;
    for (size_t index = 0U; index < registry->count; ++index) {
        if (umi_ui_component_spec_validate(&registry->items[index]) != UMI_STATUS_OK)
            return UMI_STATUS_INVALID_STATE;
        for (size_t prior = 0U; prior < index; ++prior)
            if (strcmp(registry->items[prior].id, registry->items[index].id) == 0)
                return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}


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
/* Complete bounded storage admission and output ownership replace unchecked traversal. All failure paths now preserve output, and the earlier implementation remains for review. The previous implementation is retained for review. */
#if 0
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
#endif
UmiStatus umi_ui_component_registry_find(const UmiUiComponentRegistry *registry,
                                         const char *id,
                                         UmiUiComponentSpec *out_spec)
{
    char selected_id[UMI_UI_COMPONENT_ID_CAPACITY];
    UmiStatus status;
    if (registry == NULL || id == NULL || out_spec == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* A const lookup cannot serve as a hidden registry mutation. Queries may
     * borrow output text; capture that text before publishing the copied value. */
    if (UmiStorageOverlaps(registry, sizeof(*registry), out_spec, sizeof(*out_spec)))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_component_registry_validate(registry);
    if (status != UMI_STATUS_OK) return status;
    status = umi_text_replace(selected_id, sizeof(selected_id), id);
    if (status != UMI_STATUS_OK) return status;
    for (size_t index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].id, selected_id) == 0) {
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
/* Validate the complete registry before duplicate search and insertion. Preserve the earlier add path for comparison with the bounded implementation. The previous implementation is retained for review. */
#if 0
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
#endif
UmiStatus umi_ui_component_registry_add(UmiUiComponentRegistry *registry,
                                        const UmiUiComponentSpec *spec)
{
    if (registry == NULL || spec == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_ui_component_registry_validate(registry);
    if (status != UMI_STATUS_OK) return status;
    if (umi_ui_component_spec_validate(spec) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep duplicate-before-full behavior for a valid catalogue. Capture the
     * input before mutation because it may borrow an inactive registry slot. */
    for (size_t index = 0U; index < registry->count; ++index)
        if (strcmp(registry->items[index].id, spec->id) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    if (registry->count == UMI_UI_COMPONENT_REGISTRY_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const UmiUiComponentSpec staged = *spec;
    registry->items[registry->count] = staged;
    registry->count += 1U;
    return UMI_STATUS_OK;
}
