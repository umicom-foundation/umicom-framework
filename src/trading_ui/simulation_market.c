/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_ui/simulation_market.c
 *
 * PURPOSE:
 *   Seed and advance deterministic quotes, bars and depth through the same
 *   canonical workspace APIs used by broker adapters and replay infrastructure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_ui/simulation_market.h"

#include <stdio.h>
#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static void copy_text(char *destination, size_t capacity, const char *source)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    (void)snprintf(destination, capacity, "%s", source != NULL ? source : "");
}

/* Provide the make instrument operation used by this module and its client applications. */
static UmiInstrument make_instrument(const char *id,
                                     const char *symbol,
                                     const char *venue,
                                     const char *currency)
{
    UmiInstrument instrument;
    (void)memset(&instrument, 0, sizeof(instrument));
    copy_text(instrument.instrument_id.value,
              sizeof(instrument.instrument_id.value), id);
    copy_text(instrument.symbol, sizeof(instrument.symbol), symbol);
    copy_text(instrument.venue, sizeof(instrument.venue), venue);
    copy_text(instrument.currency.code, sizeof(instrument.currency.code), currency);
    instrument.multiplier = 1.0;
    return instrument;
}

/* Provide the configure default operation used by this module and its client applications. */
static void configure_default(UmiTradingSimulationInstrumentState *state,
                              const char *id,
                              const char *symbol,
                              double mid,
                              double tick,
                              double spread)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL) return;
    (void)memset(state, 0, sizeof(*state));
    state->instrument = make_instrument(id, symbol, "UMISIM", "USD");
    state->mid_price = mid;
    state->previous_close = mid - (4.0 * tick);
    state->tick_size = tick;
    state->spread = spread;
}

/*
 * Provide the publish instrument operation used by this module and its client
 * applications.
 */
static UmiStatus publish_instrument(UmiTradingSimulationMarket *market,
                                    UmiTradingSimulationInstrumentState *state,
                                    int64_t event_time_ms,
                                    double open_price)
{
    UmiQuote quote;
    UmiBar bar;
    UmiMarketDepth depth;
    UmiStatus status;
    size_t level;
    const double half_spread = state->spread * 0.5;

    (void)memset(&quote, 0, sizeof(quote));
    quote.instrument = state->instrument;
    quote.bid = state->mid_price - half_spread;
    quote.ask = state->mid_price + half_spread;
    quote.bid_size = 100.0 + (double)((market->sequence + 3U) % 17U) * 10.0;
    quote.ask_size = 100.0 + (double)((market->sequence + 7U) % 19U) * 10.0;
    quote.event_time_ms = event_time_ms;

    (void)memset(&bar, 0, sizeof(bar));
    bar.instrument = state->instrument;
    bar.open = open_price;
    bar.close = state->mid_price;
    bar.high = (bar.open > bar.close ? bar.open : bar.close) + state->tick_size;
    bar.low = (bar.open < bar.close ? bar.open : bar.close) - state->tick_size;
    bar.volume = 1000.0 + (double)(market->sequence % 250U) * 25.0;
    bar.start_time_ms = event_time_ms - 60000;
    bar.end_time_ms = event_time_ms;

    (void)memset(&depth, 0, sizeof(depth));
    depth.instrument = state->instrument;
    depth.bid_count = 5U;
    depth.ask_count = 5U;
    depth.event_time_ms = event_time_ms;
    /* Visit each bounded item once so every record receives the same rule. */
    for (level = 0U; level < 5U; ++level) {
        const double offset = state->tick_size * (double)(level + 1U);
        depth.bids[level].price = quote.bid - offset;
        depth.bids[level].size = 50.0 + (double)(level + 1U) * 25.0;
        depth.asks[level].price = quote.ask + offset;
        depth.asks[level].size = 55.0 + (double)(level + 1U) * 25.0;
    }

    status = umi_trading_workspace_update_quote(market->workspace, &quote);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_trading_workspace_update_bar(
            market->workspace, &bar, state->previous_close);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_trading_workspace_update_depth(market->workspace, &depth);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_trading_workspace_set_market_state(
            market->workspace,
            state->instrument.instrument_id.value,
            UMI_MARKET_OPEN);
    return status;
}

