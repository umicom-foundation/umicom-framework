/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_observable_view_delta.c
 *
 * PURPOSE:
 *   Verify scoped, atomic render updates from the live reactive state owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/reactive/observable_store.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%d: %s\n", __LINE__, #condition); exit(EXIT_FAILURE); \
} } while (0)

static void PutNumber(UmiUiReactiveObservableStore *store, const char *name, int64_t number)
{
    UmiUiValue value;
    CHECK(umi_ui_value_set_integer(&value, number) == UMI_STATUS_OK);
    CHECK(umi_ui_reactive_observable_store_put(store, name, &value) == UMI_STATUS_OK);
}

static void ApplyRange(UmiUiReactiveObservableStore *store)
{
    UmiUiReactiveChangeEvent events[2] = {
        {.path = "range.low", .before_value = {.kind = UMI_UI_VALUE_INTEGER, .integer_value = 1},
         .after_value = {.kind = UMI_UI_VALUE_INTEGER, .integer_value = 4}},
        {.path = "range.high", .before_value = {.kind = UMI_UI_VALUE_INTEGER, .integer_value = 2},
         .after_value = {.kind = UMI_UI_VALUE_INTEGER, .integer_value = 8}},
    };
    UmiUiReactiveChangeTransaction tx = {0};
    UmiUiReactiveBatchUpdate batch;
    CHECK(umi_ui_reactive_change_transaction_begin(&tx, "view.range", store->revision) == UMI_STATUS_OK);
    CHECK(umi_ui_reactive_observable_store_apply(store, &tx, events, 2U, &batch) == UMI_STATUS_OK);
    CHECK(batch.committed && batch.mutation_count == 2U && batch.end_revision == store->revision);
}

static void CheckNumber(const UmiUiReactiveKeyValue *row, const char *key, int64_t number,
    uint64_t revision)
{
    CHECK(strcmp(row->key, key) == 0 && row->revision == revision);
    CHECK(row->value.kind == UMI_UI_VALUE_INTEGER && row->value.integer_value == number);
}

static void CheckOwnership(UmiUiReactiveObservableStore *store)
{
    UmiUiValue value = {.kind = UMI_UI_VALUE_STRING, .integer_value = 99, .boolean_value = 1,
        .real_value = 2.0, .string_value = "visible\0unused"};
    CHECK(umi_ui_reactive_observable_store_put(store, "range.low", &value) == UMI_STATUS_OK);
    store->items[0].key[20] = 'x';
    const char *const keys[] = {"range.low"};
    UmiUiReactiveKeyValue row;
    UmiUiReactiveStoreRead read;
    CHECK(umi_ui_reactive_observable_store_read_changes(store, 0U, keys, 1U, &row, 1U, &read)
          == UMI_STATUS_OK);
    PutNumber(store, "range.low", 7);
    CHECK(read.row_count == 1U && strcmp(row.key, "range.low") == 0 && row.key[20] == '\0');
    CHECK(row.value.kind == UMI_UI_VALUE_STRING && strcmp(row.value.string_value, "visible") == 0);
    const char zero[UMI_UI_VALUE_STRING_CAPACITY - sizeof "visible"] = {0};
    CHECK(memcmp(row.value.string_value + sizeof "visible", zero, sizeof zero) == 0);
    CHECK(row.value.integer_value == 0 && row.value.boolean_value == 0 && row.value.real_value == 0.0);
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *test = argv[1];
    UmiUiReactiveObservableStore *store = malloc(sizeof *store);
    CHECK(store != NULL);
    umi_ui_reactive_observable_store_init(store);
    PutNumber(store, "range.low", 1);
    PutNumber(store, "range.high", 2);
    PutNumber(store, "private.setting", 999);
    const uint64_t baseline = store->revision;
    const char *keys[] = {"range.high", "range.low"};
    size_t selected = 2U, capacity = 3U;
    UmiUiReactiveKeyValue rows[3], unchanged[3];
    memset(rows, 0x51, sizeof rows);
    memcpy(unchanged, rows, sizeof rows);
    UmiUiReactiveStoreRead read = {.observed_revision = 91U, .row_count = 92U};
    UmiStatus expected = UMI_STATUS_OK;
    uint64_t since = baseline;
    size_t count = 2U;

    if (strcmp(test, "initial") == 0) {
        since = 0U;
    } else if (strcmp(test, "committed") == 0) {
        ApplyRange(store);
    } else if (strcmp(test, "repeated") == 0) {
        PutNumber(store, "range.low", 4);
        PutNumber(store, "range.low", 7);
        count = 1U;
    } else if (strcmp(test, "unchanged") == 0) {
        PutNumber(store, "private.setting", 555);
        capacity = 0U;
        count = 0U;
    } else if (strcmp(test, "unknown") == 0) {
        since = 0U;
        keys[1] = "range.missing";
        expected = UMI_STATUS_NOT_FOUND;
    } else if (strcmp(test, "duplicate") == 0) {
        since = 0U;
        keys[1] = keys[0];
        expected = UMI_STATUS_ALREADY_EXISTS;
    } else if (strcmp(test, "future") == 0) {
        since = baseline + 1U;
        expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(test, "small") == 0) {
        since = 0U;
        capacity = 1U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(test, "corrupt") == 0) {
        store->items[0].revision = 0U;
        CHECK(umi_ui_reactive_observable_store_read_changes(store, since, keys, selected, rows, capacity, &read)
              == UMI_STATUS_INVALID_STATE);
        store->items[0].revision = 1U;
        strcpy(store->items[2].key, "range.high");
        expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(test, "invalid-key") == 0) {
        keys[1] = "";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(test, "ownership") == 0) {
        CheckOwnership(store);
        free(store);
        return EXIT_SUCCESS;
    } else if (strcmp(test, "empty") == 0) {
        selected = 0U;
        capacity = 0U;
        since = 0U;
        count = 0U;
    } else {
        CHECK(strcmp(test, "exhausted") == 0);
        store->revision = UINT64_MAX - 1U;
        since = store->revision;
        PutNumber(store, "range.low", 7);
        count = 1U;
    }

    CHECK(umi_ui_reactive_observable_store_read_changes(store, since,
        selected == 0U ? NULL : keys, selected, capacity == 0U ? NULL : rows, capacity, &read) == expected);
    if (expected != UMI_STATUS_OK) {
        CHECK(read.observed_revision == 91U && read.row_count == 92U);
        CHECK(memcmp(rows, unchanged, sizeof rows) == 0);
    } else {
        CHECK(read.observed_revision == store->revision && read.row_count == count);
        if (count == 2U && since == 0U) {
            CheckNumber(&rows[0], "range.high", 2, 2U);
            CheckNumber(&rows[1], "range.low", 1, 1U);
        } else if (count == 2U) {
            CheckNumber(&rows[0], "range.high", 8, read.observed_revision);
            CheckNumber(&rows[1], "range.low", 4, read.observed_revision);
        } else if (count == 1U) {
            CheckNumber(&rows[0], "range.low", 7, read.observed_revision);
        }
        CHECK(memcmp(rows + count, unchanged + count, (3U - count) * sizeof(*rows)) == 0);
    }
    free(store);
    return EXIT_SUCCESS;
}
