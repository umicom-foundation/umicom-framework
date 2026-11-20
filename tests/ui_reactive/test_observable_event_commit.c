/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_observable_event_commit.c
 *
 * PURPOSE:
 *   Exercise atomic reactive widget edits under an observed state revision.
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
#include "umicom/ui/reactive/observable_store.h"

#define CHECK_EVENT(condition) do { \
    if (!(condition)) { fprintf(stderr, "%d: %s\n", __LINE__, #condition); result = 1; goto done; } \
} while (0)

static UmiUiValue integer(int64_t value)
{
    return (UmiUiValue){.kind = UMI_UI_VALUE_INTEGER, .integer_value = value};
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *test = argv[1];
    UmiUiReactiveObservableStore *store = malloc(sizeof *store);
    UmiUiReactiveObservableStore *before = malloc(sizeof *before);
    UmiUiReactiveChangeTransaction transaction = {0};
    UmiUiReactiveBatchUpdate batch = {.end_revision = 777U};
    UmiUiReactiveChangeEvent events[2] = {0};
    size_t count = sizeof events / sizeof events[0];
    int result = 0;
    CHECK_EVENT(store != NULL && before != NULL);
    umi_ui_reactive_observable_store_init(store);
    const UmiUiValue one = integer(1);
    CHECK_EVENT(umi_ui_reactive_observable_store_put(store, "filter.low", &one) == UMI_STATUS_OK);
    CHECK_EVENT(umi_ui_reactive_observable_store_put(store, "filter.high", &one) == UMI_STATUS_OK);
    CHECK_EVENT(umi_ui_reactive_observable_store_put(store, "view.mode", &one) == UMI_STATUS_OK);
    strcpy(events[0].path, "filter.low");
    strcpy(events[1].path, "filter.high");
    events[0].before_value = events[1].before_value = one;
    events[0].after_value = integer(10);
    events[1].after_value = integer(20);
    CHECK_EVENT(umi_ui_reactive_change_transaction_begin(&transaction, "story.event", store->revision) == UMI_STATUS_OK);

    UmiStatus expected = UMI_STATUS_OK;
    if (!strcmp(test, "alias")) {
        CHECK_EVENT(umi_ui_reactive_change_transaction_begin(&transaction,
            transaction.transaction_id, store->revision) == UMI_STATUS_OK);
        CHECK_EVENT(!strcmp(transaction.transaction_id, "story.event"));
    } else if (!strcmp(test, "invalid-id")) {
        char unterminated[UMI_UI_REACTIVE_ID_CAPACITY];
        memset(unterminated, 'x', sizeof unterminated);
        CHECK_EVENT(umi_ui_reactive_change_transaction_begin(&transaction, unterminated, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK_EVENT(!strcmp(transaction.transaction_id, "story.event") && transaction.base_revision == store->revision);
    } else if (!strcmp(test, "stale")) {
        CHECK_EVENT(umi_ui_reactive_observable_store_put(store, "view.mode", &one) == UMI_STATUS_OK);
        expected = UMI_STATUS_INVALID_STATE;
    } else if (!strcmp(test, "previous-value")) {
        events[1].before_value = integer(7);
        expected = UMI_STATUS_INVALID_STATE;
    } else if (!strcmp(test, "unknown-path")) {
        strcpy(events[1].path, "filter.undeclared");
        expected = UMI_STATUS_NOT_FOUND;
    } else if (!strcmp(test, "duplicate-path")) {
        strcpy(events[1].path, events[0].path);
        expected = UMI_STATUS_ALREADY_EXISTS;
    } else if (!strcmp(test, "invalid-value")) {
        events[1].after_value.kind = (UmiUiValueKind)99;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (!strcmp(test, "empty")) {
        count = 0U;
    } else if (!strcmp(test, "last") || !strcmp(test, "exhausted")) {
        store->revision = !strcmp(test, "last") ? UINT64_MAX - 1U : UINT64_MAX;
        transaction.base_revision = store->revision;
        if (store->revision == UINT64_MAX) expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(test, "commit") != 0) {
        result = 2;
        goto done;
    }

    memcpy(before, store, sizeof *before);
    UmiUiReactiveChangeTransaction saved_transaction;
    UmiUiReactiveBatchUpdate saved_batch;
    memcpy(&saved_transaction, &transaction, sizeof saved_transaction);
    memcpy(&saved_batch, &batch, sizeof saved_batch);
    CHECK_EVENT(umi_ui_reactive_observable_store_apply(store, &transaction,
        count == 0U ? NULL : events, count, &batch) == expected);
    if (expected != UMI_STATUS_OK) {
        CHECK_EVENT(memcmp(store, before, sizeof *store) == 0);
        CHECK_EVENT(memcmp(&transaction, &saved_transaction, sizeof transaction) == 0);
        CHECK_EVENT(memcmp(&batch, &saved_batch, sizeof batch) == 0);
        goto done;
    }

    CHECK_EVENT(transaction.state == UMI_UI_REACTIVE_TRANSACTION_COMMITTED && transaction.change_count == count);
    CHECK_EVENT(batch.committed && batch.mutation_count == count && batch.start_revision == before->revision);
    CHECK_EVENT(batch.end_revision == store->revision && store->revision == before->revision + (count != 0U));
    if (count != 0U) {
        const UmiUiValue ten = integer(10);
        const UmiUiValue twenty = integer(20);
        CHECK_EVENT(store->items[0].revision == store->revision && store->items[1].revision == store->revision);
        CHECK_EVENT(umi_ui_value_equal(&store->items[0].value, &ten));
        CHECK_EVENT(umi_ui_value_equal(&store->items[1].value, &twenty));
        CHECK_EVENT(store->items[2].revision == before->items[2].revision);
    } else {
        CHECK_EVENT(memcmp(store, before, sizeof *store) == 0);
    }
    CHECK_EVENT(umi_ui_reactive_observable_store_apply(store, &transaction, events, count, &batch)
        == UMI_STATUS_INVALID_STATE);
done:
    free(before);
    free(store);
    return result;
}
