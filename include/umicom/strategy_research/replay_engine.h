/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/replay_engine.h
 *
 * PURPOSE:
 *   Orchestrate deterministic historical replay over the canonical replay
 *   clock, event validation, cursor and market-window contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_REPLAY_ENGINE_H
#define UMICOM_STRATEGY_RESEARCH_REPLAY_ENGINE_H

#include "umicom/base/status.h"
#include "umicom/trading/replay_clock.h"
#include "umicom/trading/replay_cursor.h"
#include "umicom/trading/replay_event.h"
#include "umicom/trading/market_replay.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStrategyReplaySession {
    UmiReplayClock clock;
    UmiReplayCursor cursor;
    int64_t startMilliseconds;
    int64_t endMilliseconds;
    uint64_t acceptedEvents;
    uint64_t rejectedEvents;
    uint64_t missingSequences;
    uint64_t lastSequence;
    int active;
    int paused;
} UmiStrategyReplaySession;

typedef struct UmiStrategyReplaySnapshot {
    int64_t nowMilliseconds;
    int64_t startMilliseconds;
    int64_t endMilliseconds;
    double speed;
    double progressPercent;
    uint64_t nextSequence;
    uint64_t acceptedEvents;
    uint64_t rejectedEvents;
    uint64_t missingSequences;
    int active;
    int paused;
} UmiStrategyReplaySnapshot;

UmiStatus umi_strategy_replay_session_begin(
    UmiStrategyReplaySession *session,
    int64_t startMilliseconds,
    int64_t endMilliseconds,
    uint64_t firstSequence,
    double speed);

UmiStatus umi_strategy_replay_session_set_paused(
    UmiStrategyReplaySession *session,
    int paused);

UmiStatus umi_strategy_replay_session_set_speed(
    UmiStrategyReplaySession *session,
    double speed);

UmiStatus umi_strategy_replay_session_accept(
    UmiStrategyReplaySession *session,
    const UmiReplayEvent *event);

UmiStatus umi_strategy_replay_session_resynchronise(
    UmiStrategyReplaySession *session,
    uint64_t nextSequence);

UmiStatus umi_strategy_replay_session_snapshot(
    const UmiStrategyReplaySession *session,
    UmiStrategyReplaySnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
