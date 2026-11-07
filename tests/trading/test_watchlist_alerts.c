/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_watchlist_alerts.c
 *
 * PURPOSE:
 *   Validate watchlist alerts behaviour in the trading foundation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This focused regression test uses deterministic values so changes to the
 * trading contract are visible immediately.
 */

/* Required fixture setup and checks also execute in optimized builds. */
#include "../support/assertions.h"
#include <assert.h>
#include "test_trading_common.h"
/* Verify that watchlist identity and price-alert lifecycle remain deterministic. */
/* Exercise full and damaged list boundaries without relying on a live broker.
 * Static storage avoids making this fixture's array size part of thread stack
 * requirements. Failed additions must not alter any bytes, including count. */
static void check_watchlist_boundaries(void)
{
    static UmiWatchlist watchlist;
    static UmiWatchlist before;
    UmiInstrument candidate = test_instrument();
    umi_watchlist_init(&watchlist);
    for (size_t index = 0U; index < UMI_TRADING_MAX_WATCHLIST; ++index) {
        int written = snprintf(candidate.instrument_id.value,
                               sizeof(candidate.instrument_id.value),
                               "WATCH.%zu", index);
        assert(written > 0 &&
               (size_t)written < sizeof(candidate.instrument_id.value));
        assert(umi_watchlist_add(&watchlist, &candidate) == UMI_STATUS_OK);
        assert(watchlist.count == index + 1U);
    }
    memcpy(&before, &watchlist, sizeof(before));
    assert(umi_watchlist_add(&watchlist, &candidate) == UMI_STATUS_ALREADY_EXISTS);
    assert(memcmp(&before, &watchlist, sizeof(before)) == 0);
    candidate = test_instrument();
    assert(umi_watchlist_add(&watchlist, &candidate) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(memcmp(&before, &watchlist, sizeof(before)) == 0);
    const size_t damaged_counts[] = {UMI_TRADING_MAX_WATCHLIST + 1U, SIZE_MAX};
    for (size_t index = 0U; index < sizeof(damaged_counts) / sizeof(damaged_counts[0]); ++index) {
        watchlist.count = damaged_counts[index];
        memcpy(&before, &watchlist, sizeof(before));
        assert(umi_watchlist_add(&watchlist, &candidate) == UMI_STATUS_INVALID_ARGUMENT);
        assert(memcmp(&before, &watchlist, sizeof(before)) == 0);
    }
    umi_watchlist_init(&watchlist);
    assert(umi_watchlist_add(&watchlist, &candidate) == UMI_STATUS_OK);
    assert(watchlist.count == 1U);
    assert(umi_watchlist_add(NULL, &candidate) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_watchlist_add(&watchlist, NULL) == UMI_STATUS_INVALID_ARGUMENT);
}

int main(void)
{
    check_watchlist_boundaries();
    UmiWatchlist watchlist;
    UmiInstrument instrument = test_instrument();
    UmiTradingAlertBook book;
    UmiTradingPriceAlert rule;
    UmiTradingPriceAlert stored;

    umi_watchlist_init(&watchlist);
    assert(umi_watchlist_add(&watchlist, &instrument) == UMI_STATUS_OK);
    assert(umi_watchlist_add(&watchlist, &instrument) ==
           UMI_STATUS_ALREADY_EXISTS);
    assert(umi_trading_alert_crossed(99.0, 101.0, 100.0));
    assert(!umi_trading_alert_crossed(101.0, 102.0, 100.0));

    umi_trading_alert_book_init(&book);
    assert(umi_trading_price_alert_init(
               &rule,
               "alert-1",
               instrument.instrument_id.value,
               UMI_TRADING_PRICE_ALERT_CROSSES_ABOVE,
               100.0,
               1000) == UMI_STATUS_OK);
    assert(umi_trading_price_alert_seed(&rule, 99.0) == UMI_STATUS_OK);
    assert(umi_trading_alert_book_add(&book, &rule) == UMI_STATUS_OK);
    assert(umi_trading_alert_book_add(&book, &rule) ==
           UMI_STATUS_ALREADY_EXISTS);

    assert(umi_trading_alert_book_evaluate(
               &book,
               instrument.instrument_id.value,
               101.0,
               1100) == UMI_STATUS_OK);
    assert(umi_trading_alert_book_at(&book, 0U, &stored) == UMI_STATUS_OK);
    assert(stored.active && !stored.acknowledged);
    assert(stored.occurrences == 1U && stored.triggered_at_ms == 1100);
    assert(umi_trading_alert_book_active_count(&book) == 1U);
    assert(umi_trading_alert_book_unacknowledged_count(&book) == 1U);

    assert(umi_trading_alert_book_acknowledge(&book, "alert-1") ==
           UMI_STATUS_OK);
    /* An older observation is ignored and cannot rewind the crossing baseline. */
    assert(umi_trading_alert_book_evaluate(
               &book,
               instrument.instrument_id.value,
               99.0,
               1000) == UMI_STATUS_OK);
    assert(umi_trading_alert_book_at(&book, 0U, &stored) == UMI_STATUS_OK);
    assert(stored.last_value == 101.0);
    assert(umi_trading_alert_book_set_enabled(&book, "alert-1", 0) ==
           UMI_STATUS_OK);
    /* Paused alerts follow the price but do not create occurrences. */
    assert(umi_trading_alert_book_evaluate(
               &book,
               instrument.instrument_id.value,
               99.0,
               1200) == UMI_STATUS_OK);
    assert(umi_trading_alert_book_evaluate(
               &book,
               instrument.instrument_id.value,
               101.0,
               1300) == UMI_STATUS_OK);
    assert(umi_trading_alert_book_at(&book, 0U, &stored) == UMI_STATUS_OK);
    assert(stored.occurrences == 1U && !stored.active);

    assert(umi_trading_alert_book_set_enabled(&book, "alert-1", 1) ==
           UMI_STATUS_OK);
    assert(umi_trading_alert_book_remove(&book, "alert-1") == UMI_STATUS_OK);
    assert(umi_trading_alert_book_count(&book) == 0U);
    return 0;
}
