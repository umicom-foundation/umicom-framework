/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/research_replay.h
 * PURPOSE: Coordinate deterministic research replay through shared trading services.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: include/umicom/strategy_research/research_replay.h
 * Purpose: Stepwise single-instrument research with next-observation fills.
 * No broker, network, database or execution authority is present here.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RESEARCH_REPLAY_H
#define UMICOM_RESEARCH_REPLAY_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/trading/types.h"
#include "umicom/strategy_research/backtest_engine.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_RESEARCH_OBSERVATION_LIMIT 65536U
#define UMI_RESEARCH_TAG_CAPACITY 96U

typedef enum UmiResearchDirection {
    UMI_RESEARCH_SHORT=-1, UMI_RESEARCH_FLAT=0,
    UMI_RESEARCH_LONG=1, UMI_RESEARCH_HOLD=2
} UmiResearchDirection;
typedef enum UmiResearchState {
    UMI_RESEARCH_READY=0, UMI_RESEARCH_RUNNING=1,
    UMI_RESEARCH_COMPLETED=2, UMI_RESEARCH_CANCELLED=3, UMI_RESEARCH_FAILED=4
} UmiResearchState;
typedef struct UmiResearchObservation {
    uint64_t sequence;
    UmiQuote quote;
} UmiResearchObservation;
typedef struct UmiResearchConfig {
    double initialEquity;
    double units;                 /* One fixed-size position; multiplier must be 1. */
    double commissionPerUnit;     /* Charged on every executed side. */
    double slippageBps;           /* Adverse execution price adjustment, not double-charged. */
    uint32_t latencyMilliseconds;
    char strategyTag[UMI_RESEARCH_TAG_CAPACITY]; /* Caller label, NOT a code attestation. */
} UmiResearchConfig;
typedef struct UmiResearchView {
    size_t observationIndex;
    UmiResearchObservation current;
    int position;                 /* -1, 0, +1 */
    double equity;                /* Bid-marked long / ask-marked short, less paid fees. */
    int hasPending;
    int pendingTarget;
} UmiResearchView;
/** Called once for a valid step, after any previously submitted order can fill.
 * The view is borrowed for the call and exposes no future array. HOLD preserves
 * a pending order; a directional target replaces it, or cancels it if already
 * at that position. User state is borrowed until destroy. This is trusted native
 * code, NOT a sandbox. No re-entry to Step/Run/Cancel/Destroy from a callback.
 * A callback error terminates the run without publishing the current step;
 * external callback side effects cannot be rolled back. */
typedef UmiStatus (*UmiResearchStrategy)(const UmiResearchView *view,
    void *userData, UmiResearchDirection *outTarget);
typedef struct UmiResearchTrace {
    uint64_t sequence;
    int64_t timeMilliseconds;
    int position;
    int hasPending;
    int pendingTarget;
    int decision;
    double signedFillUnits;
    double fillPrice;
    double stepCommission;
    double equity;
} UmiResearchTrace;
typedef struct UmiResearchSnapshot {
    UmiResearchState state;
    UmiStatus failure;
    size_t processed;
    size_t total;
    int position;
    int hasPending;
    int pendingTarget;
    double entryPrice;
    double equity;
    double unrealisedPnl;          /* Open position gross mark, before its entry fee. */
    double totalCommission;        /* Includes fees on an unclosed entry. */
    double executionSlippage;      /* Price impact already included in fill prices. */
    double markToMarketDrawdown;
    double markToMarketDrawdownPercent; /* Worst ratio to its own contemporaneous peak. */
    uint64_t fills;
    uint64_t insufficientLiquidity;
    int profitFactorAvailable;     /* False when there are no losing closed trades. */
    UmiStrategyBacktestSnapshot closedTrades;
    char inputFingerprint[65];
} UmiResearchSnapshot;
typedef struct UmiResearchReplay UmiResearchReplay;
UmiResearchConfig UmiResearchConfigDefault(void);
/** Validates and copies the ENTIRE input first. No sorting, gap repair, code
 * loading or disk writes. Fixed input limit. Invalid creation leaves *out NULL.
 * All methods are single-owner, synchronous; use independent instances or
 * external serialisation. Caller must keep the object alive for every call. */
UmiStatus UmiResearchReplayCreate(const UmiResearchConfig *config,
    const UmiResearchObservation *observations, size_t count,
    UmiResearchStrategy strategy, void *userData, UmiResearchReplay **out);
void UmiResearchReplayDestroy(UmiResearchReplay *replay);
UmiStatus UmiResearchReplayStep(UmiResearchReplay *replay);
/** Executes at most budget observations. Budget is 1..65536. Returns errors
 * without claiming completion; outProcessed is this call's committed count. */
UmiStatus UmiResearchReplayRun(UmiResearchReplay *replay, size_t budget, size_t *outProcessed);
UmiStatus UmiResearchReplayCancel(UmiResearchReplay *replay);
UmiStatus UmiResearchReplaySnapshot(const UmiResearchReplay *replay, UmiResearchSnapshot *out);
UmiStatus UmiResearchReplayTraceAt(const UmiResearchReplay *replay, size_t index, UmiResearchTrace *out);
#ifdef __cplusplus
}
#endif
#endif
