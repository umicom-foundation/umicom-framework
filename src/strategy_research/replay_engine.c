/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/replay_engine.c
 *
 * PURPOSE:
 *   Implement deterministic research replay while preserving canonical replay
 *   clock and sequence semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/replay_engine.h"

#include <string.h>

UmiStatus umi_strategy_replay_session_begin(
    UmiStrategyReplaySession *session,
    int64_t startMilliseconds,
    int64_t endMilliseconds,
    uint64_t firstSequence,
    double speed)
{
    if (session == NULL || startMilliseconds < 0 ||
        endMilliseconds < startMilliseconds ||
        firstSequence == 0U || speed <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(session, 0, sizeof(*session));
    session->startMilliseconds = startMilliseconds;
    session->endMilliseconds = endMilliseconds;
    session->active = 1;
    umi_replay_clock_init(&session->clock, startMilliseconds, speed);
    umi_replay_cursor_init(&session->cursor, firstSequence);
    return UMI_STATUS_OK;
}

UmiStatus umi_strategy_replay_session_set_paused(
    UmiStrategyReplaySession *session,
    int paused)
{
    if (session == NULL || !session->active) {
        return UMI_STATUS_INVALID_STATE;
    }
    session->paused = paused != 0;
    return UMI_STATUS_OK;
}

UmiStatus umi_strategy_replay_session_set_speed(
    UmiStrategyReplaySession *session,
    double speed)
{
    if (session == NULL || !session->active || speed <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    session->clock.speed = speed;
    return UMI_STATUS_OK;
}

UmiStatus umi_strategy_replay_session_accept(
    UmiStrategyReplaySession *session,
    const UmiReplayEvent *event)
{
    if (session == NULL || event == NULL || !session->active) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (session->paused) return UMI_STATUS_BUSY;
    if (!umi_replay_event_valid(event)) {
        session->rejectedEvents += 1U;
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!umi_market_replay_in_window(
            event,
            session->startMilliseconds,
            session->endMilliseconds)) {
        session->rejectedEvents += 1U;
        return UMI_STATUS_NOT_FOUND;
    }
    if (event->sequence != session->cursor.next_sequence) {
        if (event->sequence > session->cursor.next_sequence) {
            session->missingSequences +=
                event->sequence - session->cursor.next_sequence;
        }
        session->rejectedEvents += 1U;
        return UMI_STATUS_INVALID_STATE;
    }
    if (!umi_replay_cursor_accept(&session->cursor, event)) {
        session->rejectedEvents += 1U;
        return UMI_STATUS_INVALID_STATE;
    }

    umi_replay_clock_advance(&session->clock, event->event_time_ms);
    session->lastSequence = event->sequence;
    session->acceptedEvents += 1U;
    if (session->clock.now_ms >= session->endMilliseconds) {
        session->active = 0;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_strategy_replay_session_resynchronise(
    UmiStrategyReplaySession *session,
    uint64_t nextSequence)
{
    if (session == NULL || nextSequence == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_replay_cursor_init(&session->cursor, nextSequence);
    return UMI_STATUS_OK;
}

UmiStatus umi_strategy_replay_session_snapshot(
    const UmiStrategyReplaySession *session,
    UmiStrategyReplaySnapshot *outSnapshot)
{
    double range;

    if (session == NULL || outSnapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(outSnapshot, 0, sizeof(*outSnapshot));
    outSnapshot->nowMilliseconds = session->clock.now_ms;
    outSnapshot->startMilliseconds = session->startMilliseconds;
    outSnapshot->endMilliseconds = session->endMilliseconds;
    outSnapshot->speed = session->clock.speed;
    outSnapshot->nextSequence = session->cursor.next_sequence;
    outSnapshot->acceptedEvents = session->acceptedEvents;
    outSnapshot->rejectedEvents = session->rejectedEvents;
    outSnapshot->missingSequences = session->missingSequences;
    outSnapshot->active = session->active;
    outSnapshot->paused = session->paused;

    range = (double)(session->endMilliseconds - session->startMilliseconds);
    if (range > 0.0) {
        outSnapshot->progressPercent =
            ((double)(session->clock.now_ms - session->startMilliseconds) /
             range) * 100.0;
        if (outSnapshot->progressPercent < 0.0) {
            outSnapshot->progressPercent = 0.0;
        }
        if (outSnapshot->progressPercent > 100.0) {
            outSnapshot->progressPercent = 100.0;
        }
    }
    return UMI_STATUS_OK;
}
