/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_watchlist_admission.c
 *
 * PURPOSE:
 *   Verify complete watchlist admission, bounded queries and unchanged refusal outputs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include <stdio.h>
#include <string.h>
#include "test_trading_common.h"

#define CHECK(expression) do { \
    if (!(expression)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); \
        return 1; \
    } \
} while (0)

static UmiWatchlist list;
static UmiWatchlist before;

static UmiInstrument named(size_t number)
{
    UmiInstrument instrument = test_instrument();
    (void)snprintf(instrument.instrument_id.value, sizeof(instrument.instrument_id.value),
                   "WATCH.%zu", number);
    return instrument;
}

static int complete_admission(void)
{
    UmiInstrument first = named(0), second = named(1);
    memset(&list, 0xa5, sizeof(list));
    umi_watchlist_init(&list);
    CHECK(umi_watchlist_validate(&list) == UMI_STATUS_OK);
    CHECK(umi_watchlist_add(&list, &first) == UMI_STATUS_OK);
    CHECK(umi_watchlist_add(&list, &second) == UMI_STATUS_OK);
    /* An early valid match must not hide a damaged later active row. */
    memset(list.instruments[1].symbol, 'x', sizeof(list.instruments[1].symbol));
    memcpy(&before, &list, sizeof(before));
    size_t index = 123U;
    CHECK(umi_watchlist_find(&list, first.instrument_id.value, &index) == UMI_STATUS_INVALID_STATE);
    CHECK(index == 123U);
    CHECK(umi_watchlist_add(&list, &first) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&before, &list, sizeof(before)) == 0);
    list.instruments[1] = first;
    CHECK(umi_watchlist_validate(&list) == UMI_STATUS_INVALID_STATE);
    CHECK(umi_watchlist_find(&list, first.instrument_id.value, &index) == UMI_STATUS_INVALID_STATE);
    CHECK(index == 123U);
    list.instruments[1] = second;
    CHECK(umi_watchlist_validate(&list) == UMI_STATUS_OK);
    CHECK(umi_watchlist_find(&list, second.instrument_id.value, &index) == UMI_STATUS_OK);
    CHECK(index == 1U);
    /* Ignore inactive bytes, including stale or malformed unused records. */
    memset(&list.instruments[2], 0xff, sizeof(list.instruments[2]));
    CHECK(umi_watchlist_validate(&list) == UMI_STATUS_OK);
    return 0;
}

static int bounded_query_and_output(void)
{
    UmiInstrument instrument = named(0);
    umi_watchlist_init(&list);
    CHECK(umi_watchlist_add(&list, &instrument) == UMI_STATUS_OK);
    char unterminated[UMI_FINANCE_ID_CAPACITY];
    memset(unterminated, 'x', sizeof(unterminated));
    size_t index = 123U;
    CHECK(umi_watchlist_find(&list, unterminated, &index) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(index == 123U);
    CHECK(umi_watchlist_find(&list, "", &index) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_watchlist_find(&list, NULL, &index) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_watchlist_find(NULL, "WATCH.0", &index) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_watchlist_find(&list, "missing", &index) == UMI_STATUS_NOT_FOUND);
    CHECK(index == 123U);
    CHECK(umi_watchlist_find(&list, "WATCH.0", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    memcpy(&before, &list, sizeof(before));
    CHECK(umi_watchlist_find(&list, "WATCH.0", &list.count) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&before, &list, sizeof(before)) == 0);
    union { char query[UMI_FINANCE_ID_CAPACITY]; size_t index; } shared;
    (void)snprintf(shared.query, sizeof(shared.query), "%s", "WATCH.0");
    CHECK(umi_watchlist_find(&list, shared.query, &shared.index) == UMI_STATUS_OK);
    CHECK(shared.index == 0U);
    /* The longest admitted identity must still match without truncation. */
    memset(instrument.instrument_id.value, 'z', sizeof(instrument.instrument_id.value) - 1U);
    instrument.instrument_id.value[sizeof(instrument.instrument_id.value) - 1U] = '\0';
    CHECK(umi_watchlist_add(&list, &instrument) == UMI_STATUS_OK);
    CHECK(umi_watchlist_find(&list, instrument.instrument_id.value, &index) == UMI_STATUS_OK);
    CHECK(index == 1U);
    return 0;
}

static int capacity_and_borrowed_inputs(void)
{
    umi_watchlist_init(&list);
    list.instruments[0] = named(0);
    /* Borrowing the currently inactive destination is permitted. */
    CHECK(umi_watchlist_add(&list, &list.instruments[0]) == UMI_STATUS_OK);
    for (size_t i = 1U; i < UMI_TRADING_MAX_WATCHLIST; ++i) {
        UmiInstrument instrument = named(i);
        CHECK(umi_watchlist_add(&list, &instrument) == UMI_STATUS_OK);
    }
    memcpy(&before, &list, sizeof(before));
    UmiInstrument candidate = named(0);
    CHECK(umi_watchlist_add(&list, &candidate) == UMI_STATUS_ALREADY_EXISTS);
    candidate = named(UMI_TRADING_MAX_WATCHLIST);
    CHECK(umi_watchlist_add(&list, &candidate) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&before, &list, sizeof(before)) == 0);
    const size_t counts[] = { UMI_TRADING_MAX_WATCHLIST + 1U, SIZE_MAX };
    for (size_t i = 0U; i < sizeof(counts) / sizeof(counts[0]); ++i) {
        list.count = counts[i];
        memcpy(&before, &list, sizeof(before));
        size_t index = 123U;
        CHECK(umi_watchlist_validate(&list) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_watchlist_find(&list, "WATCH.0", &index) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(index == 123U);
        CHECK(umi_watchlist_add(&list, &candidate) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&before, &list, sizeof(before)) == 0);
    }
    CHECK(umi_watchlist_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}

int main(void)
{
    CHECK(complete_admission() == 0);
    CHECK(bounded_query_and_output() == 0);
    CHECK(capacity_and_borrowed_inputs() == 0);
    return 0;
}