/*
 * Initialise trading simulation market from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_trading_simulation_market_init(
    UmiTradingSimulationMarket *market,
    UmiTradingWorkspace *workspace)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (market == NULL || workspace == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(market, 0, sizeof(*market));
    market->workspace = workspace;
    return UMI_STATUS_OK;
}

/*
 * Provide the trading simulation market seed default operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_simulation_market_seed_default(
    UmiTradingSimulationMarket *market,
    int64_t event_time_ms)
{
    UmiTradingWorkspaceSnapshot snapshot;
    UmiStatus status;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (market == NULL || market->workspace == NULL || event_time_ms < 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (market->seeded) return UMI_STATUS_ALREADY_EXISTS;

    status = umi_trading_workspace_snapshot(market->workspace, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (snapshot.watchlist_count != 0U)
        return UMI_STATUS_INVALID_STATE;

    configure_default(&market->instruments[0],
                      "SIM.EURUSD", "EURUSD", 1.17325, 0.00005, 0.00010);
    configure_default(&market->instruments[1],
                      "SIM.GBPUSD", "GBPUSD", 1.35210, 0.00005, 0.00012);
    configure_default(&market->instruments[2],
                      "SIM.USDJPY", "USDJPY", 146.850, 0.005, 0.010);
    configure_default(&market->instruments[3],
                      "SIM.XAUUSD", "XAUUSD", 3395.20, 0.10, 0.30);
    configure_default(&market->instruments[4],
                      "SIM.SPX500", "SPX500", 6510.25, 0.25, 0.50);
    configure_default(&market->instruments[5],
                      "SIM.NAS100", "NAS100", 23980.50, 0.25, 0.75);
    market->instrument_count = UMI_TRADING_SIMULATION_MARKET_DEFAULT_INSTRUMENTS;
    market->event_time_ms = event_time_ms;
    market->sequence = 1U;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < market->instrument_count; ++index) {
        status = umi_trading_workspace_add_instrument(
            market->workspace, &market->instruments[index].instrument);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = umi_trading_workspace_set_market_state(
            market->workspace,
            market->instruments[index].instrument.instrument_id.value,
            UMI_MARKET_PREOPEN);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = publish_instrument(
            market, &market->instruments[index], event_time_ms,
            market->instruments[index].previous_close);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    market->seeded = 1;
    return umi_trading_workspace_refresh(market->workspace);
}

/*
 * Provide the trading simulation market step operation used by this module and its client
 * applications.
 */
UmiStatus umi_trading_simulation_market_step(
    UmiTradingSimulationMarket *market,
    int64_t elapsed_ms)
{
    UmiTradingWorkspaceSnapshot snapshot;
    UmiStatus status;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (market == NULL || market->workspace == NULL || elapsed_ms <= 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!market->seeded) return UMI_STATUS_INVALID_STATE;
    status = umi_trading_workspace_snapshot(market->workspace, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (snapshot.environment != UMI_TRADING_SIMULATION)
        return UMI_STATUS_INVALID_STATE;

    market->event_time_ms += elapsed_ms;
    market->sequence += 1U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < market->instrument_count; ++index) {
        UmiTradingSimulationInstrumentState *state = &market->instruments[index];
        const double previous = state->mid_price;
        const uint64_t phase = (market->sequence + (uint64_t)(index * 3U)) % 12U;
        const double direction = phase < 6U ? 1.0 : -1.0;
        const double magnitude = 1.0 +
            (double)((market->sequence + (uint64_t)index) % 3U);
        state->mid_price += direction * magnitude * state->tick_size;
        status = publish_instrument(market, state, market->event_time_ms, previous);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return umi_trading_workspace_refresh(market->workspace);
}

/*
 * Return the number of records represented by trading simulation market instrument without
 * changing their state.
 */
size_t umi_trading_simulation_market_instrument_count(
    const UmiTradingSimulationMarket *market)
{
    return market != NULL ? market->instrument_count : 0U;
}

/*
 * Provide the trading simulation market sequence operation used by this module and its
 * client applications.
 */
uint64_t umi_trading_simulation_market_sequence(
    const UmiTradingSimulationMarket *market)
{
    return market != NULL ? market->sequence : 0U;
}
