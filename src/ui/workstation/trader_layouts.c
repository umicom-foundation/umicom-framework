/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/trader_layouts.c
 *
 * PURPOSE:
 *   Implement Framework-owned Trader presets derived from the supplied Active Trading, Multi-Chart, Research and Portfolio/Risk mockups.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/trader_layouts.h"

static UmiStatus add(UmiWsLayoutTemplate *layout, const char *id, UmiWsDockRegion region, int32_t row, int32_t col, double weight) {
    return umi_ws_layout_template_add(layout, id, region, row, col, weight);
}

UmiStatus umi_ws_trader_layout_active(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "trader.active", "Active Trading", UMI_WS_DOMAIN_TRADER);
    if (s != UMI_STATUS_OK) return s;
    if ((s = add(layout, "trader.market-radar", UMI_WS_DOCK_LEFT, 0, 0, 0.8)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "trader.events-alerts", UMI_WS_DOCK_LEFT, 1, 0, 0.5)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "trader.primary-chart", UMI_WS_DOCK_CENTRE, 0, 1, 2.2)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "trader.orders-positions", UMI_WS_DOCK_BOTTOM, 1, 1, 0.7)) != UMI_STATUS_OK) return s;
    return add(layout, "trader.dom-order", UMI_WS_DOCK_RIGHT, 0, 2, 0.95);
}

UmiStatus umi_ws_trader_layout_multi_chart(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "trader.multi-chart", "Multi-Chart Analysis", UMI_WS_DOMAIN_TRADER);
    if (s != UMI_STATUS_OK) return s;
    if ((s = add(layout, "trader.watchlist", UMI_WS_DOCK_LEFT, 0, 0, 0.7)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "trader.chart.1", UMI_WS_DOCK_CENTRE, 0, 1, 1.0)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "trader.chart.2", UMI_WS_DOCK_CENTRE, 0, 2, 1.0)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "trader.chart.3", UMI_WS_DOCK_CENTRE, 1, 1, 1.0)) != UMI_STATUS_OK) return s;
    return add(layout, "trader.chart.4", UMI_WS_DOCK_CENTRE, 1, 2, 1.0);
}

UmiStatus umi_ws_trader_layout_research(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "trader.research", "Predictive Research", UMI_WS_DOMAIN_TRADER);
    if (s != UMI_STATUS_OK) return s;
    if ((s = add(layout, "trader.feature-snapshot", UMI_WS_DOCK_LEFT, 0, 0, 0.8)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "trader.forecast-chart", UMI_WS_DOCK_CENTRE, 0, 1, 2.0)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "trader.analogues", UMI_WS_DOCK_BOTTOM, 1, 1, 0.7)) != UMI_STATUS_OK) return s;
    return add(layout, "trader.model-evidence", UMI_WS_DOCK_RIGHT, 0, 2, 0.9);
}

UmiStatus umi_ws_trader_layout_portfolio_risk(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "trader.portfolio-risk", "Portfolio & Risk", UMI_WS_DOMAIN_TRADER);
    if (s != UMI_STATUS_OK) return s;
    if ((s = add(layout, "trader.accounts", UMI_WS_DOCK_LEFT, 0, 0, 0.8)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "trader.exposure-map", UMI_WS_DOCK_CENTRE, 0, 1, 2.0)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "trader.scenario-margin", UMI_WS_DOCK_BOTTOM, 1, 1, 0.7)) != UMI_STATUS_OK) return s;
    return add(layout, "trader.risk-limits", UMI_WS_DOCK_RIGHT, 0, 2, 0.9);
}

UmiStatus umi_ws_trader_layout_scalper(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "trader.scalper", "Futures Scalper", UMI_WS_DOMAIN_TRADER);
    if (s != UMI_STATUS_OK) return s;
    if ((s = add(layout, "trader.dom", UMI_WS_DOCK_LEFT, 0, 0, 1.0)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "trader.order-flow-chart", UMI_WS_DOCK_CENTRE, 0, 1, 1.8)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "trader.time-sales", UMI_WS_DOCK_RIGHT, 0, 2, 0.8)) != UMI_STATUS_OK) return s;
    return add(layout, "trader.positions", UMI_WS_DOCK_BOTTOM, 1, 1, 0.6);
}

UmiStatus umi_ws_trader_layout_strategy_lab(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "trader.strategy-lab", "Strategy Lab", UMI_WS_DOMAIN_TRADER);
    if (s != UMI_STATUS_OK) return s;
    if ((s = add(layout, "trader.replay-chart", UMI_WS_DOCK_CENTRE, 0, 1, 1.8)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "trader.strategy-parameters", UMI_WS_DOCK_LEFT, 0, 0, 0.8)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "trader.optimisation", UMI_WS_DOCK_RIGHT, 0, 2, 0.8)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "trader.trade-list", UMI_WS_DOCK_BOTTOM, 1, 0, 0.7)) != UMI_STATUS_OK) return s;
    return add(layout, "trader.equity-curve", UMI_WS_DOCK_BOTTOM, 1, 1, 0.8);
}

UmiStatus umi_ws_trader_layout_options(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "trader.options", "Options", UMI_WS_DOMAIN_TRADER);
    if (s != UMI_STATUS_OK) return s;
    if ((s = add(layout, "trader.underlying-chart", UMI_WS_DOCK_LEFT, 0, 0, 0.8)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "trader.option-chain", UMI_WS_DOCK_CENTRE, 0, 1, 1.8)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "trader.volatility-surface", UMI_WS_DOCK_RIGHT, 0, 2, 0.9)) != UMI_STATUS_OK) return s;
    if ((s = add(layout, "trader.strategy-builder", UMI_WS_DOCK_BOTTOM, 1, 0, 0.7)) != UMI_STATUS_OK) return s;
    return add(layout, "trader.greeks-risk", UMI_WS_DOCK_BOTTOM, 1, 1, 0.8);
}
