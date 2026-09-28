/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/component_integration/main.c
 *
 * PURPOSE:
 *   Use three public Framework components in one native consumer. A fictional
 *   tape and historical strategy share instrument records. Paper/Live options
 *   are validated separately; neither mode opens a socket or enables orders.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/market_tape.h"
#include "umicom/strategy_research/research_replay.h"
#include "umicom/broker_connectivity/connection.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* The callback requests simulated positions, not broker orders. The replay
 * service decides whether an instruction is eligible on a later observation. */
static UmiStatus PracticeStrategy(const UmiResearchView *view, void *context,
    UmiResearchDirection *target)
{
    (void)context;
    if (view == NULL || target == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *target = view->observationIndex == 0U ? UMI_RESEARCH_LONG :
        view->observationIndex == 2U ? UMI_RESEARCH_FLAT : UMI_RESEARCH_HOLD;
    return UMI_STATUS_OK;
}

int main(void)
{
    UmiMarketTape *tape = NULL;
    UmiResearchReplay *replay = NULL;
    UmiMarketTapeSnapshot *tapeView = calloc(1U, sizeof(*tapeView));
    UmiMarketTapeConfig tapeConfig = UmiMarketTapeConfigDefault();
    UmiResearchConfig researchConfig = UmiResearchConfigDefault();
    UmiResearchSnapshot result = {0};
    UmiResearchObservation observations[4] = {0};
    const double prices[4] = {100.0, 101.0, 99.0, 100.5};
    UmiInstrument instrument = {.instrument_id = {"WORKSHOP"}, .symbol = "WORKSHOP",
        .venue = "PRACTICE", .currency = {"GBP"}, .multiplier = 1.0};
    UmiStatus status = UMI_STATUS_OK;
    size_t processed = 0U;
    int exitCode = 1;
    if (tapeView == NULL) {
        fputs("Cannot allocate the owned tape snapshot.\n", stderr);
        return 1;
    }
    status = UmiMarketTapeCreate(&tapeConfig, &tape);
    if (status == UMI_STATUS_OK) status = UmiMarketTapeRegister(tape, &instrument);
    if (status == UMI_STATUS_OK) status = UmiMarketTapeBegin(tape, &instrument.instrument_id, 1U, 1U);
    for (size_t i = 0U; i < 4U && status == UMI_STATUS_OK; ++i) {
        UmiMarketTapePacket packet = {0};
        int64_t timestamp = 1000 + (int64_t)i * 200;
        packet.kind = UMI_MARKET_TAPE_TRADE;
        packet.generation = 1U;
        packet.sequence = (uint64_t)i + 1U;
        packet.receivedTimeMs = timestamp;
        packet.value.trade = (UmiTradeTick){instrument, prices[i], 2.0, timestamp};
        status = UmiMarketTapeApply(tape, &packet, NULL);
        observations[i].sequence = (uint64_t)i + 1U;
        observations[i].quote.instrument = instrument;
        observations[i].quote.bid = prices[i] - 0.5;
        observations[i].quote.ask = prices[i] + 0.5;
        observations[i].quote.bid_size = 10.0;
        observations[i].quote.ask_size = 10.0;
        observations[i].quote.event_time_ms = timestamp;
    }
    if (status == UMI_STATUS_OK) status = UmiMarketTapeRead(tape, 1800, tapeView);
    if (status != UMI_STATUS_OK || tapeView->barCount != 1U) {
        if (status == UMI_STATUS_OK) status = UMI_STATUS_INTERNAL_ERROR;
        goto finished;
    }
    researchConfig.initialEquity = 10000.0;
    researchConfig.units = 1.0;
    researchConfig.commissionPerUnit = 0.0;
    researchConfig.slippageBps = 0.0;
    researchConfig.latencyMilliseconds = 0U;
    status = UmiResearchReplayCreate(&researchConfig, observations, 4U,
        PracticeStrategy, NULL, &replay);
    if (status == UMI_STATUS_OK) status = UmiResearchReplayRun(replay, 4U, &processed);
    if (status == UMI_STATUS_OK) status = UmiResearchReplaySnapshot(replay, &result);
    if (status != UMI_STATUS_OK || processed != 4U || result.state != UMI_RESEARCH_COMPLETED ||
        result.position != 0 || result.fills != 2U || fabs(result.equity - 9998.5) > 0.000001) {
        if (status == UMI_STATUS_OK) status = UMI_STATUS_INTERNAL_ERROR;
        goto finished;
    }
    UmiIbkrConnectionOptions paper = UmiIbkrConnectionOptionsDefault();
    UmiIbkrConnectionOptions live = paper;
    live.environment = UMI_TRADING_LIVE;
    live.adapter.paperOnly = 0;
    live.adapter.port = UmiIbkrDefaultPort(UMI_IBKR_TWS, UMI_TRADING_LIVE);
    if (UmiIbkrConnectionValidate(&paper) != UMI_STATUS_OK ||
        UmiIbkrConnectionValidate(&live) != UMI_STATUS_PERMISSION_DENIED) {
        status = UMI_STATUS_INTERNAL_ERROR;
        goto finished;
    }
    live.acknowledgeLive = true;
    if (UmiIbkrConnectionValidate(&live) != UMI_STATUS_OK ||
        !paper.adapter.readOnly || !live.adapter.readOnly) {
        status = UMI_STATUS_INTERNAL_ERROR;
        goto finished;
    }
    printf("WORKSHOP bar: open %.2f, high %.2f, low %.2f, close %.2f, volume %.0f\n",
        tapeView->bars[0].open, tapeView->bars[0].high, tapeView->bars[0].low,
        tapeView->bars[0].close, tapeView->bars[0].volume);
    printf("Historical replay: %zu observations; final equity %.2f; no open position.\n",
        processed, result.equity);
    puts("Paper and Live configuration: read-only; Live acknowledgement required.");
    puts("Practice complete. No broker connection, file write or real order was made.");
    exitCode = 0;
finished:
    if (exitCode != 0) fprintf(stderr, "Component check failed (status %d).\n", (int)status);
    UmiResearchReplayDestroy(replay);
    UmiMarketTapeDestroy(tape);
    free(tapeView);
    return exitCode;
}
