/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/market_tape/practice.c
 *
 * PURPOSE:
 *   Make sequence loss and stale observations reproducible in a practice workspace.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/trading/market_tape_practice.h"
#include <stdlib.h>
#include <string.h>
struct UmiMarketTapePractice {
    UmiMarketTape *tape;
    UmiInstrument instruments[2];
    size_t selected;
    uint64_t generation[2], sequence[2], step[2];
    int64_t nowMs;
};
UmiStatus UmiMarketTapePracticeCreate(UmiMarketTapePractice **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiMarketTapePractice *p = calloc(1U, sizeof(*p));
    if (p == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiMarketTapeConfig config = UmiMarketTapeConfigDefault();
    UmiStatus status = UmiMarketTapeCreate(&config, &p->tape);
    if (status != UMI_STATUS_OK) { free(p); return status; }
    p->instruments[0] = (UmiInstrument){ .instrument_id = {"DEMO_ALPHA"}, .symbol = "ALPHA",
        .venue = "PRACTICE", .currency = {"GBP"}, .multiplier = 1.0 };
    p->instruments[1] = (UmiInstrument){ .instrument_id = {"DEMO_BETA"}, .symbol = "BETA",
        .venue = "PRACTICE", .currency = {"GBP"}, .multiplier = 1.0 };
    for (size_t i = 0U; i < 2U && status == UMI_STATUS_OK; ++i) {
        status = UmiMarketTapeRegister(p->tape, &p->instruments[i]);
        if (status == UMI_STATUS_OK)
            status = UmiMarketTapeBegin(p->tape, &p->instruments[i].instrument_id, 1U, 1U);
        p->generation[i] = 1U; p->sequence[i] = 1U;
    }
    if (status != UMI_STATUS_OK) { UmiMarketTapePracticeDestroy(p); return status; }
    p->nowMs = 1000000;
    *out = p;
    return UMI_STATUS_OK;
}
void UmiMarketTapePracticeDestroy(UmiMarketTapePractice *p)
{
    if (p == NULL) return;
    UmiMarketTapeDestroy(p->tape);
    free(p);
}
UmiStatus UmiMarketTapePracticeSelect(UmiMarketTapePractice *p, size_t index)
{
    if (p == NULL || index >= 2U) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiMarketTapeSelect(p->tape, &p->instruments[index].instrument_id);
    if (status == UMI_STATUS_OK) p->selected = index;
    return status;
}
UmiStatus UmiMarketTapePracticeRead(UmiMarketTapePractice *p, UmiMarketTapeSnapshot *out)
{
    return p != NULL ? UmiMarketTapeRead(p->tape, p->nowMs, out) : UMI_STATUS_INVALID_ARGUMENT;
}
UmiStatus UmiMarketTapePracticeAct(UmiMarketTapePractice *p,
    UmiMarketTapePracticeAction action, UmiMarketTapeRejection *reason)
{
    if (reason != NULL) *reason = UMI_MARKET_TAPE_BAD_INPUT;
    if (p == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t i = p->selected;
    if (action == UMI_MARKET_PRACTICE_DISCONNECT) {
        UmiStatus status = UmiMarketTapeDisconnect(p->tape, &p->instruments[i].instrument_id);
        if (status == UMI_STATUS_OK && reason != NULL) *reason = UMI_MARKET_TAPE_ACCEPTED;
        return status;
    }
    if (action == UMI_MARKET_PRACTICE_NEW_EPOCH) {
        if (p->generation[i] == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
        UmiStatus status = UmiMarketTapeBegin(p->tape, &p->instruments[i].instrument_id,
            p->generation[i] + 1U, 1U);
        if (status == UMI_STATUS_OK) {
            ++p->generation[i]; p->sequence[i] = 1U; p->step[i] = 0U;
            if (reason != NULL) *reason = UMI_MARKET_TAPE_ACCEPTED;
        }
        return status;
    }
    if (action == UMI_MARKET_PRACTICE_AGE) {
        if (p->nowMs > INT64_MAX - 3000) return UMI_STATUS_CAPACITY_EXCEEDED;
        p->nowMs += 3000;
        if (reason != NULL) *reason = UMI_MARKET_TAPE_ACCEPTED;
        return UMI_STATUS_OK;
    }
    if (action != UMI_MARKET_PRACTICE_NEXT && action != UMI_MARKET_PRACTICE_GAP)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (p->nowMs > INT64_MAX - 250 || p->sequence[i] > UINT64_MAX - 3U || p->step[i] == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    p->nowMs += 250;
    double base = i == 0U ? 100.0 : 50.0;
    unsigned position = (unsigned)(p->step[i] % 16U);
    double bid = base + (double)(position <= 8U ? position : 16U - position) * 0.25;
    UmiMarketTapePacket packet = {0};
    packet.kind = UMI_MARKET_TAPE_QUOTE;
    packet.generation = p->generation[i];
    packet.sequence = p->sequence[i] + (action == UMI_MARKET_PRACTICE_GAP ? 1U : 0U);
    packet.receivedTimeMs = p->nowMs;
    packet.value.quote = (UmiQuote){p->instruments[i], bid, bid + 0.25, 10.0, 12.0, p->nowMs};
    UmiStatus status = UmiMarketTapeApply(p->tape, &packet, reason);
    if (status != UMI_STATUS_OK) return status;
    ++p->sequence[i];
    memset(&packet.value, 0, sizeof(packet.value));
    packet.kind = UMI_MARKET_TAPE_DEPTH;
    packet.sequence = p->sequence[i];
    packet.value.depth.instrument = p->instruments[i];
    packet.value.depth.event_time_ms = p->nowMs;
    packet.value.depth.bid_count = 5U; packet.value.depth.ask_count = 5U;
    for (size_t level = 0U; level < 5U; ++level) {
        packet.value.depth.bids[level] = (UmiDepthLevel){bid - 0.25 * (double)level, 10.0 + (double)level};
        packet.value.depth.asks[level] = (UmiDepthLevel){bid + 0.25 * (double)(level + 1U), 12.0 + (double)level};
    }
    status = UmiMarketTapeApply(p->tape, &packet, reason);
    if (status != UMI_STATUS_OK) return status;
    ++p->sequence[i];
    memset(&packet.value, 0, sizeof(packet.value));
    packet.kind = UMI_MARKET_TAPE_TRADE;
    packet.sequence = p->sequence[i];
    packet.value.trade = (UmiTradeTick){p->instruments[i], bid + 0.125,
        1.0 + (double)(p->step[i] % 5U), p->nowMs};
    status = UmiMarketTapeApply(p->tape, &packet, reason);
    if (status == UMI_STATUS_OK) { ++p->sequence[i]; ++p->step[i]; }
    return status;
}
