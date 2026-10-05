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

#include "umicom/ui/context_changes.h"

#include <stdio.h>
#include <string.h>

/* Bound external text before copying it. Walking to the first terminator also
 * supports ordinary short string literals without reading beyond their object. */
static int context_text_fits(const char *text)
{
    size_t index;
    if (text == NULL) return 0;
    for (index = 0U; index < UMI_APPLICATION_RUNTIME_TEXT_CAPACITY; ++index)
        if (text[index] == '\0') return 1;
    return 0;
}

/* The store is a public value type, so validate its shape before trusting a
 * caller-provided count. Unused array slots do not participate in its meaning. */
UmiStatus umi_application_context_binding_validate(
    const UmiApplicationContextBindingStore *store)
{
    size_t index, previous;
    if (store == NULL || store->structure_size != sizeof(*store) ||
        store->entry_count > UMI_APPLICATION_RUNTIME_MAX_CONTEXT_BINDINGS)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < store->entry_count; ++index) {
        const UmiApplicationContextBindingEntry *entry = &store->entries[index];
        if (!context_text_fits(entry->group_id) || entry->group_id[0] == '\0' ||
            !context_text_fits(entry->value)) return UMI_STATUS_INVALID_ARGUMENT;
        for (previous = 0U; previous < index; ++previous)
            if (strcmp(store->entries[previous].group_id, entry->group_id) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
    }
    return UMI_STATUS_OK;
}

/* Provide the find entry operation used by this module and its client applications. */
static int find_entry(const UmiApplicationContextBindingStore *store, const char *group_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    /* The shared lookup also protects the borrowed-value getter. */
    if (umi_application_context_binding_validate(store) != UMI_STATUS_OK ||
        !context_text_fits(group_id) || group_id[0] == '\0') return -1;
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
/* Validate and stage each context entry before publication so failed copies cannot alter the store.
 * The former implementation is retained for engineering review. */
#if 0
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
#endif
UmiStatus umi_application_context_binding_set(
    UmiApplicationContextBindingStore *store, const char *group_id, const char *value)
{
    UmiApplicationContextBindingEntry candidate = {0};
    UmiStatus status = umi_application_context_binding_validate(store);
    int found;
    size_t index;
    if (status != UMI_STATUS_OK) return status;
    if (!context_text_fits(group_id) || group_id[0] == '\0' ||
        !context_text_fits(value)) return UMI_STATUS_INVALID_ARGUMENT;
    found = find_entry(store, group_id);
    if (store->revision == UINT64_MAX || (found >= 0 &&
        store->entries[(size_t)found].revision == UINT64_MAX))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (found < 0 && store->entry_count == UMI_APPLICATION_RUNTIME_MAX_CONTEXT_BINDINGS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Copy both strings before replacing a slot: either input may point into
     * this same store. Truncation would silently change the linked identity. */
    memcpy(candidate.group_id, group_id, strlen(group_id) + 1U);
    memcpy(candidate.value, value, strlen(value) + 1U);
    candidate.revision = found < 0 ? 1U : store->entries[(size_t)found].revision + 1U;
    index = found < 0 ? store->entry_count : (size_t)found;
    store->entries[index] = candidate;
    if (found < 0) ++store->entry_count;
    ++store->revision;
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
/* Validate removals and guard revision exhaustion before changing the context cache.
 * The former implementation is retained for engineering review. */
#if 0
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
#endif
UmiStatus umi_application_context_binding_clear(
    UmiApplicationContextBindingStore *store, const char *group_id)
{
    UmiStatus status = umi_application_context_binding_validate(store);
    int found;
    size_t index;
    if (status != UMI_STATUS_OK) return status;
    if (!context_text_fits(group_id) || group_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    found = find_entry(store, group_id);
    if (found < 0) return UMI_STATUS_NOT_FOUND;
    if (store->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Shift only live entries and clear the retired slot, keeping subsequent
     * value copies independent of a context that is no longer present. */
    for (index = (size_t)found; index + 1U < store->entry_count; ++index)
        store->entries[index] = store->entries[index + 1U];
    --store->entry_count;
    memset(&store->entries[store->entry_count], 0, sizeof(store->entries[0]));
    ++store->revision;
    return UMI_STATUS_OK;
}

/* Publish cached application context-link values through the existing UI context authority. */
/* Publish the complete linked-context projection through the atomic UI context owner.
 * The former implementation is retained for engineering review. */
#if 0
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
#endif
UmiStatus umi_application_context_binding_apply_to_ui(
    const UmiApplicationContextBindingStore *store, UmiUiContextStore *ui_context)
{
    UmiUiContextChange changes[UMI_APPLICATION_RUNTIME_MAX_CONTEXT_BINDINGS] = {0};
    UmiStatus status = umi_application_context_binding_validate(store);
    size_t index;
    if (status != UMI_STATUS_OK) return status;
    if (ui_context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* One UI transaction prevents a full UI store from accepting only the
     * first few links. Keys owned by other services remain untouched. */
    for (index = 0U; index < store->entry_count; ++index) {
        changes[index].operation = UMI_UI_CONTEXT_CHANGE_SET;
        changes[index].value.kind = UMI_UI_CONTEXT_STRING;
        memcpy(changes[index].value.key, store->entries[index].group_id,
            strlen(store->entries[index].group_id) + 1U);
        memcpy(changes[index].value.string_value, store->entries[index].value,
            strlen(store->entries[index].value) + 1U);
    }
    return UmiUiContextApplyChanges(ui_context, umi_ui_context_revision(ui_context),
        changes, store->entry_count, NULL);
}
