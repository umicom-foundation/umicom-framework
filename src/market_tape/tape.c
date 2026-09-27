/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/market_tape/tape.c
 *
 * PURPOSE:
 *   Retain sequenced canonical market observations and coherent read snapshots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/market_tape.h"
#include "umicom/trading/quote.h"
#include "umicom/trading/market_data_quality.h"
#include "umicom/platform/threading.h"
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

typedef struct TapeSlot {
    UmiMarketTapeRow row;
    UmiTradeTick trades[UMI_MARKET_TAPE_TRADE_LIMIT];
    size_t tradeHead, tradeCount;
    UmiBar bars[UMI_MARKET_TAPE_BAR_LIMIT];
    size_t barHead, barCount;
    UmiMarketDepth depth;
} TapeSlot;
struct UmiMarketTape {
    UmiMutex *mutex;
    UmiMarketTapeConfig config;
    uint64_t revision;
    size_t count, selected;
    TapeSlot slots[UMI_MARKET_TAPE_INSTRUMENT_LIMIT];
};

/* Bounded strings are checked before strcmp; a malformed fixed array must
 * never turn model validation into an unbounded read or a terminal escape. */
static bool Token(const char *text, size_t capacity)
{
    size_t i;
    if (text == NULL || capacity == 0U || text[0] == '\0') return false;
    for (i = 0U; i < capacity; ++i) {
        unsigned char c = (unsigned char)text[i];
        if (c == 0U) return true;
        if (c < 33U || c > 126U) return false;
    }
    return false;
}
static bool InstrumentValid(const UmiInstrument *value)
{
    if (value == NULL || !Token(value->instrument_id.value, sizeof(value->instrument_id.value)) ||
        !Token(value->symbol, sizeof(value->symbol)) ||
        !Token(value->venue, sizeof(value->venue)) ||
        !isfinite(value->multiplier) || value->multiplier <= 0.0 || value->expiry_yyyymmdd < 0)
        return false;
    for (size_t i = 0U; i < 3U; ++i)
        if (value->currency.code[i] < 'A' || value->currency.code[i] > 'Z') return false;
    return value->currency.code[3] == '\0';
}
static bool SameInstrument(const UmiInstrument *a, const UmiInstrument *b)
{
    return strcmp(a->instrument_id.value, b->instrument_id.value) == 0 &&
        strcmp(a->symbol, b->symbol) == 0 && strcmp(a->venue, b->venue) == 0 &&
        memcmp(a->currency.code, b->currency.code, sizeof(a->currency.code)) == 0 &&
        a->multiplier == b->multiplier && a->expiry_yyyymmdd == b->expiry_yyyymmdd;
}
static size_t Find(const UmiMarketTape *tape, const UmiFinancialId *id)
{
    for (size_t i = 0U; i < tape->count; ++i)
        if (strcmp(id->value, tape->slots[i].row.instrument.instrument_id.value) == 0) return i;
    return tape->count;
}
static bool IdValid(const UmiFinancialId *id)
{
    return id != NULL && Token(id->value, sizeof(id->value));
}
static void IncrementRetired(uint64_t *count)
{
    if (*count != UINT64_MAX) ++*count;
}
UmiMarketTapeConfig UmiMarketTapeConfigDefault(void)
{
    return (UmiMarketTapeConfig){1000, 2000};
}
UmiStatus UmiMarketTapeCreate(const UmiMarketTapeConfig *config, UmiMarketTape **outTape)
{
    UmiMarketTape *tape;
    UmiStatus status;
    if (outTape == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outTape = NULL;
    if (config == NULL || config->barIntervalMs < 1 || config->barIntervalMs > 86400000 ||
        config->staleAfterMs < 1) return UMI_STATUS_INVALID_ARGUMENT;
    tape = calloc(1U, sizeof(*tape));
    if (tape == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&tape->mutex);
    if (status != UMI_STATUS_OK) { free(tape); return status; }
    tape->config = *config;
    *outTape = tape;
    return UMI_STATUS_OK;
}
void UmiMarketTapeDestroy(UmiMarketTape *tape)
{
    if (tape == NULL) return;
    /* Caller has quiesced clients; a mutex cannot repair use after destruction. */
    umi_mutex_destroy(tape->mutex);
    free(tape);
}
UmiStatus UmiMarketTapeRegister(UmiMarketTape *tape, const UmiInstrument *instrument)
{
    UmiStatus status;
    if (tape == NULL || !InstrumentValid(instrument)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(tape->mutex);
    if (status != UMI_STATUS_OK) return status;
    if (Find(tape, &instrument->instrument_id) != tape->count) status = UMI_STATUS_ALREADY_EXISTS;
    else if (tape->count == UMI_MARKET_TAPE_INSTRUMENT_LIMIT || tape->revision == UINT64_MAX)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    else {
        tape->slots[tape->count].row.instrument = *instrument;
        ++tape->count;
        ++tape->revision;
    }
    (void)umi_mutex_unlock(tape->mutex);
    return status;
}
UmiStatus UmiMarketTapeBegin(UmiMarketTape *tape, const UmiFinancialId *id,
    uint64_t generation, uint64_t firstSequence)
{
    UmiStatus status;
    if (tape == NULL || !IdValid(id) || generation == 0U || firstSequence == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(tape->mutex);
    if (status != UMI_STATUS_OK) return status;
    size_t index = Find(tape, id);
    if (index == tape->count) status = UMI_STATUS_NOT_FOUND;
    else if (generation <= tape->slots[index].row.generation) status = UMI_STATUS_INVALID_STATE;
    else if (tape->revision == UINT64_MAX) status = UMI_STATUS_CAPACITY_EXCEEDED;
    else {
        TapeSlot *slot = &tape->slots[index];
        UmiInstrument instrument = slot->row.instrument;
        memset(slot, 0, sizeof(*slot));
        slot->row.instrument = instrument;
        slot->row.generation = generation;
        slot->row.expectedSequence = firstSequence;
        slot->row.connected = true;
        ++tape->revision;
    }
    (void)umi_mutex_unlock(tape->mutex);
    return status;
}
UmiStatus UmiMarketTapeDisconnect(UmiMarketTape *tape, const UmiFinancialId *id)
{
    UmiStatus status;
    if (tape == NULL || !IdValid(id)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(tape->mutex);
    if (status != UMI_STATUS_OK) return status;
    size_t index = Find(tape, id);
    if (index == tape->count) status = UMI_STATUS_NOT_FOUND;
    else if (tape->slots[index].row.connected) {
        if (tape->revision == UINT64_MAX) status = UMI_STATUS_CAPACITY_EXCEEDED;
        else { tape->slots[index].row.connected = false; ++tape->revision; }
    }
    (void)umi_mutex_unlock(tape->mutex);
    return status;
}
UmiStatus UmiMarketTapeSelect(UmiMarketTape *tape, const UmiFinancialId *id)
{
    UmiStatus status;
    if (tape == NULL || !IdValid(id)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(tape->mutex);
    if (status != UMI_STATUS_OK) return status;
    size_t index = Find(tape, id);
    if (index == tape->count) status = UMI_STATUS_NOT_FOUND;
    else if (index != tape->selected) {
        if (tape->revision == UINT64_MAX) status = UMI_STATUS_CAPACITY_EXCEEDED;
        else { tape->selected = index; ++tape->revision; }
    }
    (void)umi_mutex_unlock(tape->mutex);
    return status;
}
static bool DepthValid(const UmiMarketDepth *depth)
{
    if (depth->bid_count > UMI_TRADING_MAX_DEPTH || depth->ask_count > UMI_TRADING_MAX_DEPTH)
        return false;
    for (size_t side = 0U; side < 2U; ++side) {
        const UmiDepthLevel *levels = side == 0U ? depth->bids : depth->asks;
        size_t count = side == 0U ? depth->bid_count : depth->ask_count;
        for (size_t i = 0U; i < count; ++i) {
            if (!isfinite(levels[i].price) || !isfinite(levels[i].size) ||
                levels[i].price <= 0.0 || levels[i].size <= 0.0) return false;
            if (i > 0U && (side == 0U ? levels[i].price >= levels[i-1U].price :
                levels[i].price <= levels[i-1U].price)) return false;
        }
    }
    return depth->bid_count == 0U || depth->ask_count == 0U ||
        depth->bids[0].price <= depth->asks[0].price;
}
static const UmiInstrument *PacketInstrument(const UmiMarketTapePacket *packet, int64_t *time)
{
    switch (packet->kind) {
        case UMI_MARKET_TAPE_QUOTE:
            *time = packet->value.quote.event_time_ms; return &packet->value.quote.instrument;
        case UMI_MARKET_TAPE_TRADE:
            *time = packet->value.trade.event_time_ms; return &packet->value.trade.instrument;
        case UMI_MARKET_TAPE_DEPTH:
            *time = packet->value.depth.event_time_ms; return &packet->value.depth.instrument;
        default: return NULL;
    }
}
static UmiStatus Fail(UmiMarketTapeRejection *reason, UmiMarketTapeRejection value, UmiStatus status)
{
    if (reason != NULL) *reason = value;
    return status;
}
/* Compute the candidate bar BEFORE changing a sequence, tick ring or counter.
 * Rejected overflow and timestamp input cannot leave a half-applied trade. */
static UmiStatus PrepareBar(const UmiMarketTape *tape, const TapeSlot *slot,
    const UmiTradeTick *tick, UmiBar *bar, bool *append)
{
    int64_t start = tick->event_time_ms - tick->event_time_ms % tape->config.barIntervalMs;
    if (start > INT64_MAX - tape->config.barIntervalMs) return UMI_STATUS_CAPACITY_EXCEEDED;
    *append = true;
    if (slot->barCount != 0U) {
        *bar = slot->bars[(slot->barHead + slot->barCount - 1U) % UMI_MARKET_TAPE_BAR_LIMIT];
        if (bar->start_time_ms == start) {
            if (tick->size > DBL_MAX - bar->volume) return UMI_STATUS_CAPACITY_EXCEEDED;
            bar->high = fmax(bar->high, tick->price);
            bar->low = fmin(bar->low, tick->price);
            bar->close = tick->price;
            bar->volume += tick->size;
            *append = false;
            return UMI_STATUS_OK;
        }
    }
    *bar = (UmiBar){0};
    bar->instrument = tick->instrument;
    bar->open = tick->price; bar->high = tick->price;
    bar->low = tick->price; bar->close = tick->price;
    bar->volume = tick->size;
    bar->start_time_ms = start; bar->end_time_ms = start + tape->config.barIntervalMs;
    return UMI_STATUS_OK;
}
static UmiStatus ApplyLocked(UmiMarketTape *tape, const UmiMarketTapePacket *packet,
    const UmiInstrument *instrument, int64_t eventTime, UmiMarketTapeRejection *reason)
{
    size_t index = Find(tape, &instrument->instrument_id);
    UmiBar bar = {0};
    bool appendBar = false;
    if (index == tape->count) return Fail(reason, UMI_MARKET_TAPE_UNKNOWN_INSTRUMENT, UMI_STATUS_NOT_FOUND);
    TapeSlot *slot = &tape->slots[index];
    UmiMarketTapeRow *row = &slot->row;
    if (!SameInstrument(instrument, &row->instrument))
        return Fail(reason, UMI_MARKET_TAPE_BAD_INPUT, UMI_STATUS_INVALID_ARGUMENT);
    if (packet->generation != row->generation)
        return Fail(reason, UMI_MARKET_TAPE_WRONG_GENERATION, UMI_STATUS_INVALID_STATE);
    if (!row->connected) return Fail(reason, UMI_MARKET_TAPE_NO_CONNECTION, UMI_STATUS_INVALID_STATE);
    if (row->gapLatched) return Fail(reason, UMI_MARKET_TAPE_SEQUENCE_GAP, UMI_STATUS_INVALID_STATE);
    if (row->sequenceExhausted || row->acceptedEvents == UINT64_MAX || tape->revision == UINT64_MAX)
        return Fail(reason, UMI_MARKET_TAPE_LIMIT_REACHED, UMI_STATUS_CAPACITY_EXCEEDED);
    if (packet->sequence < row->expectedSequence)
        return Fail(reason, UMI_MARKET_TAPE_DUPLICATE_OR_OLD, UMI_STATUS_ALREADY_EXISTS);
    if (row->acceptedEvents != 0U && packet->receivedTimeMs < row->receivedTimeMs)
        return Fail(reason, UMI_MARKET_TAPE_TIME_REGRESSION, UMI_STATUS_INVALID_ARGUMENT);
    int64_t prior = 0;
    bool havePrior = false;
    if (packet->kind == UMI_MARKET_TAPE_QUOTE) { prior = row->quote.event_time_ms; havePrior = row->haveQuote; }
    if (packet->kind == UMI_MARKET_TAPE_TRADE) { prior = row->lastTrade.event_time_ms; havePrior = row->haveTrade; }
    if (packet->kind == UMI_MARKET_TAPE_DEPTH) { prior = slot->depth.event_time_ms; havePrior = row->haveDepth; }
    if (havePrior && eventTime < prior)
        return Fail(reason, UMI_MARKET_TAPE_TIME_REGRESSION, UMI_STATUS_INVALID_ARGUMENT);
    if (packet->kind == UMI_MARKET_TAPE_TRADE) {
        UmiStatus status = PrepareBar(tape, slot, &packet->value.trade, &bar, &appendBar);
        if (status != UMI_STATUS_OK) return Fail(reason, UMI_MARKET_TAPE_LIMIT_REACHED, status);
    }
    if (packet->sequence > row->expectedSequence) {
        row->gapLatched = true;
        row->gapObservedSequence = packet->sequence;
        ++tape->revision;
        return Fail(reason, UMI_MARKET_TAPE_SEQUENCE_GAP, UMI_STATUS_INVALID_STATE);
    }
    if (packet->kind == UMI_MARKET_TAPE_QUOTE) {
        row->quote = packet->value.quote; row->haveQuote = true;
    } else if (packet->kind == UMI_MARKET_TAPE_DEPTH) {
        slot->depth = packet->value.depth; row->haveDepth = true;
    } else {
        row->lastTrade = packet->value.trade; row->haveTrade = true;
        if (slot->tradeCount == UMI_MARKET_TAPE_TRADE_LIMIT) {
            slot->trades[slot->tradeHead] = packet->value.trade;
            slot->tradeHead = (slot->tradeHead + 1U) % UMI_MARKET_TAPE_TRADE_LIMIT;
            IncrementRetired(&row->discardedTrades);
        } else {
            slot->trades[(slot->tradeHead + slot->tradeCount) % UMI_MARKET_TAPE_TRADE_LIMIT] = packet->value.trade;
            ++slot->tradeCount;
        }
        if (!appendBar) slot->bars[(slot->barHead + slot->barCount - 1U) % UMI_MARKET_TAPE_BAR_LIMIT] = bar;
        else if (slot->barCount == UMI_MARKET_TAPE_BAR_LIMIT) {
            slot->bars[slot->barHead] = bar;
            slot->barHead = (slot->barHead + 1U) % UMI_MARKET_TAPE_BAR_LIMIT;
            IncrementRetired(&row->discardedBars);
        } else {
            slot->bars[(slot->barHead + slot->barCount) % UMI_MARKET_TAPE_BAR_LIMIT] = bar;
            ++slot->barCount;
        }
    }
    ++row->acceptedEvents;
    row->receivedTimeMs = packet->receivedTimeMs;
    row->lastSequence = packet->sequence;
    if (packet->sequence == UINT64_MAX) row->sequenceExhausted = true;
    else row->expectedSequence = packet->sequence + 1U;
    ++tape->revision;
    return Fail(reason, UMI_MARKET_TAPE_ACCEPTED, UMI_STATUS_OK);
}
UmiStatus UmiMarketTapeApply(UmiMarketTape *tape, const UmiMarketTapePacket *packet,
    UmiMarketTapeRejection *outReason)
{
    int64_t eventTime = 0;
    const UmiInstrument *instrument;
    UmiStatus status;
    if (outReason != NULL) *outReason = UMI_MARKET_TAPE_BAD_INPUT;
    if (tape == NULL || packet == NULL || packet->sequence == 0U || packet->generation == 0U ||
        packet->receivedTimeMs < 0) return UMI_STATUS_INVALID_ARGUMENT;
    instrument = PacketInstrument(packet, &eventTime);
    if (!InstrumentValid(instrument) || eventTime < 0 || eventTime > packet->receivedTimeMs)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (packet->kind == UMI_MARKET_TAPE_QUOTE && !umi_quote_valid(&packet->value.quote))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (packet->kind == UMI_MARKET_TAPE_TRADE && (!isfinite(packet->value.trade.price) ||
        !isfinite(packet->value.trade.size) || packet->value.trade.price <= 0.0 ||
        packet->value.trade.size <= 0.0)) return UMI_STATUS_INVALID_ARGUMENT;
    if (packet->kind == UMI_MARKET_TAPE_DEPTH && !DepthValid(&packet->value.depth))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(tape->mutex);
    if (status != UMI_STATUS_OK) return status;
    status = ApplyLocked(tape, packet, instrument, eventTime, outReason);
    (void)umi_mutex_unlock(tape->mutex);
    return status;
}
static bool Recent(int64_t now, int64_t time, int64_t threshold)
{
    return time <= now && (uint64_t)now - (uint64_t)time < (uint64_t)threshold;
}
UmiStatus UmiMarketTapeRead(UmiMarketTape *tape, int64_t nowMs, UmiMarketTapeSnapshot *out)
{
    UmiStatus status;
    if (tape == NULL || out == NULL || nowMs < 0) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(tape->mutex);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < tape->count; ++i) {
        if (tape->slots[i].row.acceptedEvents != 0U && nowMs < tape->slots[i].row.receivedTimeMs) {
            (void)umi_mutex_unlock(tape->mutex);
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    memset(out, 0, sizeof(*out));
    out->revision = tape->revision; out->observedAtMs = nowMs;
    out->barIntervalMs = tape->config.barIntervalMs;
    out->rowCount = tape->count; out->selectedIndex = tape->selected;
    for (size_t i = 0U; i < tape->count; ++i) {
        const TapeSlot *slot = &tape->slots[i];
        UmiMarketTapeRow *row = &out->rows[i];
        *row = slot->row;
        bool usable = row->connected && !row->gapLatched && !row->sequenceExhausted;
        row->quoteFresh = usable && row->haveQuote && Recent(nowMs, row->quote.event_time_ms, tape->config.staleAfterMs);
        row->tradeFresh = usable && row->haveTrade && Recent(nowMs, row->lastTrade.event_time_ms, tape->config.staleAfterMs);
        row->depthFresh = usable && row->haveDepth && slot->depth.bid_count > 0U && slot->depth.ask_count > 0U &&
            Recent(nowMs, slot->depth.event_time_ms, tape->config.staleAfterMs);
        row->quoteQuality = row->quoteFresh ?
            umi_market_data_quality_score(&row->quote, nowMs, tape->config.staleAfterMs) : 0.0;
        if (!row->connected) row->state = UMI_MARKET_TAPE_DISCONNECTED;
        else if (row->gapLatched) row->state = UMI_MARKET_TAPE_GAP;
        else if (row->sequenceExhausted) row->state = UMI_MARKET_TAPE_EXHAUSTED;
        else if (row->acceptedEvents == 0U) row->state = UMI_MARKET_TAPE_WAITING;
        else if (!Recent(nowMs, row->receivedTimeMs, tape->config.staleAfterMs)) row->state = UMI_MARKET_TAPE_STALE;
        else row->state = UMI_MARKET_TAPE_CURRENT;
    }
    if (tape->count != 0U) {
        const TapeSlot *slot = &tape->slots[tape->selected];
        out->tradeCount = slot->tradeCount; out->barCount = slot->barCount;
        for (size_t i = 0U; i < slot->tradeCount; ++i)
            out->trades[i] = slot->trades[(slot->tradeHead + i) % UMI_MARKET_TAPE_TRADE_LIMIT];
        for (size_t i = 0U; i < slot->barCount; ++i)
            out->bars[i] = slot->bars[(slot->barHead + i) % UMI_MARKET_TAPE_BAR_LIMIT];
        out->depth = slot->depth;
    }
    (void)umi_mutex_unlock(tape->mutex);
    return UMI_STATUS_OK;
}
const char *UmiMarketTapeStateText(UmiMarketTapeState state)
{
    switch (state) {
        case UMI_MARKET_TAPE_WAITING: return "Waiting for data";
        case UMI_MARKET_TAPE_CURRENT: return "Recent packet; check each data lane";
        case UMI_MARKET_TAPE_STALE: return "Stale";
        case UMI_MARKET_TAPE_GAP: return "Sequence gap; new epoch required";
        case UMI_MARKET_TAPE_DISCONNECTED: return "Disconnected";
        case UMI_MARKET_TAPE_EXHAUSTED: return "Sequence exhausted; new epoch required";
        default: return "Unknown state";
    }
}
const char *UmiMarketTapeRejectionText(UmiMarketTapeRejection reason)
{
    switch (reason) {
        case UMI_MARKET_TAPE_ACCEPTED: return "Accepted";
        case UMI_MARKET_TAPE_BAD_INPUT: return "Invalid market data";
        case UMI_MARKET_TAPE_UNKNOWN_INSTRUMENT: return "Unregistered instrument";
        case UMI_MARKET_TAPE_WRONG_GENERATION: return "Wrong stream generation";
        case UMI_MARKET_TAPE_NO_CONNECTION: return "Disconnected stream";
        case UMI_MARKET_TAPE_DUPLICATE_OR_OLD: return "Duplicate or old sequence";
        case UMI_MARKET_TAPE_SEQUENCE_GAP: return "Sequence gap is latched";
        case UMI_MARKET_TAPE_TIME_REGRESSION: return "Timestamp moved backwards";
        case UMI_MARKET_TAPE_LIMIT_REACHED: return "Arithmetic or sequence limit reached";
        default: return "Unknown result";
    }
}
