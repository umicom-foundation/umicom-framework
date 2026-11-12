/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_order_storage_admission.c
 *
 * PURPOSE:
 *   Exercise bounded complete order identity admission and borrowed lookup outputs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test_trading_common.h"

#define CHECK(expression) do { \
    if (!(expression)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); \
        return 1; \
    } \
} while (0)

int main(void)
{
    UmiOrderStore *store = calloc(1U, sizeof(*store));
    UmiOrderStore *before = malloc(sizeof(*before));
    CHECK(store != NULL && before != NULL);
    UmiOrder input = {0};
    input.request = test_order_request();
    input.status = UMI_ORDER_ACCEPTED;
    input.version = 1U;
    UmiOrder *found = &input;
    const UmiOrder *read_only = &input;
    CHECK(umi_order_store_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_order_store_validate(store) == UMI_STATUS_OK);
    CHECK(umi_order_store_find_const(store, "missing", &read_only) == UMI_STATUS_NOT_FOUND);
    CHECK(read_only == NULL);
    CHECK(umi_order_store_add(store, &input) == UMI_STATUS_OK);
    CHECK(umi_order_store_find_const(store, "ORDER-1", &read_only) == UMI_STATUS_OK);
    CHECK(read_only == &store->orders[0]);
    CHECK(umi_order_store_find(store, "ORDER-1", &found) == UMI_STATUS_OK);
    CHECK(found == &store->orders[0]);

    /* A match in row zero must not hide a damaged later active identity. */
    store->orders[1] = input;
    store->count = 2U;
    CHECK(umi_order_store_validate(store) == UMI_STATUS_INVALID_STATE);
    CHECK(umi_order_store_find(store, "ORDER-1", &found) == UMI_STATUS_INVALID_STATE);
    CHECK(found == NULL);
    memset(store->orders[1].request.client_order_id.value, 'x',
           sizeof(store->orders[1].request.client_order_id.value));
    memcpy(before, store, sizeof(*store));
    CHECK(umi_order_store_find_const(store, "ORDER-1", &read_only) == UMI_STATUS_INVALID_STATE);
    CHECK(read_only == NULL);
    CHECK(umi_order_store_add(store, &input) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(before, store, sizeof(*store)) == 0);
    store->count = 1U; // The malformed inactive row is outside the active contract.
    CHECK(umi_order_store_validate(store) == UMI_STATUS_OK);

    char query[sizeof(input.request.client_order_id.value)];
    memset(query, 'q', sizeof(query));
    CHECK(umi_order_store_find(store, query, &found) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(found == NULL);
    CHECK(umi_order_store_find(store, "", &found) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(found == NULL);
    memcpy(before, store, sizeof(*store));
    CHECK(umi_order_store_find(store, "ORDER-1", (UmiOrder **)&store->orders[0])
          == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_order_store_find_const(store, "ORDER-1", (const UmiOrder **)&store->orders[0])
          == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(before, store, sizeof(*store)) == 0);

    /* Query capture must finish before publishing pointer bytes into this union. */
    union { char text[sizeof(UmiFinancialId)]; UmiOrder *output; } aliased = {0};
    strcpy(aliased.text, "ORDER-1");
    CHECK(umi_order_store_find(store, aliased.text, &aliased.output) == UMI_STATUS_OK);
    CHECK(aliased.output == &store->orders[0]);

    store->count = SIZE_MAX;
    CHECK(umi_order_store_validate(store) == UMI_STATUS_INVALID_STATE);
    CHECK(umi_order_store_find(store, "ORDER-1", &found) == UMI_STATUS_INVALID_STATE);
    CHECK(found == NULL);
    store->count = UMI_TRADING_MAX_ORDERS + 1U;
    CHECK(umi_order_store_add(store, &input) == UMI_STATUS_INVALID_STATE);

    umi_order_store_init(store);
    for (size_t i = 0U; i < UMI_TRADING_MAX_ORDERS; ++i) {
        (void)snprintf(input.request.client_order_id.value,
                       sizeof(input.request.client_order_id.value), "order-%zu", i);
        CHECK(umi_order_store_add(store, &input) == UMI_STATUS_OK);
    }
    memcpy(before, store, sizeof(*store));
    CHECK(umi_order_store_add(store, &store->orders[0]) == UMI_STATUS_ALREADY_EXISTS);
    strcpy(input.request.client_order_id.value, "extra");
    CHECK(umi_order_store_add(store, &input) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(before, store, sizeof(*store)) == 0);

    umi_order_store_init(store);
    store->orders[7] = input;
    CHECK(umi_order_store_add(store, &store->orders[7]) == UMI_STATUS_OK);
    CHECK(store->count == 1U && strcmp(store->orders[0].request.client_order_id.value, "extra") == 0);
    memset(input.request.client_order_id.value, 'z', sizeof(input.request.client_order_id.value));
    input.request.client_order_id.value[sizeof(input.request.client_order_id.value) - 1U] = '\0';
    CHECK(umi_order_store_add(store, &input) == UMI_STATUS_OK);
    CHECK(umi_order_store_find_const(store, input.request.client_order_id.value, &read_only)
          == UMI_STATUS_OK);
    CHECK(read_only == &store->orders[1]);
    free(before);
    free(store);
    return 0;
}
