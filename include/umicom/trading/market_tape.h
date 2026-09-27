/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/market_tape.h
 *
 * PURPOSE:
 *   Own bounded market observations and publish coherent linked-view snapshots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TRADING_MARKET_TAPE_H
#define UMICOM_TRADING_MARKET_TAPE_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_MARKET_TAPE_INSTRUMENT_LIMIT 16U
#define UMI_MARKET_TAPE_TRADE_LIMIT 64U
#define UMI_MARKET_TAPE_BAR_LIMIT 64U

typedef struct UmiMarketTape UmiMarketTape;
typedef struct UmiMarketTapeConfig {
    int64_t barIntervalMs;      /* 1..86400000; fixed for this tape. */
    int64_t staleAfterMs;       /* Positive; age == threshold is stale. */
} UmiMarketTapeConfig;

typedef enum UmiMarketTapeKind {
    UMI_MARKET_TAPE_QUOTE = 1,
    UMI_MARKET_TAPE_TRADE = 2,
    UMI_MARKET_TAPE_DEPTH = 3
} UmiMarketTapeKind;

/* A transport envelope, not another definition of prices/instruments/bars.
 * sequence is CONTIGUOUS PER REGISTERED INSTRUMENT within generation. Adapters
 * with venue-wide sequence numbers must demultiplex before calling this API.
 * Times use the same normalised UTC millisecond clock; negative/future source
 * timestamps and decreasing receive times are refused. No time is inferred. */
typedef struct UmiMarketTapePacket {
    UmiMarketTapeKind kind;
    uint64_t generation;
    uint64_t sequence;
    int64_t receivedTimeMs;
    union {
        UmiQuote quote;
        UmiTradeTick trade;
        UmiMarketDepth depth;   /* Complete sorted snapshot, NOT a delta. */
    } value;
} UmiMarketTapePacket;

typedef enum UmiMarketTapeState {
    UMI_MARKET_TAPE_WAITING = 0,
    UMI_MARKET_TAPE_CURRENT,
    UMI_MARKET_TAPE_STALE,
    UMI_MARKET_TAPE_GAP,
    UMI_MARKET_TAPE_DISCONNECTED,
    UMI_MARKET_TAPE_EXHAUSTED
} UmiMarketTapeState;

typedef enum UmiMarketTapeRejection {
    UMI_MARKET_TAPE_ACCEPTED = 0,
    UMI_MARKET_TAPE_BAD_INPUT,
    UMI_MARKET_TAPE_UNKNOWN_INSTRUMENT,
    UMI_MARKET_TAPE_WRONG_GENERATION,
    UMI_MARKET_TAPE_NO_CONNECTION,
    UMI_MARKET_TAPE_DUPLICATE_OR_OLD,
    UMI_MARKET_TAPE_SEQUENCE_GAP,
    UMI_MARKET_TAPE_TIME_REGRESSION,
    UMI_MARKET_TAPE_LIMIT_REACHED
} UmiMarketTapeRejection;

typedef struct UmiMarketTapeRow {
    UmiInstrument instrument;
    UmiMarketTapeState state;
    uint64_t generation;
    uint64_t expectedSequence;
    uint64_t lastSequence;
    uint64_t gapObservedSequence;
    uint64_t acceptedEvents;
    int64_t receivedTimeMs;
    bool haveQuote, haveTrade, haveDepth;
    bool quoteFresh, tradeFresh, depthFresh;
    bool gapLatched, connected, sequenceExhausted;
    UmiQuote quote;
    UmiTradeTick lastTrade;
    double quoteQuality;
    uint64_t discardedTrades;  /* Retention, not missing feed messages. */
    uint64_t discardedBars;
} UmiMarketTapeRow;

typedef struct UmiMarketTapeSnapshot {
    uint64_t revision;
    int64_t observedAtMs;
    int64_t barIntervalMs;
    size_t rowCount;
    size_t selectedIndex;
    UmiMarketTapeRow rows[UMI_MARKET_TAPE_INSTRUMENT_LIMIT];
    /* These arrays belong to selectedIndex at THIS revision. Chronological
     * order, newest at the end. Empty time buckets are never fabricated. */
    size_t tradeCount;
    UmiTradeTick trades[UMI_MARKET_TAPE_TRADE_LIMIT];
    size_t barCount;
    UmiBar bars[UMI_MARKET_TAPE_BAR_LIMIT];
    UmiMarketDepth depth;
} UmiMarketTapeSnapshot;

UmiMarketTapeConfig UmiMarketTapeConfigDefault(void);
UmiStatus UmiMarketTapeCreate(const UmiMarketTapeConfig *config, UmiMarketTape **outTape);
/* Stop/join all clients before destroy. Other calls are serialised internally;
 * no callbacks, disk/database access, allocation or UI work occurs on Apply. */
void UmiMarketTapeDestroy(UmiMarketTape *tape);
UmiStatus UmiMarketTapeRegister(UmiMarketTape *tape, const UmiInstrument *instrument);
/* Explicitly begins a new observation epoch and discards that instrument's
 * retained observations. generation must strictly increase; firstSequence > 0.
 * This acknowledges a NEW boundary, not recovery of missing historical ticks. */
UmiStatus UmiMarketTapeBegin(UmiMarketTape *tape, const UmiFinancialId *instrumentId,
    uint64_t generation, uint64_t firstSequence);
UmiStatus UmiMarketTapeDisconnect(UmiMarketTape *tape, const UmiFinancialId *instrumentId);
UmiStatus UmiMarketTapeSelect(UmiMarketTape *tape, const UmiFinancialId *instrumentId);
/* Valid gap evidence latches GAP but is NOT applied. All other refusals leave
 * model state/revision unchanged. A gap requires a new Begin; later packets
 * cannot silently restore continuity. Old/duplicate events are never counted. */
UmiStatus UmiMarketTapeApply(UmiMarketTape *tape, const UmiMarketTapePacket *packet,
    UmiMarketTapeRejection *outReason);
/* Caller owns the complete copy, including strings. No borrowed views.
 * nowMs must not precede any accepted receive timestamp; invalid calls do not
 * change outSnapshot. Freshness is calculated independently for each lane. */
UmiStatus UmiMarketTapeRead(UmiMarketTape *tape, int64_t nowMs,
    UmiMarketTapeSnapshot *outSnapshot);
const char *UmiMarketTapeStateText(UmiMarketTapeState state);
const char *UmiMarketTapeRejectionText(UmiMarketTapeRejection reason);
#ifdef __cplusplus
}
#endif
#endif
