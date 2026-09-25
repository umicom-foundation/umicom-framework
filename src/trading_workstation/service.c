/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/service.c
 *
 * PURPOSE:
 *   Provide one discoverable professional trading-workstation catalogue and
 *   build a bounded readiness snapshot from the canonical trading workspace.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_workstation/service.h"

#include <math.h>
#include <string.h>

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

typedef struct ProfessionalEntry {
    const char *id;
    UmiTradingProfessionalEvaluator evaluator;
} ProfessionalEntry;

static const ProfessionalEntry CATALOGUE[] = {
    {"chart_tape.activity", umi_trading_professional_evaluate_chart_tape_activity},
    {"chart_tape.chart-range", umi_trading_professional_evaluate_chart_tape_chart_range},
    {"chart_tape.chart-readiness", umi_trading_professional_evaluate_chart_tape_chart_readiness},
    {"chart_tape.linked-context", umi_trading_professional_evaluate_chart_tape_linked_context},
    {"chart_tape.momentum", umi_trading_professional_evaluate_chart_tape_momentum},
    {"chart_tape.tape-readiness", umi_trading_professional_evaluate_chart_tape_tape_readiness},
    {"chart_tape.trade-rate", umi_trading_professional_evaluate_chart_tape_trade_rate},
    {"chart_tape.trade-size-signal", umi_trading_professional_evaluate_chart_tape_trade_size_signal},
    {"chart_tape.volatility", umi_trading_professional_evaluate_chart_tape_volatility},
    {"chart_tape.volume-signal", umi_trading_professional_evaluate_chart_tape_volume_signal},
    {"depth.ask-pressure", umi_trading_professional_evaluate_depth_ask_pressure},
    {"depth.bid-pressure", umi_trading_professional_evaluate_depth_bid_pressure},
    {"depth.imbalance", umi_trading_professional_evaluate_depth_imbalance},
    {"depth.ladder-density", umi_trading_professional_evaluate_depth_ladder_density},
    {"depth.microprice", umi_trading_professional_evaluate_depth_microprice},
    {"depth.quality", umi_trading_professional_evaluate_depth_quality},
    {"depth.readiness", umi_trading_professional_evaluate_depth_readiness},
    {"depth.spread-ticks", umi_trading_professional_evaluate_depth_spread_ticks},
    {"depth.sweep-risk", umi_trading_professional_evaluate_depth_sweep_risk},
    {"depth.top-liquidity", umi_trading_professional_evaluate_depth_top_liquidity},
    {"execution.buying-power-use", umi_trading_professional_evaluate_execution_buying_power_use},
    {"execution.cancel-readiness", umi_trading_professional_evaluate_execution_cancel_readiness},
    {"execution.fill-progress", umi_trading_professional_evaluate_execution_fill_progress},
    {"execution.limit-distance", umi_trading_professional_evaluate_execution_limit_distance},
    {"execution.order-notional", umi_trading_professional_evaluate_execution_order_notional},
    {"execution.order-validation", umi_trading_professional_evaluate_execution_order_validation},
    {"execution.readiness", umi_trading_professional_evaluate_execution_readiness},
    {"execution.risk-limit-use", umi_trading_professional_evaluate_execution_risk_limit_use},
    {"execution.stop-distance", umi_trading_professional_evaluate_execution_stop_distance},
    {"execution.submit-readiness", umi_trading_professional_evaluate_execution_submit_readiness},
    {"market.feed-health", umi_trading_professional_evaluate_market_feed_health},
    {"market.quote-change", umi_trading_professional_evaluate_market_quote_change},
    {"market.quote-change-percent", umi_trading_professional_evaluate_market_quote_change_percent},
    {"market.quote-midprice", umi_trading_professional_evaluate_market_quote_midprice},
    {"market.quote-readiness", umi_trading_professional_evaluate_market_quote_readiness},
    {"market.quote-spread", umi_trading_professional_evaluate_market_quote_spread},
    {"market.quote-staleness", umi_trading_professional_evaluate_market_quote_staleness},
    {"market.range-position", umi_trading_professional_evaluate_market_range_position},
    {"market.session-range", umi_trading_professional_evaluate_market_session_range},
    {"market.spread-basis-points", umi_trading_professional_evaluate_market_spread_basis_points},
    {"portfolio.concentration", umi_trading_professional_evaluate_portfolio_concentration},
    {"portfolio.gross-exposure", umi_trading_professional_evaluate_portfolio_gross_exposure},
    {"portfolio.margin-utilisation", umi_trading_professional_evaluate_portfolio_margin_utilisation},
    {"portfolio.net-exposure", umi_trading_professional_evaluate_portfolio_net_exposure},
    {"portfolio.position-count", umi_trading_professional_evaluate_portfolio_position_count},
    {"portfolio.position-notional", umi_trading_professional_evaluate_portfolio_position_notional},
    {"portfolio.readiness", umi_trading_professional_evaluate_portfolio_readiness},
    {"portfolio.realised-pnl", umi_trading_professional_evaluate_portfolio_realised_pnl},
    {"portfolio.risk-budget", umi_trading_professional_evaluate_portfolio_risk_budget},
    {"portfolio.unrealised-pnl", umi_trading_professional_evaluate_portfolio_unrealised_pnl},
    {"research.alert-attention", umi_trading_professional_evaluate_research_alert_attention},
    {"research.alert-density", umi_trading_professional_evaluate_research_alert_density},
    {"research.alert-trigger", umi_trading_professional_evaluate_research_alert_trigger},
    {"research.calendar-impact", umi_trading_professional_evaluate_research_calendar_impact},
    {"research.calendar-proximity", umi_trading_professional_evaluate_research_calendar_proximity},
    {"research.readiness", umi_trading_professional_evaluate_research_readiness},
    {"research.research-quality", umi_trading_professional_evaluate_research_research_quality},
    {"research.scanner-momentum", umi_trading_professional_evaluate_research_scanner_momentum},
    {"research.scanner-score", umi_trading_professional_evaluate_research_scanner_score},
    {"research.scanner-volume", umi_trading_professional_evaluate_research_scanner_volume},
    {"watchlist.alert-density", umi_trading_professional_evaluate_watchlist_alert_density},
    {"watchlist.capacity", umi_trading_professional_evaluate_watchlist_capacity},
    {"watchlist.context", umi_trading_professional_evaluate_watchlist_context},
    {"watchlist.instrument-count", umi_trading_professional_evaluate_watchlist_instrument_count},
    {"watchlist.momentum", umi_trading_professional_evaluate_watchlist_momentum},
    {"watchlist.ranking", umi_trading_professional_evaluate_watchlist_ranking},
    {"watchlist.readiness", umi_trading_professional_evaluate_watchlist_readiness},
    {"watchlist.spread-quality", umi_trading_professional_evaluate_watchlist_spread_quality},
    {"watchlist.visible-ratio", umi_trading_professional_evaluate_watchlist_visible_ratio},
    {"watchlist.volume-signal", umi_trading_professional_evaluate_watchlist_volume_signal},
    {"workstation.account-context", umi_trading_professional_evaluate_workstation_account_context},
    {"workstation.chart-sync", umi_trading_professional_evaluate_workstation_chart_sync},
    {"workstation.crosshair-sync", umi_trading_professional_evaluate_workstation_crosshair_sync},
    {"workstation.data-quality", umi_trading_professional_evaluate_workstation_data_quality},
    {"workstation.instrument-context", umi_trading_professional_evaluate_workstation_instrument_context},
    {"workstation.link-group-health", umi_trading_professional_evaluate_workstation_link_group_health},
    {"workstation.order-context", umi_trading_professional_evaluate_workstation_order_context},
    {"workstation.panel-readiness", umi_trading_professional_evaluate_workstation_panel_readiness},
    {"workstation.professional-readiness", umi_trading_professional_evaluate_workstation_professional_readiness},
    {"workstation.timeframe-sync", umi_trading_professional_evaluate_workstation_timeframe_sync},
};

