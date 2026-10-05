/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_replay.c
 *
 * PURPOSE:
 *   Validate replay behaviour in the trading foundation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This focused regression test uses deterministic values so changes to the trading contract are visible immediately.
 */

/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include "umicom/trading/trading.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/trading/types.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiReplayEventTransferEqual(const UmiReplayEvent *a, const UmiReplayEvent *b)
{
    return a->sequence == b->sequence &&
        a->event_time_ms == b->event_time_ms &&
        strcmp(a->type, b->type) == 0 &&
        strcmp(a->payload, b->payload) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiReplayEventTransferTails(UmiReplayEvent *value)
{
    (void)value;
    {
        size_t used = strlen(value->type) + 1U;
        memset(value->type + used, 0xa5, sizeof(value->type) - used);
    }
    {
        size_t used = strlen(value->payload) + 1U;
        memset(value->payload + used, 0xa5, sizeof(value->payload) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiReplayEventTransferMalformed(const UmiReplayEvent *sample)
{
    (void)sample;
    {
        UmiReplayEvent invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.type, 'x', sizeof(invalid.type));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_replay_event_valid(&invalid)) ||
            umi_replay_event_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated type was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiReplayEvent invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.payload, 'x', sizeof(invalid.payload));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_replay_event_valid(&invalid)) ||
            umi_replay_event_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated payload was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiReplayEventTransferCases, UmiReplayEvent,
    umi_replay_event_archive_encode, umi_replay_event_archive_decode,
    UmiReplayEventTransferEqual, UmiReplayEventTransferTails, UmiReplayEventTransferMalformed)

int main(void){
    UmiReplayEvent e={1U,1000,{0},{0}};(void)snprintf(e.type,sizeof(e.type),"%s","tick");assert(umi_replay_event_valid(&e));
    if (UmiReplayEventTransferCases(&e) != 0) return 1;

    UmiReplayCursor c;umi_replay_cursor_init(&c,1U);assert(umi_replay_cursor_accept(&c,&e));assert(c.next_sequence==2U);
    UmiReplayClock clock;umi_replay_clock_init(&clock,0,2.0);umi_replay_clock_advance(&clock,1000);assert(clock.now_ms==1000);
    assert(umi_market_replay_in_window(&e,0,2000));assert(umi_replay_event_digest(&e)!=0U);return 0;
}
