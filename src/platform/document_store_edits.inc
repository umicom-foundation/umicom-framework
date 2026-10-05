/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/document_store_edits.inc
 * PURPOSE: Publish checked text replacements only after the complete set is prepared.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/document_store_edits.h"

/* Included by document_store.c: the original store remains the only owner.
 * Comparing fields explicitly avoids treating structure padding as identity. */
static int StoreEditSnapshotMatches(const UmiDocumentEntry *entry, const UmiDocumentSnapshot *expected)
{
    return entry->document_id == expected->document_id && entry->revision == expected->revision &&
           entry->saved_revision == expected->saved_revision && entry->length == expected->length &&
           entry->external_change == expected->external_change &&
           (entry->revision != entry->saved_revision) == !!expected->dirty &&
           (entry->path[0] != '\0') == !!expected->has_path && strcmp(entry->path, expected->path) == 0 &&
           strcmp(entry->display_name, expected->display_name) == 0;
}

/* Draft owners can explicitly record an edit that returns to stored bytes, preserving dirty and Undo semantics.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiDocumentStoreReplaceTextChanges(UmiDocumentStore *store,
    const UmiDocumentStoreTextChange *changes, size_t count)
{
    if (store == NULL || changes == NULL || count == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_DOCUMENT_STORE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t total = 0U;
    for (size_t i = 0U; i < count; ++i) {
        const UmiDocumentStoreTextChange *change = &changes[i];
        if (change->expected == NULL || change->expected->document_id == 0U || change->text == NULL)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (memchr(change->expected->path, '\0', sizeof(change->expected->path)) == NULL ||
            memchr(change->expected->display_name, '\0', sizeof(change->expected->display_name)) == NULL)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (change->length > UMI_DOCUMENT_STORE_EDIT_MAXIMUM_BYTES ||
            change->length > UMI_DOCUMENT_STORE_EDIT_BUDGET_BYTES - total)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        total += change->length;
        if (memchr(change->text, '\0', change->length) != NULL) return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t prior = 0U; prior < i; ++prior)
            if (changes[prior].expected->document_id == change->expected->document_id)
                return UMI_STATUS_ALREADY_EXISTS;
    }
    char **prepared = calloc(count, sizeof(*prepared));
    size_t *indices = calloc(count, sizeof(*indices));
    if (prepared == NULL || indices == NULL) { free(prepared); free(indices); return UMI_STATUS_OUT_OF_MEMORY; }
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; i < count; ++i) {
        prepared[i] = malloc(changes[i].length + 1U);
        if (prepared[i] == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; break; }
        memcpy(prepared[i], changes[i].text, changes[i].length);
        prepared[i][changes[i].length] = '\0';
    }
    if (status == UMI_STATUS_OK) {
        (void)umi_mutex_lock(store->mutex);
        for (size_t i = 0U; i < count; ++i) {
            indices[i] = umi_document_store_find_index(store, changes[i].expected->document_id);
            if (indices[i] == SIZE_MAX) { status = UMI_STATUS_NOT_FOUND; break; }
            UmiDocumentEntry *entry = &store->entries[indices[i]];
            if (!StoreEditSnapshotMatches(entry, changes[i].expected)) { status = UMI_STATUS_INVALID_STATE; break; }
            int changed = entry->length != changes[i].length ||
                memcmp(entry->text, prepared[i], entry->length) != 0;
            if (changed && entry->revision == UINT64_MAX) { status = UMI_STATUS_CAPACITY_EXCEEDED; break; }
        }
        /* There are no allocations, callbacks or fallible operations from this
         * point until the complete set is visible. Old buffers are released
         * after unlocking, keeping the critical section short. */
        if (status == UMI_STATUS_OK) for (size_t i = 0U; i < count; ++i) {
            UmiDocumentEntry *entry = &store->entries[indices[i]];
            if (entry->length == changes[i].length && memcmp(entry->text, prepared[i], entry->length) == 0)
                continue;
            char *previous = entry->text;
            entry->text = prepared[i]; prepared[i] = previous;
            entry->length = changes[i].length; entry->capacity = changes[i].length + 1U;
            ++entry->revision; entry->external_change = 0;
        }
        (void)umi_mutex_unlock(store->mutex);
    }
    for (size_t i = 0U; i < count; ++i) free(prepared[i]);
    free(prepared); free(indices); return status;
}
#endif
UmiStatus UmiDocumentStoreReplaceTextChanges(UmiDocumentStore *store,
                                             const UmiDocumentStoreTextChange *changes, size_t count)
{
    if (store == NULL || changes == NULL || count == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_DOCUMENT_STORE_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t total = 0U;
    for (size_t i = 0U; i < count; ++i)
    {
        const UmiDocumentStoreTextChange *change = &changes[i];
        if ((change->force_revision != 0 && change->force_revision != 1) || change->expected == NULL ||
            change->expected->document_id == 0U || change->text == NULL)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (memchr(change->expected->path, '\0', sizeof(change->expected->path)) == NULL ||
            memchr(change->expected->display_name, '\0', sizeof(change->expected->display_name)) == NULL)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (change->length > UMI_DOCUMENT_STORE_EDIT_MAXIMUM_BYTES ||
            change->length > UMI_DOCUMENT_STORE_EDIT_BUDGET_BYTES - total)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        total += change->length;
        if (memchr(change->text, '\0', change->length) != NULL)
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t prior = 0U; prior < i; ++prior)
            if (changes[prior].expected->document_id == change->expected->document_id)
                return UMI_STATUS_ALREADY_EXISTS;
    }
    char **prepared = calloc(count, sizeof(*prepared));
    size_t *indices = calloc(count, sizeof(*indices));
    if (prepared == NULL || indices == NULL)
    {
        free(prepared);
        free(indices);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; i < count; ++i)
    {
        prepared[i] = malloc(changes[i].length + 1U);
        if (prepared[i] == NULL)
        {
            status = UMI_STATUS_OUT_OF_MEMORY;
            break;
        }
        memcpy(prepared[i], changes[i].text, changes[i].length);
        prepared[i][changes[i].length] = '\0';
    }
    if (status == UMI_STATUS_OK)
    {
        (void)umi_mutex_lock(store->mutex);
        for (size_t i = 0U; i < count; ++i)
        {
            indices[i] = umi_document_store_find_index(store, changes[i].expected->document_id);
            if (indices[i] == SIZE_MAX)
            {
                status = UMI_STATUS_NOT_FOUND;
                break;
            }
            UmiDocumentEntry *entry = &store->entries[indices[i]];
            if (!StoreEditSnapshotMatches(entry, changes[i].expected))
            {
                status = UMI_STATUS_INVALID_STATE;
                break;
            }
            int changed = changes[i].force_revision || entry->length != changes[i].length ||
                          memcmp(entry->text, prepared[i], entry->length) != 0;
            if (changed && entry->revision == UINT64_MAX)
            {
                status = UMI_STATUS_CAPACITY_EXCEEDED;
                break;
            }
        }
        /* There are no allocations, callbacks or fallible operations from this
         * point until the complete set is visible. Old buffers are released
         * after unlocking, keeping the critical section short. */
        if (status == UMI_STATUS_OK)
            for (size_t i = 0U; i < count; ++i)
            {
                UmiDocumentEntry *entry = &store->entries[indices[i]];
                if (!changes[i].force_revision && entry->length == changes[i].length &&
                    memcmp(entry->text, prepared[i], entry->length) == 0)
                    continue;
                char *previous = entry->text;
                entry->text = prepared[i];
                prepared[i] = previous;
                entry->length = changes[i].length;
                entry->capacity = changes[i].length + 1U;
                ++entry->revision;
                entry->external_change = 0;
            }
        (void)umi_mutex_unlock(store->mutex);
    }
    for (size_t i = 0U; i < count; ++i)
        free(prepared[i]);
    free(prepared);
    free(indices);
    return status;
}
