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
#include "umicom/umicom.h"

#include <assert.h>
#include <string.h>

int main(void)
{
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
