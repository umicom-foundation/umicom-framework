/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/document_store.c
 *
 * PURPOSE:
 *   Implement bounded document ownership, revision tracking, safe editing,
 *   atomic persistence, external-change state and immutable snapshots.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/document_store.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/atomic_file.h"
#include "umicom/platform/threading.h"

typedef struct UmiDocumentEntry {
    UmiDocumentId document_id;
    char display_name[UMI_DOCUMENT_DISPLAY_NAME_CAPACITY];
    char path[UMI_PATH_CAPACITY];
    char *text;
    size_t length;
    size_t capacity;
    uint64_t revision;
    uint64_t saved_revision;
    int external_change;
} UmiDocumentEntry;

struct UmiDocumentStore {
    UmiDocumentEntry entries[UMI_DOCUMENT_STORE_MAX];
    size_t count;
    UmiDocumentId next_document_id;
    UmiMutex *mutex;
};

static size_t umi_document_store_find_index(const UmiDocumentStore *store,
                                            UmiDocumentId document_id)
{
    size_t index;
    for (index = 0U; index < store->count; ++index) {
        if (store->entries[index].document_id == document_id) {
            return index;
        }
    }
    return SIZE_MAX;
}

static size_t umi_document_store_find_path(const UmiDocumentStore *store,
                                           const char *path)
{
    size_t index;
    for (index = 0U; index < store->count; ++index) {
        if (store->entries[index].path[0] != '\0' &&
            strcmp(store->entries[index].path, path) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

static void umi_document_entry_dispose(UmiDocumentEntry *entry)
{
    if (entry == NULL) {
        return;
    }
    free(entry->text);
    (void)memset(entry, 0, sizeof(*entry));
}

static UmiStatus umi_document_entry_reserve(UmiDocumentEntry *entry,
                                            size_t requested_capacity)
{
    char *resized;
    size_t capacity;

    if (requested_capacity <= entry->capacity) {
        return UMI_STATUS_OK;
    }

    capacity = entry->capacity > 0U ? entry->capacity : 64U;
    while (capacity < requested_capacity) {
        if (capacity > SIZE_MAX / 2U) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        capacity *= 2U;
    }

    resized = (char *)realloc(entry->text, capacity);
    if (resized == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    entry->text = resized;
    entry->capacity = capacity;
    return UMI_STATUS_OK;
}

static void umi_document_snapshot_copy(const UmiDocumentEntry *entry,
                                       UmiDocumentSnapshot *snapshot)
{
    (void)memset(snapshot, 0, sizeof(*snapshot));
    snapshot->document_id = entry->document_id;
    (void)snprintf(snapshot->display_name,
                   sizeof(snapshot->display_name),
                   "%s",
                   entry->display_name);
    (void)snprintf(snapshot->path,
                   sizeof(snapshot->path),
                   "%s",
                   entry->path);
    snapshot->length = entry->length;
    snapshot->revision = entry->revision;
    snapshot->saved_revision = entry->saved_revision;
    snapshot->dirty = entry->revision != entry->saved_revision;
    snapshot->external_change = entry->external_change;
    snapshot->has_path = entry->path[0] != '\0';
}

static UmiStatus umi_document_store_allocate_entry(
    UmiDocumentStore *store,
    const char *display_name,
    UmiDocumentEntry **out_entry)
{
    UmiDocumentEntry *entry;

    if (store->count >= UMI_DOCUMENT_STORE_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (display_name == NULL || display_name[0] == '\0' ||
        strlen(display_name) >= UMI_DOCUMENT_DISPLAY_NAME_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    entry = &store->entries[store->count++];
    (void)memset(entry, 0, sizeof(*entry));
    entry->document_id = store->next_document_id++;
    (void)snprintf(entry->display_name,
                   sizeof(entry->display_name),
                   "%s",
                   display_name);
    entry->text = (char *)calloc(1U, 1U);
    if (entry->text == NULL) {
        store->count -= 1U;
        (void)memset(entry, 0, sizeof(*entry));
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    entry->capacity = 1U;
    *out_entry = entry;
    return UMI_STATUS_OK;
}

UmiStatus umi_document_store_create(UmiDocumentStore **out_store)
{
    UmiDocumentStore *store;
    UmiStatus status;

    if (out_store == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_store = NULL;

    store = (UmiDocumentStore *)calloc(1U, sizeof(*store));
    if (store == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    status = umi_mutex_create(&store->mutex);
    if (status != UMI_STATUS_OK) {
        free(store);
        return status;
    }

    store->next_document_id = 1U;
    *out_store = store;
    return UMI_STATUS_OK;
}

void umi_document_store_destroy(UmiDocumentStore *store)
{
    size_t index;

    if (store == NULL) {
        return;
    }

    for (index = 0U; index < store->count; ++index) {
        umi_document_entry_dispose(&store->entries[index]);
    }
    umi_mutex_destroy(store->mutex);
    free(store);
}

UmiStatus umi_document_store_new(UmiDocumentStore *store,
                                 const char *display_name,
                                 UmiDocumentId *out_document_id)
{
    UmiDocumentEntry *entry;
    UmiStatus status;

    if (store == NULL || out_document_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_document_id = 0U;

    (void)umi_mutex_lock(store->mutex);
    status = umi_document_store_allocate_entry(store, display_name, &entry);
    if (status == UMI_STATUS_OK) {
        *out_document_id = entry->document_id;
    }
    (void)umi_mutex_unlock(store->mutex);
    return status;
}

UmiStatus umi_document_store_open(UmiDocumentStore *store,
                                  const char *path,
                                  UmiDocumentId *out_document_id)
{
    char normalised[UMI_PATH_CAPACITY];
    char *text = NULL;
    size_t length = 0U;
    const char *display_name;
    const char *separator;
    UmiDocumentEntry *entry;
    UmiStatus status;

    if (store == NULL || path == NULL || path[0] == '\0' ||
        out_document_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_document_id = 0U;

    if (strlen(path) >= sizeof(normalised)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)snprintf(normalised, sizeof(normalised), "%s", path);
    (void)umi_fs_normalise(normalised);

    status = umi_fs_read_text(normalised, &text, &length);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    separator = strrchr(normalised, '/');
#ifdef _WIN32
    {
        const char *backslash = strrchr(normalised, '\\');
        if (backslash != NULL && (separator == NULL || backslash > separator)) {
            separator = backslash;
        }
    }
#endif
    display_name = separator != NULL ? separator + 1 : normalised;

    (void)umi_mutex_lock(store->mutex);
    if (umi_document_store_find_path(store, normalised) != SIZE_MAX) {
        (void)umi_mutex_unlock(store->mutex);
        umi_fs_free_text(text);
        return UMI_STATUS_ALREADY_EXISTS;
    }

    status = umi_document_store_allocate_entry(store,
                                               display_name,
                                               &entry);
    if (status == UMI_STATUS_OK) {
        free(entry->text);
        entry->text = text;
        entry->length = length;
        entry->capacity = length + 1U;
        entry->revision = 1U;
        entry->saved_revision = 1U;
        (void)snprintf(entry->path,
                       sizeof(entry->path),
                       "%s",
                       normalised);
        *out_document_id = entry->document_id;
        text = NULL;
    }
    (void)umi_mutex_unlock(store->mutex);

    umi_fs_free_text(text);
    return status;
}

UmiStatus umi_document_store_close(UmiDocumentStore *store,
                                   UmiDocumentId document_id,
                                   int force)
{
    size_t index;
    size_t move_count;

    if (store == NULL || document_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)umi_mutex_lock(store->mutex);
    index = umi_document_store_find_index(store, document_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(store->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    if (!force &&
        store->entries[index].revision != store->entries[index].saved_revision) {
        (void)umi_mutex_unlock(store->mutex);
        return UMI_STATUS_INVALID_STATE;
    }

    umi_document_entry_dispose(&store->entries[index]);
    move_count = store->count - index - 1U;
    if (move_count > 0U) {
        (void)memmove(&store->entries[index],
                      &store->entries[index + 1U],
                      move_count * sizeof(store->entries[0]));
    }
    store->count -= 1U;
    (void)memset(&store->entries[store->count],
                 0,
                 sizeof(store->entries[0]));
    (void)umi_mutex_unlock(store->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_document_store_snapshot(const UmiDocumentStore *store,
                                      UmiDocumentId document_id,
                                      UmiDocumentSnapshot *out_snapshot)
{
    size_t index;
    UmiDocumentStore *mutable_store;

    if (store == NULL || document_id == 0U || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    mutable_store = (UmiDocumentStore *)store;
    (void)umi_mutex_lock(mutable_store->mutex);
    index = umi_document_store_find_index(store, document_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(mutable_store->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    umi_document_snapshot_copy(&store->entries[index], out_snapshot);
    (void)umi_mutex_unlock(mutable_store->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_document_store_at(const UmiDocumentStore *store,
                                size_t index,
                                UmiDocumentSnapshot *out_snapshot)
{
    UmiDocumentStore *mutable_store;

    if (store == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    mutable_store = (UmiDocumentStore *)store;
    (void)umi_mutex_lock(mutable_store->mutex);
    if (index >= store->count) {
        (void)umi_mutex_unlock(mutable_store->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    umi_document_snapshot_copy(&store->entries[index], out_snapshot);
    (void)umi_mutex_unlock(mutable_store->mutex);
    return UMI_STATUS_OK;
}

size_t umi_document_store_count(const UmiDocumentStore *store)
{
    size_t count;
    UmiDocumentStore *mutable_store;

    if (store == NULL) {
        return 0U;
    }

    mutable_store = (UmiDocumentStore *)store;
    (void)umi_mutex_lock(mutable_store->mutex);
    count = store->count;
    (void)umi_mutex_unlock(mutable_store->mutex);
    return count;
}

UmiStatus umi_document_store_copy_text(const UmiDocumentStore *store,
                                       UmiDocumentId document_id,
                                       char **out_text,
                                       size_t *out_length)
{
    size_t index;
    char *copy;
    UmiDocumentStore *mutable_store;

    if (store == NULL || document_id == 0U || out_text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_text = NULL;
    if (out_length != NULL) {
        *out_length = 0U;
    }

    mutable_store = (UmiDocumentStore *)store;
    (void)umi_mutex_lock(mutable_store->mutex);
    index = umi_document_store_find_index(store, document_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(mutable_store->mutex);
        return UMI_STATUS_NOT_FOUND;
    }

    copy = (char *)malloc(store->entries[index].length + 1U);
    if (copy == NULL) {
        (void)umi_mutex_unlock(mutable_store->mutex);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    (void)memcpy(copy,
                 store->entries[index].text,
                 store->entries[index].length + 1U);
    if (out_length != NULL) {
        *out_length = store->entries[index].length;
    }
    (void)umi_mutex_unlock(mutable_store->mutex);
    *out_text = copy;
    return UMI_STATUS_OK;
}

void umi_document_store_free_text(char *text)
{
    free(text);
}

UmiStatus umi_document_store_replace_text(UmiDocumentStore *store,
                                          UmiDocumentId document_id,
                                          const char *text,
                                          size_t length)
{
    size_t index;
    UmiDocumentEntry *entry;
    UmiStatus status;

    if (store == NULL || document_id == 0U ||
        (text == NULL && length > 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)umi_mutex_lock(store->mutex);
    index = umi_document_store_find_index(store, document_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(store->mutex);
        return UMI_STATUS_NOT_FOUND;
    }

    entry = &store->entries[index];
    status = umi_document_entry_reserve(entry, length + 1U);
    if (status == UMI_STATUS_OK) {
        if (length > 0U) {
            (void)memcpy(entry->text, text, length);
        }
        entry->text[length] = '\0';
        entry->length = length;
        entry->revision += 1U;
        entry->external_change = 0;
    }
    (void)umi_mutex_unlock(store->mutex);
    return status;
}

UmiStatus umi_document_store_insert(UmiDocumentStore *store,
                                    UmiDocumentId document_id,
                                    size_t offset,
                                    const char *text,
                                    size_t length)
{
    size_t index;
    UmiDocumentEntry *entry;
    UmiStatus status;

    if (store == NULL || document_id == 0U ||
        (text == NULL && length > 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)umi_mutex_lock(store->mutex);
    index = umi_document_store_find_index(store, document_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(store->mutex);
        return UMI_STATUS_NOT_FOUND;
    }

    entry = &store->entries[index];
    if (offset > entry->length) {
        (void)umi_mutex_unlock(store->mutex);
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_document_entry_reserve(entry, entry->length + length + 1U);
    if (status == UMI_STATUS_OK && length > 0U) {
        (void)memmove(entry->text + offset + length,
                      entry->text + offset,
                      entry->length - offset + 1U);
        (void)memcpy(entry->text + offset, text, length);
        entry->length += length;
        entry->revision += 1U;
        entry->external_change = 0;
    }
    (void)umi_mutex_unlock(store->mutex);
    return status;
}

UmiStatus umi_document_store_erase(UmiDocumentStore *store,
                                   UmiDocumentId document_id,
                                   size_t offset,
                                   size_t length)
{
    size_t index;
    UmiDocumentEntry *entry;

    if (store == NULL || document_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)umi_mutex_lock(store->mutex);
    index = umi_document_store_find_index(store, document_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(store->mutex);
        return UMI_STATUS_NOT_FOUND;
    }

    entry = &store->entries[index];
    if (offset > entry->length || length > entry->length - offset) {
        (void)umi_mutex_unlock(store->mutex);
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (length > 0U) {
        (void)memmove(entry->text + offset,
                      entry->text + offset + length,
                      entry->length - offset - length + 1U);
        entry->length -= length;
        entry->revision += 1U;
        entry->external_change = 0;
    }
    (void)umi_mutex_unlock(store->mutex);
    return UMI_STATUS_OK;
}

static UmiStatus umi_document_store_save_path(UmiDocumentStore *store,
                                              UmiDocumentId document_id,
                                              const char *override_path)
{
    size_t index;
    char path[UMI_PATH_CAPACITY];
    char *text;
    size_t length;
    uint64_t revision;
    UmiStatus status;

    (void)umi_mutex_lock(store->mutex);
    index = umi_document_store_find_index(store, document_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(store->mutex);
        return UMI_STATUS_NOT_FOUND;
    }

    if (override_path != NULL) {
        if (override_path[0] == '\0' ||
            strlen(override_path) >= sizeof(path)) {
            (void)umi_mutex_unlock(store->mutex);
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        (void)snprintf(path, sizeof(path), "%s", override_path);
        (void)umi_fs_normalise(path);
    } else {
        if (store->entries[index].path[0] == '\0') {
            (void)umi_mutex_unlock(store->mutex);
            return UMI_STATUS_INVALID_STATE;
        }
        (void)snprintf(path,
                       sizeof(path),
                       "%s",
                       store->entries[index].path);
    }

    length = store->entries[index].length;
    revision = store->entries[index].revision;
    text = (char *)malloc(length + 1U);
    if (text == NULL) {
        (void)umi_mutex_unlock(store->mutex);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    (void)memcpy(text, store->entries[index].text, length + 1U);
    (void)umi_mutex_unlock(store->mutex);

    status = umi_atomic_file_write(path, text, length);
    free(text);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    (void)umi_mutex_lock(store->mutex);
    index = umi_document_store_find_index(store, document_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(store->mutex);
        return UMI_STATUS_NOT_FOUND;
    }

    if (override_path != NULL) {
        const char *separator = strrchr(path, '/');
#ifdef _WIN32
        const char *backslash = strrchr(path, '\\');
        if (backslash != NULL && (separator == NULL || backslash > separator)) {
            separator = backslash;
        }
#endif
        (void)snprintf(store->entries[index].path,
                       sizeof(store->entries[index].path),
                       "%s",
                       path);
        (void)snprintf(store->entries[index].display_name,
                       sizeof(store->entries[index].display_name),
                       "%s",
                       separator != NULL ? separator + 1 : path);
    }
    if (store->entries[index].revision == revision) {
        store->entries[index].saved_revision = revision;
        store->entries[index].external_change = 0;
    }
    (void)umi_mutex_unlock(store->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_document_store_save(UmiDocumentStore *store,
                                  UmiDocumentId document_id)
{
    if (store == NULL || document_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_document_store_save_path(store, document_id, NULL);
}

UmiStatus umi_document_store_save_as(UmiDocumentStore *store,
                                     UmiDocumentId document_id,
                                     const char *path)
{
    if (store == NULL || document_id == 0U || path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_document_store_save_path(store, document_id, path);
}

UmiStatus umi_document_store_mark_external_change(
    UmiDocumentStore *store,
    UmiDocumentId document_id,
    int changed)
{
    size_t index;

    if (store == NULL || document_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)umi_mutex_lock(store->mutex);
    index = umi_document_store_find_index(store, document_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(store->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    store->entries[index].external_change = changed != 0;
    (void)umi_mutex_unlock(store->mutex);
    return UMI_STATUS_OK;
}
