/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading_ui/simulation_market.h
 *
 * PURPOSE:
 *   Provide a deterministic, broker-free professional market simulation that
 *   makes Trader and Studio trading workspaces immediately usable and testable.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_UI_SIMULATION_MARKET_H
#define UMICOM_TRADING_UI_SIMULATION_MARKET_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/trading/workspace.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TRADING_SIMULATION_MARKET_MAX_INSTRUMENTS 8U
#define UMI_TRADING_SIMULATION_MARKET_DEFAULT_INSTRUMENTS 6U

typedef struct UmiTradingSimulationInstrumentState {
    UmiInstrument instrument;
    double mid_price;
    double previous_close;
    double tick_size;
    double spread;
} UmiTradingSimulationInstrumentState;

typedef struct UmiTradingSimulationMarket {
    UmiTradingWorkspace *workspace;
    UmiTradingSimulationInstrumentState
        instruments[UMI_TRADING_SIMULATION_MARKET_MAX_INSTRUMENTS];
    size_t instrument_count;
    uint64_t sequence;
    int64_t event_time_ms;
    int seeded;
} UmiTradingSimulationMarket;

UmiStatus umi_trading_simulation_market_init(
    UmiTradingSimulationMarket *market,
    UmiTradingWorkspace *workspace);
UmiStatus umi_trading_simulation_market_seed_default(
    UmiTradingSimulationMarket *market,
    int64_t event_time_ms);
UmiStatus umi_trading_simulation_market_step(
    UmiTradingSimulationMarket *market,
    int64_t elapsed_ms);
size_t umi_trading_simulation_market_instrument_count(
    const UmiTradingSimulationMarket *market);
uint64_t umi_trading_simulation_market_sequence(
    const UmiTradingSimulationMarket *market);

#ifdef __cplusplus
}
#endif
#endif
