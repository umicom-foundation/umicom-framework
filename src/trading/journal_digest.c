/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/journal_digest.c
 *
 * PURPOSE:
 *   Calculate a deterministic lightweight digest for replay evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of journal digest. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/journal_digest.h"

/*
 * Provide the replay event digest operation used by this module and its client
 * applications.
 */
uint64_t umi_replay_event_digest(const UmiReplayEvent *event)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (event == NULL) {
        return 0U;
    }

    uint64_t hash = 1469598103934665603ULL;
    const unsigned char *cursor = (const unsigned char *)event->type;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != 0U) {
        hash ^= (uint64_t)*cursor;
        hash *= 1099511628211ULL;
        cursor++;
    }

    hash ^= event->sequence;
    hash *= 1099511628211ULL;
    hash ^= (uint64_t)event->event_time_ms;
    return hash;
}
