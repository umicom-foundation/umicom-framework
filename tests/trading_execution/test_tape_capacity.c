/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_execution/test_tape_capacity.c
 *
 * PURPOSE:
 *   Verify trade-tape admission before retained-data mutation and outer-workspace delegation at capacity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../trading/test_trading_common.h"
/* Fault injection uses the real child and workspace owners. Their ordinary
 * production entry points remain the operations under qualification. */
#include "../../src/trading/trade_tape.c"
#include "../../src/trading/workspace.c"
#define TAPE_CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); result = 1; goto done; } } while (0)

int main(int argc, char **argv)
{
    static const char *known[] = {"append-ordinary", "append-last", "append-exhausted", "append-full", "append-stale", "append-invalid", "filter-ordinary", "filter-last", "filter-exhausted", "filter-noop", "filter-invalid", "pause-ordinary", "pause-last", "pause-exhausted", "pause-noop", "pause-resume", "remove-ordinary", "remove-last", "remove-exhausted", "remove-absent", "ready-ordinary", "ready-last", "ready-exhausted", "ready-noop", "workspace-filter-owner", "workspace-filter-child", "workspace-pause-owner", "workspace-pause-child", "workspace-ready-both"};
    if (argc != 2) return 2;
    const char *name = argv[1]; int found = 0, result = 0;
    for (size_t i = 0U; i < sizeof(known)/sizeof(known[0]); ++i) if (!strcmp(name, known[i])) found = 1;
    if (!found) return 2;
    UmiTradingTradeTape *tape = NULL, *before = NULL;
    UmiTradingWorkspace *workspace = NULL, *ownerBefore = NULL;
    UmiInstrument instrument = test_instrument();
    UmiTradeTick tick = {0}; tick.instrument = instrument; tick.price = 100.0; tick.size = 4.0; tick.event_time_ms = 1000;
    UmiTradingTradeTapeRecord record;
    TAPE_CHECK(umi_trading_trade_tape_record_init(&record, 1U, &tick,
        UMI_TRADING_TRADE_DIRECTION_BUYER_INITIATED, "regular") == UMI_STATUS_OK);
    int outer = !strncmp(name, "workspace-", 10U);
    if (outer) {
        TAPE_CHECK(umi_trading_workspace_create(NULL, &workspace) == UMI_STATUS_OK);
        tape = workspace->trade_tape;
    } else TAPE_CHECK(umi_trading_trade_tape_create(&tape) == UMI_STATUS_OK);
    TAPE_CHECK(umi_trading_trade_tape_append(tape, &record) == UMI_STATUS_OK);
    if (!strcmp(name, "append-full")) {
        for (size_t i = 1U; i < UMI_TRADING_TRADE_TAPE_CAPACITY; ++i) {
            record.sequence = (uint64_t)i + 1U;
            TAPE_CHECK(umi_trading_trade_tape_append(tape, &record) == UMI_STATUS_OK);
        }
    }
    if (!strcmp(name, "pause-resume")) TAPE_CHECK(umi_trading_trade_tape_set_paused(tape, 1) == UMI_STATUS_OK);
    uint64_t oldSequence = tape->last_sequence;
    record.sequence = oldSequence + 2U; /* A rejected gap must not change diagnostics. */
    int ordinary = strstr(name, "-ordinary") != NULL, last = strstr(name, "-last") != NULL;
    tape->revision = ordinary ? 42U : last ? UINT64_MAX - 1U : UINT64_MAX;
    if (outer) {
        workspace->revision = strstr(name, "-owner") || strstr(name, "-both") ? UINT64_MAX : 42U;
        if (strstr(name, "-owner")) tape->revision = 42U;
        ownerBefore = malloc(sizeof(*ownerBefore)); TAPE_CHECK(ownerBefore != NULL);
        memcpy(ownerBefore, workspace, sizeof(*ownerBefore));
    }
    before = malloc(sizeof(*before)); TAPE_CHECK(before != NULL); memcpy(before, tape, sizeof(*before));
    int noop = strstr(name, "-noop") != NULL || !strcmp(name, "remove-absent");
    int invalid = strstr(name, "-invalid") != NULL;
    int stale = !strcmp(name, "append-stale");
    int ready = !strncmp(name, "ready-", 6U) || !strcmp(name, "workspace-ready-both");
    UmiStatus expected = invalid ? UMI_STATUS_INVALID_ARGUMENT : stale ? UMI_STATUS_INVALID_STATE :
        ordinary || last || noop || ready ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status;
    if (outer) {
        if (strstr(name, "-filter-")) status = umi_trading_workspace_set_trade_tape_filter(
            workspace, UMI_TRADING_TRADE_TAPE_UNKNOWN, 2.0);
        else if (strstr(name, "-pause-")) status = umi_trading_workspace_set_trade_tape_paused(workspace, 1);
        else status = umi_trading_workspace_set_trade_tape_provider_ready(workspace, 0);
    } else if (!strncmp(name, "append-", 7U)) {
        if (invalid) record.structure_size = 0U;
        if (stale) record.sequence = oldSequence;
        status = umi_trading_trade_tape_append(tape, &record);
    } else if (!strncmp(name, "filter-", 7U)) {
        status = umi_trading_trade_tape_set_filter(tape,
            invalid ? (UmiTradingTradeTapeFilter)99 : noop ? UMI_TRADING_TRADE_TAPE_ALL : UMI_TRADING_TRADE_TAPE_UNKNOWN,
            noop ? 0.0 : 2.0);
    } else if (!strncmp(name, "pause-", 6U)) {
        status = umi_trading_trade_tape_set_paused(tape, noop || !strcmp(name, "pause-resume") ? 0 : 1);
    } else if (!strncmp(name, "remove-", 7U)) {
        status = umi_trading_trade_tape_remove_instrument(tape, noop ? "absent-instrument" : instrument.instrument_id.value);
    } else status = umi_trading_trade_tape_set_provider_ready(tape, noop ? 1 : 0);
    TAPE_CHECK(status == expected);
    if (expected != UMI_STATUS_OK || noop) {
        TAPE_CHECK(!memcmp(before, tape, sizeof(*tape)));
        if (outer) TAPE_CHECK(!memcmp(ownerBefore, workspace, sizeof(*workspace)));
    } else {
        TAPE_CHECK(tape->revision == (before->revision == UINT64_MAX ? UINT64_MAX : before->revision + 1U));
        if (ready) {
            TAPE_CHECK(!tape->provider_ready && tape->count == before->count && tape->last_sequence == before->last_sequence);
            TAPE_CHECK(!memcmp(tape->records, before->records, sizeof(tape->records)));
            if (outer) TAPE_CHECK(workspace->revision == UINT64_MAX);
        } else if (!strncmp(name, "append-", 7U)) {
            TAPE_CHECK(tape->last_sequence == record.sequence && tape->missing_sequence_count == before->missing_sequence_count + 1U);
            TAPE_CHECK(tape->count == before->count + 1U);
        } else if (!strncmp(name, "filter-", 7U)) {
            TAPE_CHECK(tape->filter == UMI_TRADING_TRADE_TAPE_UNKNOWN && tape->minimum_size == 2.0 && tape->count == before->count);
        } else if (!strncmp(name, "pause-", 6U)) {
            TAPE_CHECK(tape->paused && tape->paused_at_sequence == oldSequence);
        } else {
            TAPE_CHECK(tape->count == 0U && tape->last_sequence == oldSequence);
            UmiTradingTradeTapeRecord cleared; memset(&cleared, 0, sizeof(cleared));
            TAPE_CHECK(!memcmp(&tape->records[0], &cleared, sizeof(cleared)));
        }
    }
    UmiTradingTradeTapeSnapshot snapshot;
    TAPE_CHECK(umi_trading_trade_tape_snapshot(tape, &snapshot) == UMI_STATUS_OK);
    TAPE_CHECK(snapshot.revision == tape->revision && snapshot.retained_count == tape->count);
done:
    free(ownerBefore); free(before);
    if (workspace != NULL) umi_trading_workspace_destroy(workspace);
    else if (tape != NULL) umi_trading_trade_tape_destroy(tape);
    return result;
}
