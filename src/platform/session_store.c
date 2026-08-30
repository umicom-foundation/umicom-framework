/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/session_store.c
 *
 * PURPOSE:
 *   Implement bounded session state, key validation, deterministic persistence
 *   and atomic replacement for product and workspace restoration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/session_store.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/atomic_file.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/threading.h"

typedef struct UmiSessionEntry {
    char key[UMI_SESSION_KEY_CAPACITY];
    char value[UMI_SESSION_VALUE_CAPACITY];
} UmiSessionEntry;

struct UmiSessionStore {
    UmiSessionEntry entries[UMI_SESSION_STORE_MAX];
    size_t count;
    UmiMutex *mutex;
};

static size_t umi_session_find_index(const UmiSessionStore *store,
                                     const char *key)
{
    size_t index;
    for (index = 0U; index < store->count; ++index) {
        if (strcmp(store->entries[index].key, key) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

static int umi_session_key_valid(const char *key)
{
    size_t index;

    if (key == NULL || key[0] == '\0' ||
        strlen(key) >= UMI_SESSION_KEY_CAPACITY) {
        return 0;
    }

    for (index = 0U; key[index] != '\0'; ++index) {
        unsigned char value = (unsigned char)key[index];
        if (!(isalnum(value) || value == '.' || value == '_' ||
              value == '-' || value == '/')) {
            return 0;
        }
    }
    return 1;
}

static int umi_session_value_valid(const char *value)
{
    return value != NULL &&
           strlen(value) < UMI_SESSION_VALUE_CAPACITY &&
           strchr(value, '\n') == NULL &&
           strchr(value, '\r') == NULL;
}

UmiStatus umi_session_store_create(UmiSessionStore **out_store)
{
    UmiSessionStore *store;
    UmiStatus status;

    if (out_store == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_store = NULL;

    store = (UmiSessionStore *)calloc(1U, sizeof(*store));
    if (store == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    status = umi_mutex_create(&store->mutex);
    if (status != UMI_STATUS_OK) {
        free(store);
        return status;
    }

    *out_store = store;
    return UMI_STATUS_OK;
}

void umi_session_store_destroy(UmiSessionStore *store)
{
    if (store == NULL) {
        return;
    }
    umi_mutex_destroy(store->mutex);
    free(store);
}

UmiStatus umi_session_store_set(UmiSessionStore *store,
                                const char *key,
                                const char *value)
{
    size_t index;

    if (store == NULL || !umi_session_key_valid(key) ||
        !umi_session_value_valid(value)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)umi_mutex_lock(store->mutex);
    index = umi_session_find_index(store, key);
    if (index == SIZE_MAX) {
        if (store->count >= UMI_SESSION_STORE_MAX) {
            (void)umi_mutex_unlock(store->mutex);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = store->count++;
        (void)snprintf(store->entries[index].key,
                       sizeof(store->entries[index].key),
                       "%s",
                       key);
    }
    (void)snprintf(store->entries[index].value,
                   sizeof(store->entries[index].value),
                   "%s",
                   value);
    (void)umi_mutex_unlock(store->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_session_store_get(const UmiSessionStore *store,
                                const char *key,
                                char *out_value,
                                size_t value_capacity)
{
    size_t index;
    size_t length;
    UmiSessionStore *mutable_store;

    if (store == NULL || key == NULL || out_value == NULL ||
        value_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    mutable_store = (UmiSessionStore *)store;
    (void)umi_mutex_lock(mutable_store->mutex);
    index = umi_session_find_index(store, key);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(mutable_store->mutex);
        return UMI_STATUS_NOT_FOUND;
    }

    length = strlen(store->entries[index].value);
    if (length + 1U > value_capacity) {
        (void)umi_mutex_unlock(mutable_store->mutex);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(out_value, store->entries[index].value, length + 1U);
    (void)umi_mutex_unlock(mutable_store->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_session_store_remove(UmiSessionStore *store,
                                   const char *key)
{
    size_t index;
    size_t move_count;

    if (store == NULL || key == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)umi_mutex_lock(store->mutex);
    index = umi_session_find_index(store, key);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(store->mutex);
        return UMI_STATUS_NOT_FOUND;
    }

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

void umi_session_store_clear(UmiSessionStore *store)
{
    if (store == NULL) {
        return;
    }
    (void)umi_mutex_lock(store->mutex);
    (void)memset(store->entries, 0, sizeof(store->entries));
    store->count = 0U;
    (void)umi_mutex_unlock(store->mutex);
}

size_t umi_session_store_count(const UmiSessionStore *store)
{
    size_t count;
    UmiSessionStore *mutable_store;

    if (store == NULL) {
        return 0U;
    }

    mutable_store = (UmiSessionStore *)store;
    (void)umi_mutex_lock(mutable_store->mutex);
    count = store->count;
    (void)umi_mutex_unlock(mutable_store->mutex);
    return count;
}

UmiStatus umi_session_store_at(const UmiSessionStore *store,
                               size_t index,
                               UmiSessionEntrySnapshot *out_snapshot)
{
    UmiSessionStore *mutable_store;

    if (store == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    mutable_store = (UmiSessionStore *)store;
    (void)umi_mutex_lock(mutable_store->mutex);
    if (index >= store->count) {
        (void)umi_mutex_unlock(mutable_store->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    (void)snprintf(out_snapshot->key,
                   sizeof(out_snapshot->key),
                   "%s",
                   store->entries[index].key);
    (void)snprintf(out_snapshot->value,
                   sizeof(out_snapshot->value),
                   "%s",
                   store->entries[index].value);
    (void)umi_mutex_unlock(mutable_store->mutex);
    return UMI_STATUS_OK;
}

static char *umi_session_trim(char *text)
{
    char *end;

    while (*text != '\0' && isspace((unsigned char)*text)) {
        text += 1;
    }
    end = text + strlen(text);
    while (end > text && isspace((unsigned char)end[-1])) {
        end -= 1;
    }
    *end = '\0';
    return text;
}

UmiStatus umi_session_store_load(UmiSessionStore *store,
                                 const char *path,
                                 int *out_loaded)
{
    char *text = NULL;
    char *cursor;
    UmiStatus status;

    if (store == NULL || path == NULL || path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (out_loaded != NULL) {
        *out_loaded = 0;
    }

    if (!umi_fs_is_file(path)) {
        return UMI_STATUS_OK;
    }

    status = umi_fs_read_text(path, &text, NULL);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    umi_session_store_clear(store);
    cursor = text;
    while (*cursor != '\0') {
        char *line = cursor;
        char *newline = strchr(cursor, '\n');
        char *equals;
        char *key;
        char *value;

        if (newline != NULL) {
            *newline = '\0';
            cursor = newline + 1;
        } else {
            cursor += strlen(cursor);
        }

        line = umi_session_trim(line);
        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }

        equals = strchr(line, '=');
        if (equals == NULL) {
            umi_fs_free_text(text);
            return UMI_STATUS_PARSE_ERROR;
        }
        *equals = '\0';
        key = umi_session_trim(line);
        value = umi_session_trim(equals + 1);

        status = umi_session_store_set(store, key, value);
        if (status != UMI_STATUS_OK) {
            umi_fs_free_text(text);
            return status;
        }
    }

    umi_fs_free_text(text);
    if (out_loaded != NULL) {
        *out_loaded = 1;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_session_store_save(const UmiSessionStore *store,
                                 const char *path)
{
    size_t index;
    size_t required = 1U;
    char *text;
    size_t offset = 0U;
    UmiSessionStore *mutable_store;
    UmiStatus status;

    if (store == NULL || path == NULL || path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    mutable_store = (UmiSessionStore *)store;
    (void)umi_mutex_lock(mutable_store->mutex);
    for (index = 0U; index < store->count; ++index) {
        size_t line_size = strlen(store->entries[index].key) +
                           strlen(store->entries[index].value) + 2U;
        if (required > SIZE_MAX - line_size) {
            (void)umi_mutex_unlock(mutable_store->mutex);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        required += line_size;
    }

    text = (char *)malloc(required);
    if (text == NULL) {
        (void)umi_mutex_unlock(mutable_store->mutex);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    text[0] = '\0';

    for (index = 0U; index < store->count; ++index) {
        int written = snprintf(text + offset,
                               required - offset,
                               "%s=%s\n",
                               store->entries[index].key,
                               store->entries[index].value);
        if (written < 0 || (size_t)written >= required - offset) {
            free(text);
            (void)umi_mutex_unlock(mutable_store->mutex);
            return UMI_STATUS_INTERNAL_ERROR;
        }
        offset += (size_t)written;
    }
    (void)umi_mutex_unlock(mutable_store->mutex);

    status = umi_atomic_file_write(path, text, offset);
    free(text);
    return status;
}
