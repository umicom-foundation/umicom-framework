/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/trade_tape.c
 *
 * PURPOSE:
 *   Implement safe public trade retention, sequence evidence, display filters,
 *   and non-destructive pause behaviour for Time and Sales surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/trade_tape.h"

#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/trading/instrument.h"
#include "umicom/trading/tick.h"

struct UmiTradingTradeTape {
    UmiTradingTradeTapeRecord records[UMI_TRADING_TRADE_TAPE_CAPACITY];
    size_t count;
    uint64_t last_sequence;
    uint64_t missing_sequence_count;
    uint64_t dropped_trade_count;
    uint64_t paused_at_sequence;
    uint64_t revision;
    double minimum_size;
    UmiTradingTradeTapeFilter filter;
    int provider_ready;
    int paused;
};

/* Advance observable state without allowing an unsigned wrap to look like an
 * unchanged zero revision to caches and user interfaces. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Accept only declared direction values so corrupted provider data cannot be
 * interpreted as a different colour or side by a presentation adapter. */
static int direction_valid(UmiTradingTradeDirection direction)
{
    return direction >= UMI_TRADING_TRADE_DIRECTION_UNKNOWN &&
           direction <= UMI_TRADING_TRADE_DIRECTION_SELLER_INITIATED;
}

/* Accept only declared filters so future enum values require an intentional
 * implementation rather than silently behaving like the show-all filter. */
static int filter_valid(UmiTradingTradeTapeFilter filter)
{
    return filter >= UMI_TRADING_TRADE_TAPE_ALL &&
           filter <= UMI_TRADING_TRADE_TAPE_UNKNOWN;
}

/* Verify that caller-provided bounded text has a terminator before any string
 * function reads it. This protects adapters that construct records manually. */
static int condition_terminated(const char *condition)
{
    return condition != NULL &&
           memchr(condition, '\0', UMI_TRADING_TRADE_CONDITION_CAPACITY) != NULL;
}

/* Validate the complete public record at the boundary shared by provider,
 * replay, test, and desktop clients. */
static int record_valid(const UmiTradingTradeTapeRecord *record)
{
    return record != NULL &&
           record->structure_size == (uint32_t)sizeof(*record) &&
           record->api_version == UMI_TRADING_TRADE_TAPE_API_VERSION &&
           record->sequence > 0U &&
           umi_trade_tick_valid(&record->trade) &&
           umi_instrument_valid(&record->trade.instrument) &&
           direction_valid(record->direction) &&
           condition_terminated(record->condition);
}

/* Increase a diagnostic counter without allowing unsigned wraparound to hide
 * an already very large data-quality problem. */
static void add_saturated(uint64_t *value, uint64_t amount)
{
    if (value == NULL) return;
    if (UINT64_MAX - *value < amount) {
        *value = UINT64_MAX;
    } else {
        *value += amount;
    }
}

/* Match an optional instrument identity. Empty text deliberately means that a
 * caller wants rows from every instrument in the tape. */
static int instrument_matches(
    const UmiTradingTradeTapeRecord *record,
    const char *instrument_id)
{
    return instrument_id == NULL || instrument_id[0] == '\0' ||
           strcmp(record->trade.instrument.instrument_id.value,
                  instrument_id) == 0;
}

/* Apply the current display policy while keeping all accepted rows retained
 * for later filter changes and resume operations. */
static int record_visible(
    const UmiTradingTradeTape *tape,
    const UmiTradingTradeTapeRecord *record,
    const char *instrument_id)
{
    if (tape == NULL || record == NULL ||
        !instrument_matches(record, instrument_id) ||
        record->trade.size < tape->minimum_size) {
        return 0;
    }
    if (tape->paused && record->sequence > tape->paused_at_sequence) {
        return 0;
    }
    switch (tape->filter) {
    case UMI_TRADING_TRADE_TAPE_ALL:
        return 1;
    case UMI_TRADING_TRADE_TAPE_BUYER_INITIATED:
        return record->direction ==
               UMI_TRADING_TRADE_DIRECTION_BUYER_INITIATED;
    case UMI_TRADING_TRADE_TAPE_SELLER_INITIATED:
        return record->direction ==
               UMI_TRADING_TRADE_DIRECTION_SELLER_INITIATED;
    case UMI_TRADING_TRADE_TAPE_UNKNOWN:
        return record->direction == UMI_TRADING_TRADE_DIRECTION_UNKNOWN;
    default:
        return 0;
    }
}

/* Build one complete value and reject text that cannot fit instead of storing
 * a truncated market condition with a different meaning. */
