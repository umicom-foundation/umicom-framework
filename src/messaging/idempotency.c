/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/idempotency.c
 *
 * PURPOSE:
 *   Implement bounded idempotency-key recording and deterministic result lookup for retried external and internal commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/idempotency.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UMI_IDEMPOTENCY_KEY_CAPACITY 160U
#define UMI_IDEMPOTENCY_RESULT_CAPACITY 512U

typedef struct UmiIdempotencyEntry {
    char key[UMI_IDEMPOTENCY_KEY_CAPACITY];
    UmiStatus outcome;
    char result[UMI_IDEMPOTENCY_RESULT_CAPACITY];
} UmiIdempotencyEntry;

struct UmiIdempotencyStore {
    UmiIdempotencyEntry *entries;
    size_t capacity;
    size_t count;
};

UmiStatus umi_idempotency_store_create(size_t capacity,
                                       UmiIdempotencyStore **out_store)
{
    UmiIdempotencyStore *store;
    if (capacity == 0U || out_store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_store = NULL;
    store = (UmiIdempotencyStore *)calloc(1U, sizeof(*store));
    if (store == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    store->entries = (UmiIdempotencyEntry *)calloc(capacity,
                                                   sizeof(*store->entries));
    if (store->entries == NULL) {
        free(store);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    store->capacity = capacity;
    *out_store = store;
    return UMI_STATUS_OK;
}

void umi_idempotency_store_destroy(UmiIdempotencyStore *store)
{
    if (store == NULL) return;
    free(store->entries);
    free(store);
}

static size_t find_key(const UmiIdempotencyStore *store, const char *key)
{
    size_t index;
    for (index = 0U; index < store->count; ++index) {
        if (strcmp(store->entries[index].key, key) == 0) return index;
    }
    return SIZE_MAX;
}

UmiStatus umi_idempotency_store_record(UmiIdempotencyStore *store,
                                       const char *key,
                                       UmiStatus outcome,
                                       const char *result_text)
{
    UmiIdempotencyEntry *entry;
    if (store == NULL || key == NULL || key[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (find_key(store, key) != SIZE_MAX) return UMI_STATUS_ALREADY_EXISTS;
    if (store->count >= store->capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    entry = &store->entries[store->count++];
    (void)snprintf(entry->key, sizeof(entry->key), "%s", key);
    entry->outcome = outcome;
    (void)snprintf(entry->result,
                   sizeof(entry->result),
                   "%s",
                   result_text != NULL ? result_text : "");
    return UMI_STATUS_OK;
}

UmiStatus umi_idempotency_store_lookup(const UmiIdempotencyStore *store,
                                       const char *key,
                                       UmiStatus *out_outcome,
                                       char *out_result,
                                       size_t result_capacity)
{
    size_t index;
    if (store == NULL || key == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_key(store, key);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (out_outcome != NULL) *out_outcome = store->entries[index].outcome;
    if (out_result != NULL && result_capacity > 0U) {
        (void)snprintf(out_result,
                       result_capacity,
                       "%s",
                       store->entries[index].result);
    }
    return UMI_STATUS_OK;
}

size_t umi_idempotency_store_count(const UmiIdempotencyStore *store)
{
    return store != NULL ? store->count : 0U;
}
