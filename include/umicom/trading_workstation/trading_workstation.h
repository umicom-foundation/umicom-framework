/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading_workstation/trading_workstation.h
 *
 * PURPOSE:
 *   Aggregate the professional trading-workstation calculation and readiness
 *   contracts used by Trader and other financial applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_WORKSTATION_H
#define UMICOM_TRADING_WORKSTATION_H

#include "umicom/trading_workstation/types.h"
#include "umicom/trading_workstation/service.h"
#include "umicom/trading_workstation/chart_tape/activity.h"
#include "umicom/trading_workstation/chart_tape/chart_range.h"
#include "umicom/trading_workstation/chart_tape/chart_readiness.h"
#include "umicom/trading_workstation/chart_tape/linked_context.h"
#include "umicom/trading_workstation/chart_tape/momentum.h"
#include "umicom/trading_workstation/chart_tape/tape_readiness.h"
#include "umicom/trading_workstation/chart_tape/trade_rate.h"
#include "umicom/trading_workstation/chart_tape/trade_size_signal.h"
#include "umicom/trading_workstation/chart_tape/volatility.h"
#include "umicom/trading_workstation/chart_tape/volume_signal.h"
#include "umicom/trading_workstation/depth/ask_pressure.h"
#include "umicom/trading_workstation/depth/bid_pressure.h"
#include "umicom/trading_workstation/depth/imbalance.h"
#include "umicom/trading_workstation/depth/ladder_density.h"
#include "umicom/trading_workstation/depth/microprice.h"
#include "umicom/trading_workstation/depth/quality.h"
#include "umicom/trading_workstation/depth/readiness.h"
#include "umicom/trading_workstation/depth/spread_ticks.h"
#include "umicom/trading_workstation/depth/sweep_risk.h"
#include "umicom/trading_workstation/depth/top_liquidity.h"
#include "umicom/trading_workstation/execution/buying_power_use.h"
#include "umicom/trading_workstation/execution/cancel_readiness.h"
#include "umicom/trading_workstation/execution/fill_progress.h"
#include "umicom/trading_workstation/execution/limit_distance.h"
#include "umicom/trading_workstation/execution/order_notional.h"
#include "umicom/trading_workstation/execution/order_validation.h"
#include "umicom/trading_workstation/execution/readiness.h"
#include "umicom/trading_workstation/execution/risk_limit_use.h"
#include "umicom/trading_workstation/execution/stop_distance.h"
#include "umicom/trading_workstation/execution/submit_readiness.h"
#include "umicom/trading_workstation/market/feed_health.h"
#include "umicom/trading_workstation/market/quote_change.h"
#include "umicom/trading_workstation/market/quote_change_percent.h"
#include "umicom/trading_workstation/market/quote_midprice.h"
#include "umicom/trading_workstation/market/quote_readiness.h"
#include "umicom/trading_workstation/market/quote_spread.h"
#include "umicom/trading_workstation/market/quote_staleness.h"
#include "umicom/trading_workstation/market/range_position.h"
#include "umicom/trading_workstation/market/session_range.h"
#include "umicom/trading_workstation/market/spread_basis_points.h"
#include "umicom/trading_workstation/portfolio/concentration.h"
#include "umicom/trading_workstation/portfolio/gross_exposure.h"
#include "umicom/trading_workstation/portfolio/margin_utilisation.h"
#include "umicom/trading_workstation/portfolio/net_exposure.h"
#include "umicom/trading_workstation/portfolio/position_count.h"
#include "umicom/trading_workstation/portfolio/position_notional.h"
#include "umicom/trading_workstation/portfolio/readiness.h"
#include "umicom/trading_workstation/portfolio/realised_pnl.h"
#include "umicom/trading_workstation/portfolio/risk_budget.h"
#include "umicom/trading_workstation/portfolio/unrealised_pnl.h"
#include "umicom/trading_workstation/research/alert_attention.h"
#include "umicom/trading_workstation/research/alert_density.h"
#include "umicom/trading_workstation/research/alert_trigger.h"
#include "umicom/trading_workstation/research/calendar_impact.h"
#include "umicom/trading_workstation/research/calendar_proximity.h"
#include "umicom/trading_workstation/research/readiness.h"
#include "umicom/trading_workstation/research/research_quality.h"
#include "umicom/trading_workstation/research/scanner_momentum.h"
#include "umicom/trading_workstation/research/scanner_score.h"
#include "umicom/trading_workstation/research/scanner_volume.h"
#include "umicom/trading_workstation/watchlist/alert_density.h"
#include "umicom/trading_workstation/watchlist/capacity.h"
#include "umicom/trading_workstation/watchlist/context.h"
#include "umicom/trading_workstation/watchlist/instrument_count.h"
#include "umicom/trading_workstation/watchlist/momentum.h"
#include "umicom/trading_workstation/watchlist/ranking.h"
#include "umicom/trading_workstation/watchlist/readiness.h"
#include "umicom/trading_workstation/watchlist/spread_quality.h"
#include "umicom/trading_workstation/watchlist/visible_ratio.h"
#include "umicom/trading_workstation/watchlist/volume_signal.h"
#include "umicom/trading_workstation/workstation/account_context.h"
#include "umicom/trading_workstation/workstation/chart_sync.h"
#include "umicom/trading_workstation/workstation/crosshair_sync.h"
#include "umicom/trading_workstation/workstation/data_quality.h"
#include "umicom/trading_workstation/workstation/instrument_context.h"
#include "umicom/trading_workstation/workstation/link_group_health.h"
#include "umicom/trading_workstation/workstation/order_context.h"
#include "umicom/trading_workstation/workstation/panel_readiness.h"
#include "umicom/trading_workstation/workstation/professional_readiness.h"
#include "umicom/trading_workstation/workstation/timeframe_sync.h"

#endif
