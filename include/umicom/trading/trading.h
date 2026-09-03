/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/trading.h
 *
 * PURPOSE:
 *   Provide one convenience include for canonical trading, risk, replay and research contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This header gathers the public trading building blocks; it does not introduce a hidden global trading object.
 */
#ifndef UMICOM_TRADING_TRADING_H
#define UMICOM_TRADING_TRADING_H
#include "umicom/trading/types.h"
#include "umicom/trading/instrument.h"
#include "umicom/trading/venue.h"
#include "umicom/trading/market_session.h"
#include "umicom/trading/market_state.h"
#include "umicom/trading/quote.h"
#include "umicom/trading/tick.h"
#include "umicom/trading/trade_tape.h"
#include "umicom/trading/bar.h"
#include "umicom/trading/depth.h"
#include "umicom/trading/order_book.h"
#include "umicom/trading/market_data_quality.h"
#include "umicom/trading/market_data_snapshot.h"
#include "umicom/trading/order_request.h"
#include "umicom/trading/order_type.h"
#include "umicom/trading/time_in_force.h"
#include "umicom/trading/order_state.h"
#include "umicom/trading/order_validation.h"
#include "umicom/trading/order_store.h"
#include "umicom/trading/execution_report.h"
#include "umicom/trading/fill.h"
#include "umicom/trading/fee.h"
#include "umicom/trading/position.h"
#include "umicom/trading/position_book.h"
#include "umicom/trading/pnl.h"
#include "umicom/trading/portfolio.h"
#include "umicom/trading/cash.h"
#include "umicom/trading/buying_power.h"
#include "umicom/trading/risk_limit.h"
#include "umicom/trading/risk_context.h"
#include "umicom/trading/risk_decision.h"
#include "umicom/trading/pretrade_risk.h"
#include "umicom/trading/kill_switch.h"
#include "umicom/trading/order_throttle.h"
#include "umicom/trading/oms.h"
#include "umicom/trading/broker.h"
#include "umicom/trading/simulated_broker.h"
#include "umicom/trading/ibkr_boundary.h"
#include "umicom/trading/replay_event.h"
#include "umicom/trading/replay_cursor.h"
#include "umicom/trading/replay_clock.h"
#include "umicom/trading/market_replay.h"
#include "umicom/trading/journal_digest.h"
#include "umicom/trading/strategy_signal.h"
#include "umicom/trading/feature_vector.h"
#include "umicom/trading/movement_event.h"
#include "umicom/trading/factor_observation.h"
#include "umicom/trading/factor_attribution.h"
#include "umicom/trading/research_stats.h"
#include "umicom/trading/watchlist.h"
#include "umicom/trading/alert.h"
#include "umicom/trading/surveillance.h"
#include "umicom/trading/self_trade.h"
#include "umicom/trading/price_band.h"
#include "umicom/trading/session_calendar.h"
#include "umicom/trading/health.h"
#include "umicom/trading/trade_capture.h"
#include "umicom/trading/execution_store.h"
#include "umicom/trading/venue_adapter.h"
#include "umicom/trading/fix_boundary.h"
#include "umicom/trading/environment.h"
#include "umicom/trading/trading_profile.h"
#include "umicom/trading/market_gap.h"
#include "umicom/trading/volatility_proxy.h"
#include "umicom/trading/momentum.h"
#include "umicom/trading/mean_reversion.h"
#include "umicom/trading/support_resistance.h"
#include "umicom/trading/research_window.h"
#include "umicom/trading/workspace.h"
#endif