size_t umi_trading_professional_service_count(void)
{
    return sizeof(CATALOGUE) / sizeof(CATALOGUE[0]);
}

UmiStatus umi_trading_professional_service_id_at(
    size_t index,
    const char **outId)
{
    if (outId == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outId = NULL;
    if (index >= umi_trading_professional_service_count()) {
        return UMI_STATUS_NOT_FOUND;
    }
    *outId = CATALOGUE[index].id;
    return UMI_STATUS_OK;
}

UmiStatus umi_trading_professional_service_evaluate(
    const char *id,
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot)
{
    size_t index;

    if (id == NULL || id[0] == '\0' || input == NULL ||
        outSnapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < umi_trading_professional_service_count(); ++index) {
        if (strcmp(CATALOGUE[index].id, id) == 0) {
            return CATALOGUE[index].evaluator(input, outSnapshot);
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

static void fill_input_from_workspace(
    const UmiTradingWorkspaceSnapshot *workspace,
    UmiTradingProfessionalInput *input)
{
    double previousClose = 0.0;

    umi_trading_professional_input_init(input);
    if (workspace == NULL) return;

    if (workspace->selected_mid > 0.0) {
        previousClose = workspace->selected_mid - workspace->selected_change;
    }

    input->bid = workspace->selected_bid;
    input->ask = workspace->selected_ask;
    input->last = workspace->selected_mid;
    input->previousClose = previousClose;
    input->referencePrice = workspace->selected_mid;
    input->depthImbalance = workspace->selected_depth_imbalance;
    input->topLiquidity = workspace->selected_top_liquidity;
    input->realisedPnl = workspace->realised_pnl;

    input->count = (uint64_t)workspace->watchlist_count;
    input->visibleCount = (uint64_t)workspace->visible_instrument_count;
    input->alerts = (uint64_t)workspace->alert_count;
    input->activeAlerts = (uint64_t)workspace->active_alert_count;
    input->unacknowledgedAlerts =
        (uint64_t)workspace->unacknowledged_alert_count;

    input->orderQuantity = workspace->draft_order.quantity;
    input->limitPrice = workspace->draft_order.limit_price;
    input->stopPrice = workspace->draft_order.stop_price;
    input->riskLimit = workspace->risk_limit.max_order_notional;
    input->orderedQuantity = workspace->has_selected_order ? 1.0 : 0.0;
    input->filledQuantity = 0.0;

    input->marketDataReady = workspace->market_data_ready;
    input->brokerReady = workspace->broker_ready;
    input->riskReady = workspace->risk_ready;
    input->healthReady = workspace->health_ready;
    input->hasInstrument = workspace->has_selected_instrument;
    input->hasQuote = workspace->has_quote;
    input->hasDepth = workspace->has_depth;
    input->hasBar = workspace->has_bar;
    input->tradeTapeReady = workspace->trade_tape.provider_ready;
    input->linkedContextReady = workspace->has_selected_instrument;
    input->trusted = 1;
    input->active = workspace->health_ready;
    input->killSwitchEngaged = workspace->kill_switch_engaged;
    input->canPreviewOrder = workspace->can_preview_order;
    input->canSubmitOrder = workspace->can_submit_order;
    input->canCancelOrder = workspace->can_cancel_order;
    input->revision = workspace->revision;
}

static UmiStatus evaluate_ready(
    const char *id,
    const UmiTradingProfessionalInput *input,
    int *outReady,
    int *outAttention)
{
    UmiTradingProfessionalSnapshot result;
    UmiStatus status;

    status = umi_trading_professional_service_evaluate(
        id, input, &result);
    if (status != UMI_STATUS_OK) return status;

    if (outReady != NULL) *outReady = result.ready;
    if (outAttention != NULL) *outAttention = result.attention;
    return UMI_STATUS_OK;
}

UmiStatus umi_trading_professional_workstation_snapshot(
    UmiTradingWorkspace *workspace,
    UmiTradingProfessionalWorkstationSnapshot *outSnapshot)
{
    UmiTradingWorkspaceSnapshot trading;
    UmiTradingProfessionalInput input;
    UmiTradingProfessionalSnapshot spreadBps;
    int attention = 0;
    UmiStatus status;

    if (workspace == NULL || outSnapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(outSnapshot, 0, sizeof(*outSnapshot));
    (void)memset(&trading, 0, sizeof(trading));
    status = umi_trading_workspace_snapshot(workspace, &trading);
    if (status != UMI_STATUS_OK) return status;

    fill_input_from_workspace(&trading, &input);
    outSnapshot->revision = trading.revision;
    outSnapshot->capabilityCount = 10U;

#define EVALUATE_READY(id_, field_) do {     int localAttention = 0;     status = evaluate_ready((id_), &input, &(field_), &localAttention);     if (status != UMI_STATUS_OK) return status;     if ((field_)) ++outSnapshot->readyCount;     if (localAttention) ++outSnapshot->attentionCount; } while (0)

    EVALUATE_READY("watchlist.readiness", outSnapshot->watchlistReady);
    EVALUATE_READY("depth.readiness", outSnapshot->depthReady);
    EVALUATE_READY("chart_tape.chart-readiness", outSnapshot->chartReady);
    EVALUATE_READY("chart_tape.tape-readiness", outSnapshot->timeAndSalesReady);
    EVALUATE_READY("execution.submit-readiness", outSnapshot->orderTicketReady);
    EVALUATE_READY("portfolio.readiness", outSnapshot->portfolioReady);
    EVALUATE_READY("research.readiness", outSnapshot->alertsReady);
    EVALUATE_READY("workstation.panel-readiness", outSnapshot->scannerReady);
    EVALUATE_READY("research.readiness", outSnapshot->calendarContextReady);
    EVALUATE_READY("workstation.instrument-context", outSnapshot->linkedContextReady);

#undef EVALUATE_READY

    status = evaluate_ready(
        "workstation.professional-readiness",
        &input,
        &outSnapshot->professionalReady,
        &attention);
    if (status != UMI_STATUS_OK) return status;
    if (attention) ++outSnapshot->attentionCount;

    outSnapshot->selectedMid = trading.selected_mid;
    outSnapshot->selectedSpread = trading.selected_spread;
    outSnapshot->selectedDepthImbalance =
        trading.selected_depth_imbalance;
    outSnapshot->selectedTopLiquidity =
        trading.selected_top_liquidity;
    outSnapshot->selectedChangePercent =
        trading.selected_change_percent;
    outSnapshot->realisedPnl = trading.realised_pnl;

    status = umi_trading_professional_service_evaluate(
        "market.spread-basis-points", &input, &spreadBps);
    if (status == UMI_STATUS_OK) {
        outSnapshot->selectedSpreadBasisPoints = spreadBps.value;
    }
    return UMI_STATUS_OK;
}
