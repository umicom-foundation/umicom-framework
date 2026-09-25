/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/strategy_research/test_replay_engine.c
 *
 * PURPOSE:
 *   Verify deterministic replay window, sequence, gap, pause and speed handling.
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

#include "umicom/strategy_research/replay_engine.h"

int main(void)
{
    UmiStrategyReplaySession session;
    UmiStrategyReplaySnapshot snapshot;
    UmiReplayEvent event = {0};

    assert(umi_strategy_replay_session_begin(
               &session, 1000, 5000, 1U, 1.0) == UMI_STATUS_OK);

    event.sequence = 1U;
    event.event_time_ms = 2000;
    (void)strcpy(event.type, "bar");
    assert(umi_strategy_replay_session_accept(&session, &event) == UMI_STATUS_OK);

    event.sequence = 3U;
    event.event_time_ms = 3000;
    assert(umi_strategy_replay_session_accept(&session, &event) ==
           UMI_STATUS_INVALID_STATE);
    assert(session.missingSequences == 1U);

    assert(umi_strategy_replay_session_resynchronise(&session, 3U) ==
           UMI_STATUS_OK);
    assert(umi_strategy_replay_session_accept(&session, &event) == UMI_STATUS_OK);
    assert(umi_strategy_replay_session_set_speed(&session, 4.0) == UMI_STATUS_OK);
    assert(umi_strategy_replay_session_set_paused(&session, 1) == UMI_STATUS_OK);

    event.sequence = 4U;
    event.event_time_ms = 4000;
    assert(umi_strategy_replay_session_accept(&session, &event) == UMI_STATUS_BUSY);
    assert(umi_strategy_replay_session_set_paused(&session, 0) == UMI_STATUS_OK);
    assert(umi_strategy_replay_session_accept(&session, &event) == UMI_STATUS_OK);

    assert(umi_strategy_replay_session_snapshot(&session, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.acceptedEvents == 3U);
    assert(snapshot.rejectedEvents == 1U);
    assert(snapshot.missingSequences == 1U);
    assert(snapshot.speed == 4.0);
    assert(snapshot.progressPercent > 74.9 && snapshot.progressPercent < 75.1);
    return 0;
}
