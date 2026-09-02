/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/context_binding.c
 *
 * PURPOSE:
 *   Implement bounded context-link values with deterministic revisions and no heap allocation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/context_binding.h"

#include <stdio.h>
#include <string.h>

/* Provide the find entry operation used by this module and its client applications. */
static int find_entry(const UmiApplicationContextBindingStore *store, const char *group_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || group_id == NULL) return -1;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < store->entry_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(store->entries[index].group_id, group_id) == 0) return (int)index;
    }
    return -1;
}

/*
 * Initialise application context binding store from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_context_binding_store_init(UmiApplicationContextBindingStore *store)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) return;
    memset(store, 0, sizeof(*store));
    store->structure_size = sizeof(*store);
}

/*
 * Copy application context binding into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_application_context_binding_set(
    UmiApplicationContextBindingStore *store,
    const char *group_id,
    const char *value)
{
    int found;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || group_id == NULL || group_id[0] == '\0' || value == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    found = find_entry(store, group_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (found < 0) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (store->entry_count >= UMI_APPLICATION_RUNTIME_MAX_CONTEXT_BINDINGS)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        index = store->entry_count++;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (snprintf(store->entries[index].group_id,
                     sizeof(store->entries[index].group_id), "%s", group_id) < 0)
            return UMI_STATUS_INTERNAL_ERROR;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        index = (size_t)found;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (snprintf(store->entries[index].value,
                 sizeof(store->entries[index].value), "%s", value) < 0)
        return UMI_STATUS_INTERNAL_ERROR;
    store->entries[index].revision += 1U;
    store->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the application context binding get operation used by this module and its client
 * applications.
 */
const char *umi_application_context_binding_get(
    const UmiApplicationContextBindingStore *store,
    const char *group_id)
{
    int found = find_entry(store, group_id);
    return found >= 0 ? store->entries[(size_t)found].value : NULL;
}

/*
 * Release or reset state held by application context binding so the same storage can be
 * reused safely.
 */
UmiStatus umi_application_context_binding_clear(
    UmiApplicationContextBindingStore *store,
    const char *group_id)
{
    int found;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || group_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    found = find_entry(store, group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (found < 0) return UMI_STATUS_NOT_FOUND;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = (size_t)found; index + 1U < store->entry_count; ++index)
        store->entries[index] = store->entries[index + 1U];
    store->entry_count -= 1U;
    store->revision += 1U;
    return UMI_STATUS_OK;
}

/* Publish cached application context-link values through the existing UI context authority. */
UmiStatus umi_application_context_binding_apply_to_ui(
    const UmiApplicationContextBindingStore *store,
    UmiUiContextStore *ui_context)
{
    size_t index;
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || ui_context == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* The application binding store is only an application-facing cache.
     * Publish every value through the existing UI context service used by commands and menus. */
    for (index = 0U; index < store->entry_count && status == UMI_STATUS_OK; ++index)
        status = umi_ui_context_set_string(ui_context,
            store->entries[index].group_id, store->entries[index].value);
    return status;
}