UmiStatus umi_trading_trade_tape_record_init(
    UmiTradingTradeTapeRecord *record,
    uint64_t sequence,
    const UmiTradeTick *trade,
    UmiTradingTradeDirection direction,
    const char *condition)
{
    size_t condition_length;

    if (record == NULL || trade == NULL || sequence == 0U ||
        !umi_trade_tick_valid(trade) || !umi_instrument_valid(&trade->instrument) ||
        !direction_valid(direction)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (condition == NULL) condition = "";
    condition_length = strlen(condition);
    if (condition_length >= UMI_TRADING_TRADE_CONDITION_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memset(record, 0, sizeof(*record));
    record->structure_size = (uint32_t)sizeof(*record);
    record->api_version = UMI_TRADING_TRADE_TAPE_API_VERSION;
    record->sequence = sequence;
    record->trade = *trade;
    record->direction = direction;
    (void)memcpy(record->condition, condition, condition_length + 1U);
    return UMI_STATUS_OK;
}

/* Allocate the large bounded row store on the heap so GUI callbacks and tests
 * do not place it on their comparatively small stacks. */
UmiStatus umi_trading_trade_tape_create(UmiTradingTradeTape **out_tape)
{
    UmiTradingTradeTape *tape;

    if (out_tape == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_tape = NULL;
    tape = (UmiTradingTradeTape *)calloc(1U, sizeof(*tape));
    if (tape == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    tape->filter = UMI_TRADING_TRADE_TAPE_ALL;
    tape->revision = 1U;
    *out_tape = tape;
    return UMI_STATUS_OK;
}

/* The tape contains values only, so one free releases all retained rows. */
void umi_trading_trade_tape_destroy(UmiTradingTradeTape *tape)
{
    free(tape);
}

/* Provider readiness is separate from retained rows: a disconnected feed may
 * still leave useful historical prints visible with an honest status badge. */
UmiStatus umi_trading_trade_tape_set_provider_ready(
    UmiTradingTradeTape *tape,
    int ready)
{
    int normalized;

    if (tape == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    normalized = ready != 0;
    if (tape->provider_ready != normalized) {
        tape->provider_ready = normalized;
        tape->revision = next_revision(tape->revision);
    }
    return UMI_STATUS_OK;
}

/* Retain accepted trades in ascending provider sequence. Full tapes discard
 * only their oldest row, preserving a deterministic most-recent window. */
UmiStatus umi_trading_trade_tape_append(
    UmiTradingTradeTape *tape,
    const UmiTradingTradeTapeRecord *record)
{
    if (tape == NULL || !record_valid(record)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (tape->last_sequence > 0U && record->sequence <= tape->last_sequence) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (tape->last_sequence > 0U &&
        record->sequence > tape->last_sequence + 1U) {
        add_saturated(&tape->missing_sequence_count,
                      record->sequence - tape->last_sequence - 1U);
    }
    if (tape->count == UMI_TRADING_TRADE_TAPE_CAPACITY) {
        (void)memmove(&tape->records[0],
                      &tape->records[1],
                      (tape->count - 1U) * sizeof(tape->records[0]));
        tape->count -= 1U;
        add_saturated(&tape->dropped_trade_count, 1U);
    }
    tape->records[tape->count++] = *record;
    tape->last_sequence = record->sequence;
    tape->provider_ready = 1;
    tape->revision = next_revision(tape->revision);
    return UMI_STATUS_OK;
}

/* Filters affect presentation only. A later filter change can therefore show
 * retained rows immediately without asking the provider to resend them. */
UmiStatus umi_trading_trade_tape_set_filter(
    UmiTradingTradeTape *tape,
    UmiTradingTradeTapeFilter filter,
    double minimum_size)
{
    if (tape == NULL || !filter_valid(filter) ||
        !isfinite(minimum_size) || minimum_size < 0.0 ||
        minimum_size > DBL_MAX) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (tape->filter != filter || tape->minimum_size != minimum_size) {
        tape->filter = filter;
        tape->minimum_size = minimum_size;
        tape->revision = next_revision(tape->revision);
    }
    return UMI_STATUS_OK;
}

/* Pausing records the latest accepted sequence. New data may continue to be
 * retained, but queries stay frozen until the user resumes the tape. */
UmiStatus umi_trading_trade_tape_set_paused(
    UmiTradingTradeTape *tape,
    int paused)
{
    int normalized;

    if (tape == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    normalized = paused != 0;
    if (tape->paused == normalized) return UMI_STATUS_OK;
    tape->paused = normalized;
    tape->paused_at_sequence = normalized ? tape->last_sequence : 0U;
    tape->revision = next_revision(tape->revision);
    return UMI_STATUS_OK;
}

/* Compact retained values in place and clear the inactive tail so removed
 * instrument data cannot reappear if storage is inspected or later reused. */
UmiStatus umi_trading_trade_tape_remove_instrument(
    UmiTradingTradeTape *tape,
    const char *instrument_id)
{
    size_t read_index;
    size_t write_index = 0U;

    if (tape == NULL || instrument_id == NULL || instrument_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (read_index = 0U; read_index < tape->count; ++read_index) {
        if (strcmp(tape->records[read_index].trade.instrument.instrument_id.value,
                   instrument_id) != 0) {
            if (write_index != read_index) {
                tape->records[write_index] = tape->records[read_index];
            }
            write_index += 1U;
        }
    }
    if (write_index != tape->count) {
        (void)memset(&tape->records[write_index],
                     0,
                     (tape->count - write_index) * sizeof(tape->records[0]));
        tape->count = write_index;
        tape->revision = next_revision(tape->revision);
    }
    return UMI_STATUS_OK;
}

/* Count rows using the exact same predicate as indexed retrieval so adapters
 * can allocate or limit their visible controls without count/data mismatch. */
size_t umi_trading_trade_tape_visible_count(
    const UmiTradingTradeTape *tape,
    const char *instrument_id)
{
    size_t index;
    size_t count = 0U;

    if (tape == NULL) return 0U;
    for (index = 0U; index < tape->count; ++index) {
        if (record_visible(tape, &tape->records[index], instrument_id)) {
            count += 1U;
        }
    }
    return count;
}

/* Walk from newest to oldest while counting only visible records. This keeps
 * row zero stable as the most recent trade in every frontend. */
UmiStatus umi_trading_trade_tape_visible_at(
    const UmiTradingTradeTape *tape,
    const char *instrument_id,
    size_t newest_first_index,
    UmiTradingTradeTapeRecord *out_record)
{
    size_t index;
    size_t visible_index = 0U;

    if (tape == NULL || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = tape->count; index > 0U; --index) {
        const UmiTradingTradeTapeRecord *record = &tape->records[index - 1U];

        if (!record_visible(tape, record, instrument_id)) continue;
        if (visible_index == newest_first_index) {
            *out_record = *record;
            return UMI_STATUS_OK;
        }
        visible_index += 1U;
    }
    return UMI_STATUS_NOT_FOUND;
}

/* Produce an independent value snapshot so UI code never borrows the tape's
 * mutable array while provider updates are being coordinated. */
UmiStatus umi_trading_trade_tape_snapshot(
    const UmiTradingTradeTape *tape,
    UmiTradingTradeTapeSnapshot *out_snapshot)
{
    if (tape == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_TRADING_TRADE_TAPE_API_VERSION;
    out_snapshot->retained_count = tape->count;
    out_snapshot->visible_count =
        umi_trading_trade_tape_visible_count(tape, NULL);
    out_snapshot->last_sequence = tape->last_sequence;
    out_snapshot->missing_sequence_count = tape->missing_sequence_count;
    out_snapshot->dropped_trade_count = tape->dropped_trade_count;
    out_snapshot->paused_at_sequence = tape->paused_at_sequence;
    out_snapshot->revision = tape->revision;
    out_snapshot->minimum_size = tape->minimum_size;
    out_snapshot->filter = tape->filter;
    out_snapshot->provider_ready = tape->provider_ready;
    out_snapshot->paused = tape->paused;
    return UMI_STATUS_OK;
}

/* Keep readable presentation text central so each frontend uses the same
 * meaning without duplicating enum conversion branches. */
const char *umi_trading_trade_direction_text(
    UmiTradingTradeDirection direction)
{
    switch (direction) {
    case UMI_TRADING_TRADE_DIRECTION_BUYER_INITIATED:
        return "Buyer initiated";
    case UMI_TRADING_TRADE_DIRECTION_SELLER_INITIATED:
        return "Seller initiated";
    case UMI_TRADING_TRADE_DIRECTION_UNKNOWN:
        return "Direction unknown";
    default:
        return "Invalid direction";
    }
}

/* Convert the active filter to a short label suitable for panels, logs, and
 * automation output without exposing numeric enum values. */
const char *umi_trading_trade_tape_filter_text(
    UmiTradingTradeTapeFilter filter)
{
    switch (filter) {
    case UMI_TRADING_TRADE_TAPE_ALL:
        return "All trades";
    case UMI_TRADING_TRADE_TAPE_BUYER_INITIATED:
        return "Buyer initiated";
    case UMI_TRADING_TRADE_TAPE_SELLER_INITIATED:
        return "Seller initiated";
    case UMI_TRADING_TRADE_TAPE_UNKNOWN:
        return "Direction unknown";
    default:
        return "Invalid filter";
    }
}
