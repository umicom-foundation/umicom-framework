/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_execution/test_simulation_clock.c
 * PURPOSE: Verify delayed simulation clocks and bounded step arithmetic.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "umicom/trading_ui/trading_ui.h"
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); return 1; } } while (0)

static int Run(const char *name, UmiTradingWorkspace *workspace)
{
    UmiTradingSimulationMarket simulation;
    UmiTradingMarketSnapshot market;
    UmiRiskDecision decision;
    CHECK(umi_trading_simulation_market_init(&simulation, workspace) == UMI_STATUS_OK);
    CHECK(umi_trading_simulation_market_seed_default(&simulation, 1000) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_set_draft_type(workspace, UMI_ORDER_MARKET, UMI_TIF_DAY) == UMI_STATUS_OK);
    if (strcmp(name, "delay") == 0) {
        CHECK(UmiTradingWorkspacePreviewOrderAt(workspace, 9000, &decision) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(UmiTradingSimulationMarketAdvanceTo(&simulation, 9000) == UMI_STATUS_OK);
        CHECK(umi_trading_workspace_selected_market(workspace, &market) == UMI_STATUS_OK);
        CHECK(market.quote.event_time_ms == 9000);
        CHECK(UmiTradingWorkspacePreviewOrderAt(workspace, 9000, &decision) == UMI_STATUS_OK && decision.allowed);
    } else if (strcmp(name, "backwards") == 0) {
        uint64_t sequence = simulation.sequence;
        CHECK(UmiTradingSimulationMarketAdvanceTo(&simulation, 1000) == UMI_STATUS_OK);
        CHECK(UmiTradingSimulationMarketAdvanceTo(&simulation, 999) == UMI_STATUS_INVALID_STATE);
        CHECK(sequence == simulation.sequence && simulation.event_time_ms == 1000);
    } else if (strcmp(name, "overflow") == 0) {
        uint64_t sequence = simulation.sequence;
        CHECK(umi_trading_simulation_market_step(&simulation, INT64_MAX) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(sequence == simulation.sequence && simulation.event_time_ms == 1000);
        simulation.instrument_count = SIZE_MAX;
        CHECK(umi_trading_simulation_market_step(&simulation, 1) == UMI_STATUS_INVALID_STATE);
        CHECK(sequence == simulation.sequence && simulation.event_time_ms == 1000);
    } else return 2;
    return 0;
}

int main(int argc, char **argv)
{
    UmiTradingWorkspace *workspace = NULL;
    if (argc != 2 || umi_trading_workspace_create(NULL, &workspace) != UMI_STATUS_OK) return 1;
    int status = Run(argv[1], workspace);
    umi_trading_workspace_destroy(workspace);
    return status;
}
