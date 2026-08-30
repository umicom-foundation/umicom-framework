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

static int find_entry(const UmiApplicationContextBindingStore *store, const char *group_id)
{
    size_t index;
    if (store == NULL || group_id == NULL) return -1;
    for (index = 0U; index < store->entry_count; ++index) {
        if (strcmp(store->entries[index].group_id, group_id) == 0) return (int)index;
    }
    return -1;
}

void umi_application_context_binding_store_init(UmiApplicationContextBindingStore *store)
{
    if (store == NULL) return;
    memset(store, 0, sizeof(*store));
    store->structure_size = sizeof(*store);
}

UmiStatus umi_application_context_binding_set(
    UmiApplicationContextBindingStore *store,
    const char *group_id,
    const char *value)
{
    int found;
    size_t index;
    if (store == NULL || group_id == NULL || group_id[0] == '\0' || value == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    found = find_entry(store, group_id);
    if (found < 0) {
        if (store->entry_count >= UMI_APPLICATION_RUNTIME_MAX_CONTEXT_BINDINGS)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        index = store->entry_count++;
        if (snprintf(store->entries[index].group_id,
                     sizeof(store->entries[index].group_id), "%s", group_id) < 0)
            return UMI_STATUS_INTERNAL_ERROR;
    } else {
        index = (size_t)found;
    }
    if (snprintf(store->entries[index].value,
                 sizeof(store->entries[index].value), "%s", value) < 0)
        return UMI_STATUS_INTERNAL_ERROR;
    store->entries[index].revision += 1U;
    store->revision += 1U;
    return UMI_STATUS_OK;
}

const char *umi_application_context_binding_get(
    const UmiApplicationContextBindingStore *store,
    const char *group_id)
{
    int found = find_entry(store, group_id);
    return found >= 0 ? store->entries[(size_t)found].value : NULL;
}

UmiStatus umi_application_context_binding_clear(
    UmiApplicationContextBindingStore *store,
    const char *group_id)
{
    int found;
    size_t index;
    if (store == NULL || group_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    found = find_entry(store, group_id);
    if (found < 0) return UMI_STATUS_NOT_FOUND;
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
    if (store == NULL || ui_context == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* The application binding store is only an application-facing cache.
     * Publish every value through the existing UI context service used by commands and menus. */
    for (index = 0U; index < store->entry_count && status == UMI_STATUS_OK; ++index)
        status = umi_ui_context_set_string(ui_context,
            store->entries[index].group_id, store->entries[index].value);
    return status;
}
