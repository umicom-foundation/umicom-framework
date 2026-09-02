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

/*
 * Initialise idempotency store from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_idempotency_store_create(size_t capacity,
                                       UmiIdempotencyStore **out_store)
{
    UmiIdempotencyStore *store;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (capacity == 0U || out_store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_store = NULL;
    store = (UmiIdempotencyStore *)calloc(1U, sizeof(*store));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    store->entries = (UmiIdempotencyEntry *)calloc(capacity,
                                                   sizeof(*store->entries));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store->entries == NULL) {
        free(store);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    store->capacity = capacity;
    *out_store = store;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by idempotency store so the same storage can be reused
 * safely.
 */
void umi_idempotency_store_destroy(UmiIdempotencyStore *store)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) return;
    free(store->entries);
    free(store);
}

/* Provide the find key operation used by this module and its client applications. */
static size_t find_key(const UmiIdempotencyStore *store, const char *key)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < store->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(store->entries[index].key, key) == 0) return index;
    }
    return SIZE_MAX;
}

/*
 * Provide the idempotency store record operation used by this module and its client
 * applications.
 */
UmiStatus umi_idempotency_store_record(UmiIdempotencyStore *store,
                                       const char *key,
                                       UmiStatus outcome,
                                       const char *result_text)
{
    UmiIdempotencyEntry *entry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || key == NULL || key[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (find_key(store, key) != SIZE_MAX) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Find idempotency store while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_idempotency_store_lookup(const UmiIdempotencyStore *store,
                                       const char *key,
                                       UmiStatus *out_outcome,
                                       char *out_result,
                                       size_t result_capacity)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || key == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_key(store, key);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_outcome != NULL) *out_outcome = store->entries[index].outcome;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_result != NULL && result_capacity > 0U) {
        (void)snprintf(out_result,
                       result_capacity,
                       "%s",
                       store->entries[index].result);
    }
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by idempotency store without changing their
 * state.
 */
size_t umi_idempotency_store_count(const UmiIdempotencyStore *store)
{
    return store != NULL ? store->count : 0U;
}
