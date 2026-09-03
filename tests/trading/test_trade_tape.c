/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/trading/test_trade_tape.c
 *
 * PURPOSE:
 *   Verify bounded public-trade retention, sequence diagnostics, filtering,
 *   pause behaviour, and instrument removal for Time and Sales clients.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "test_trading_common.h"

/* Build a complete record through the public constructor so each test row
 * follows the same validation path used by provider adapters and replay. */
static UmiTradingTradeTapeRecord make_record(
    const UmiInstrument *instrument,
    uint64_t sequence,
    double price,
    double size,
    UmiTradingTradeDirection direction)
{
    UmiTradeTick trade = {0};
    UmiTradingTradeTapeRecord record;

    trade.instrument = *instrument;
    trade.price = price;
    trade.size = size;
    trade.event_time_ms = (int64_t)sequence * 1000;
    assert(umi_trading_trade_tape_record_init(
               &record, sequence, &trade, direction, "regular") ==
           UMI_STATUS_OK);
    return record;
}

/* Exercise the tape as a client would: accept ordered public data, change
 * display policy, pause without losing new rows, and finally remove a symbol. */
int main(void)
{
    UmiTradingTradeTape *tape = NULL;
    UmiInstrument instrument = test_instrument();
    UmiTradingTradeTapeRecord record;
    UmiTradingTradeTapeRecord visible;
    UmiTradingTradeTapeSnapshot snapshot;
    size_t index;

    assert(umi_trading_trade_tape_create(&tape) == UMI_STATUS_OK);
    assert(umi_trading_trade_tape_snapshot(tape, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.retained_count == 0U);
    assert(!snapshot.provider_ready);

    record = make_record(
        &instrument, 1U, 25000.0, 20.0,
        UMI_TRADING_TRADE_DIRECTION_BUYER_INITIATED);
    assert(umi_trading_trade_tape_append(tape, &record) == UMI_STATUS_OK);
    /* Duplicate or stale provider sequences are rejected so the visible tape
     * cannot silently show the same market trade twice. */
    assert(umi_trading_trade_tape_append(tape, &record) ==
           UMI_STATUS_INVALID_STATE);
    record = make_record(
        &instrument, 3U, 24999.5, 5.0,
        UMI_TRADING_TRADE_DIRECTION_SELLER_INITIATED);
    assert(umi_trading_trade_tape_append(tape, &record) == UMI_STATUS_OK);
    assert(umi_trading_trade_tape_snapshot(tape, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.provider_ready);
    assert(snapshot.missing_sequence_count == 1U);
    assert(snapshot.visible_count == 2U);

    assert(umi_trading_trade_tape_set_filter(
               tape, UMI_TRADING_TRADE_TAPE_BUYER_INITIATED, 10.0) ==
           UMI_STATUS_OK);
    assert(umi_trading_trade_tape_visible_count(
               tape, instrument.instrument_id.value) == 1U);
    assert(umi_trading_trade_tape_visible_at(
               tape, instrument.instrument_id.value, 0U, &visible) ==
           UMI_STATUS_OK);
    assert(visible.sequence == 1U);

    assert(umi_trading_trade_tape_set_filter(
               tape, UMI_TRADING_TRADE_TAPE_ALL, 0.0) == UMI_STATUS_OK);
    assert(umi_trading_trade_tape_set_paused(tape, 1) == UMI_STATUS_OK);
    record = make_record(
        &instrument, 4U, 25001.0, 12.0,
        UMI_TRADING_TRADE_DIRECTION_BUYER_INITIATED);
    assert(umi_trading_trade_tape_append(tape, &record) == UMI_STATUS_OK);
    assert(umi_trading_trade_tape_visible_count(
               tape, instrument.instrument_id.value) == 2U);
    assert(umi_trading_trade_tape_set_paused(tape, 0) == UMI_STATUS_OK);
    assert(umi_trading_trade_tape_visible_count(
               tape, instrument.instrument_id.value) == 3U);
    assert(umi_trading_trade_tape_visible_at(
               tape, instrument.instrument_id.value, 0U, &visible) ==
           UMI_STATUS_OK);
    assert(visible.sequence == 4U);

    /* Fill beyond capacity to prove the oldest row is dropped deliberately
     * rather than writing outside the fixed allocation. */
    for (index = 5U;
         index <= UMI_TRADING_TRADE_TAPE_CAPACITY + 4U;
         ++index) {
        record = make_record(
            &instrument, (uint64_t)index, 25000.0 + (double)index, 1.0,
            UMI_TRADING_TRADE_DIRECTION_UNKNOWN);
        assert(umi_trading_trade_tape_append(tape, &record) == UMI_STATUS_OK);
    }
    assert(umi_trading_trade_tape_snapshot(tape, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.retained_count == UMI_TRADING_TRADE_TAPE_CAPACITY);
    assert(snapshot.dropped_trade_count == 3U);
    assert(umi_trading_trade_tape_remove_instrument(
               tape, instrument.instrument_id.value) == UMI_STATUS_OK);
    assert(umi_trading_trade_tape_visible_count(tape, NULL) == 0U);

    /* Oversized condition text is rejected instead of being truncated into a
     * different market meaning. */
    {
        UmiTradeTick trade = {0};
        char condition[UMI_TRADING_TRADE_CONDITION_CAPACITY + 1U];

        trade.instrument = instrument;
        trade.price = 1.0;
        trade.size = 1.0;
        (void)memset(condition, 'x', sizeof(condition) - 1U);
        condition[sizeof(condition) - 1U] = '\0';
        assert(umi_trading_trade_tape_record_init(
                   &record, 1U, &trade,
                   UMI_TRADING_TRADE_DIRECTION_UNKNOWN, condition) ==
               UMI_STATUS_CAPACITY_EXCEEDED);
    }

    umi_trading_trade_tape_destroy(tape);
    return 0;
}
