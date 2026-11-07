/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_idempotency.c
 *
 * PURPOSE:
 *   Verify idempotency-key recording, duplicate rejection and stable result lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Required fixture setup and checks also execute in optimized builds. */
#include "support/assertions.h"
#include "umicom/umicom.h"

#include <assert.h>
#include <string.h>

/* Boundaries must preserve identity, capacity and caller outputs together. */
static void test_complete_records(void)
{
    UmiIdempotencyStore *store = NULL;
    char key[UMI_IDEMPOTENCY_KEY_CAPACITY + 1U];
    char text[UMI_IDEMPOTENCY_RESULT_CAPACITY + 1U];
    char copy[UMI_IDEMPOTENCY_RESULT_CAPACITY];
    UmiStatus outcome = UMI_STATUS_BUSY;
    memset(key, 'k', sizeof(key));
    key[sizeof(key) - 1U] = '\0';
    memset(text, 'r', sizeof(text));
    text[sizeof(text) - 1U] = '\0';
    assert(umi_idempotency_store_create(1U, &store) == UMI_STATUS_OK);
    assert(umi_idempotency_store_record(store, key, UMI_STATUS_OK, "") ==
           UMI_STATUS_CAPACITY_EXCEEDED);
    assert(umi_idempotency_store_record(store, "valid", UMI_STATUS_OK, text) ==
           UMI_STATUS_CAPACITY_EXCEEDED);
    assert(umi_idempotency_store_count(store) == 0U);
    key[UMI_IDEMPOTENCY_KEY_CAPACITY - 1U] = '\0';
    text[UMI_IDEMPOTENCY_RESULT_CAPACITY - 1U] = '\0';
    assert(umi_idempotency_store_record(store, key, UMI_STATUS_OK, text) == UMI_STATUS_OK);
    assert(umi_idempotency_store_count(store) == 1U);
    assert(umi_idempotency_store_record(store, key, UMI_STATUS_BUSY, "different") ==
           UMI_STATUS_ALREADY_EXISTS);
    assert(umi_idempotency_store_record(store, "other", UMI_STATUS_OK, "") ==
           UMI_STATUS_CAPACITY_EXCEEDED);
    memset(copy, 'Q', sizeof(copy));
    assert(umi_idempotency_store_lookup(store, key, &outcome, copy, sizeof(copy) - 1U) ==
           UMI_STATUS_CAPACITY_EXCEEDED);
    assert(outcome == UMI_STATUS_BUSY);
    for (size_t i = 0U; i < sizeof(copy); ++i) assert(copy[i] == 'Q');
    assert(umi_idempotency_store_lookup(store, key, &outcome, copy, 0U) ==
           UMI_STATUS_CAPACITY_EXCEEDED);
    assert(outcome == UMI_STATUS_BUSY && copy[0] == 'Q');
    assert(umi_idempotency_store_lookup(store, key, &outcome, copy, sizeof(copy)) ==
           UMI_STATUS_OK);
    assert(outcome == UMI_STATUS_OK && strcmp(copy, text) == 0);
    outcome = UMI_STATUS_BUSY;
    assert(umi_idempotency_store_lookup(store, key, &outcome, NULL, 0U) == UMI_STATUS_OK);
    assert(outcome == UMI_STATUS_OK);
    key[UMI_IDEMPOTENCY_KEY_CAPACITY - 1U] = 'k';
    assert(umi_idempotency_store_lookup(store, key, NULL, NULL, 0U) ==
           UMI_STATUS_CAPACITY_EXCEEDED);
    umi_idempotency_store_destroy(store);
}

static void test_empty_and_missing_results(void)
{
    UmiIdempotencyStore *store = NULL;
    char output[2] = {'Q', 'Q'};
    UmiStatus outcome = UMI_STATUS_BUSY;
    assert(umi_idempotency_store_create(1U, &store) == UMI_STATUS_OK);
    assert(umi_idempotency_store_record(store, "", UMI_STATUS_OK, "") ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_idempotency_store_record(store, "empty", UMI_STATUS_OK, NULL) == UMI_STATUS_OK);
    assert(umi_idempotency_store_lookup(store, "missing", &outcome, output, sizeof(output)) ==
           UMI_STATUS_NOT_FOUND);
    assert(outcome == UMI_STATUS_BUSY && output[0] == 'Q' && output[1] == 'Q');
    assert(umi_idempotency_store_lookup(store, "empty", &outcome, output, 1U) == UMI_STATUS_OK);
    assert(outcome == UMI_STATUS_OK && output[0] == '\0' && output[1] == 'Q');
    umi_idempotency_store_destroy(store);
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    test_complete_records();
    test_empty_and_missing_results();
    UmiIdempotencyStore *store = NULL;
    UmiStatus outcome;
    char result[64];
    assert(umi_idempotency_store_create(4U, &store) == UMI_STATUS_OK);
    assert(umi_idempotency_store_record(store, "request-1",
                                        UMI_STATUS_OK, "accepted") == UMI_STATUS_OK);
    assert(umi_idempotency_store_record(store, "request-1",
                                        UMI_STATUS_OK, "again") == UMI_STATUS_ALREADY_EXISTS);
    assert(umi_idempotency_store_lookup(store, "request-1", &outcome,
                                        result, sizeof(result)) == UMI_STATUS_OK);
    assert(outcome == UMI_STATUS_OK && strcmp(result, "accepted") == 0);
    umi_idempotency_store_destroy(store);
    return 0;
}
