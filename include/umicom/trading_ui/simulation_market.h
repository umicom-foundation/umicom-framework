/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading_ui/simulation_market.h
 *
 * PURPOSE:
 *   Provide a deterministic, broker-free professional market simulation that
 *   makes Trader and Studio trading workspaces immediately usable and testable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the trading simulation instrument state data shared with callers of this
 * public contract.
 */
typedef struct UmiTradingSimulationInstrumentState {
    UmiInstrument instrument;
    double mid_price;
    double previous_close;
    double tick_size;
    double spread;
} UmiTradingSimulationInstrumentState;

/**
 * Represent the trading simulation market data shared with callers of this public
 * contract.
 */
typedef struct UmiTradingSimulationMarket {
    UmiTradingWorkspace *workspace;
    UmiTradingSimulationInstrumentState
        instruments[UMI_TRADING_SIMULATION_MARKET_MAX_INSTRUMENTS];
    size_t instrument_count;
    uint64_t sequence;
    /* Public trade sequence advances once per emitted instrument trade and is
     * independent from the coarser simulation-step counter. */
    uint64_t trade_sequence;
    int64_t event_time_ms;
    int seeded;
} UmiTradingSimulationMarket;

/**
 * Initialise trading simulation market from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_trading_simulation_market_init(
    UmiTradingSimulationMarket *market,
    UmiTradingWorkspace *workspace);
/**
 * Provide the trading simulation market seed default operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_simulation_market_seed_default(
    UmiTradingSimulationMarket *market,
    int64_t event_time_ms);
/**
 * Provide the trading simulation market step operation used by this module and its client
 * applications.
 */
UmiStatus umi_trading_simulation_market_step(
    UmiTradingSimulationMarket *market,
    int64_t elapsed_ms);
/**
 * Return the number of records represented by trading simulation market instrument without
 * changing their state.
 */
size_t umi_trading_simulation_market_instrument_count(
    const UmiTradingSimulationMarket *market);
/**
 * Provide the trading simulation market sequence operation used by this module and its
 * client applications.
 */
uint64_t umi_trading_simulation_market_sequence(
    const UmiTradingSimulationMarket *market);

#ifdef __cplusplus
}
#endif
#endif
