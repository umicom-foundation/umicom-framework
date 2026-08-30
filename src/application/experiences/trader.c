/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experiences/trader.c
 *
 * PURPOSE:
 *   Define the canonical Framework-owned product experience for Umicom Trader.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experiences/trader.h"

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

static const UmiExperiencePanelDefinition PANELS[] = {
    { sizeof(UmiExperiencePanelDefinition), "watchlist", "Watchlist",
      "Canonical instruments and market evidence", "umicom.trading", "trading.red", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "chart", "Chart",
      "Price, volume and analysis workspace", "umicom.chart", "trading.red", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "depth", "Market Depth",
      "Bid/ask ladder and liquidity", "umicom.trading", "trading.red", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED | UMI_EXPERIENCE_PANEL_CRITICAL },
    { sizeof(UmiExperiencePanelDefinition), "order-entry", "Order Entry",
      "Validated order ticket", "umicom.trading", "trading.red", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED | UMI_EXPERIENCE_PANEL_CRITICAL },
    { sizeof(UmiExperiencePanelDefinition), "blotter", "Trade Blotter",
      "Orders and executions", "umicom.trading", "trading.red", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "positions", "Positions & P&L",
      "Portfolio and P&L", "umicom.trading", "operations.green", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "risk", "Risk",
      "Limits, readiness and kill switch", "umicom.trading", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CRITICAL },
    { sizeof(UmiExperiencePanelDefinition), "account", "Account",
      "Account balances and buying power", "umicom.trading", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "scanner", "Market Scanner",
      "Cross-market opportunity scanning", "umicom.trading", "trading.red", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "predictive-lab", "Predictive Research Lab",
      "Factor attribution and movement-event analysis", "umicom.trading", "research.gold", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "news", "News",
      "Linked market news", "umicom.trading", "trading.red", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "context-inspector", "Context Inspector",
      "Linked instrument/account/order context", "umicom.application.federation", "trading.red", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "strategy", "Strategy",
      "Strategy plan, evidence and controls", "umicom.trading", "research.gold", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "replay", "Replay",
      "Deterministic market replay controls", "umicom.trading", "research.gold", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "output", "Research Output",
      "Backtest and evaluation evidence", "umicom.diagnostics", "operations.green", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE }
};

static const char *const LAYOUT_TRADING[] = {"watchlist", "chart", "depth", "order-entry", "blotter", "positions", "risk", "account"};
static const char *const LAYOUT_RESEARCH[] = {"watchlist", "chart", "scanner", "predictive-lab", "news", "context-inspector"};
static const char *const LAYOUT_STRATEGY_DEVELOPMENT[] = {"chart", "strategy", "replay", "predictive-lab", "output", "risk"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "trading", "Trading",
      "Execution, risk and market monitoring", LAYOUT_TRADING, COUNT_OF(LAYOUT_TRADING), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR | UMI_EXPERIENCE_LAYOUT_CONTEXT_LINKED },
    { sizeof(UmiExperienceLayoutDefinition), "research", "Research",
      "Charts, scanner, factors and predictive research", LAYOUT_RESEARCH, COUNT_OF(LAYOUT_RESEARCH), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR | UMI_EXPERIENCE_LAYOUT_CONTEXT_LINKED },
    { sizeof(UmiExperienceLayoutDefinition), "strategy-development", "Strategy Development",
      "Replay, strategy evidence and simulation", LAYOUT_STRATEGY_DEVELOPMENT, COUNT_OF(LAYOUT_STRATEGY_DEVELOPMENT), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_CONTEXT_LINKED }
};

static const UmiExperienceFeatureDefinition FEATURES[] = {
    { sizeof(UmiExperienceFeatureDefinition), "trader.market-data", "Market data workspace",
      "Quotes, bars, depth and sessions", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.trading" },
    { sizeof(UmiExperienceFeatureDefinition), "trader.oms", "OMS and executions",
      "Order lifecycle and execution state", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.trading" },
    { sizeof(UmiExperienceFeatureDefinition), "trader.risk", "Risk and kill switch",
      "Pre-trade risk, limits and emergency controls", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.trading" },
    { sizeof(UmiExperienceFeatureDefinition), "trader.ui", "Desktop trading workstation",
      "GTK multi-monitor rendering of Framework models", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_APPLICATION, "umicom.ui" },
    { sizeof(UmiExperienceFeatureDefinition), "trader.ibkr-paper", "IBKR paper adapter",
      "Concrete TWS API transport and reconciliation", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER, "umicom.trading.ibkr" },
    { sizeof(UmiExperienceFeatureDefinition), "trader.replay", "Replay/backtesting",
      "Deterministic replay and simulation", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.trading" },
    { sizeof(UmiExperienceFeatureDefinition), "trader.predictive", "Predictive research",
      "Features, factors, events, MFE/MAE and validation", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.trading" },
    { sizeof(UmiExperienceFeatureDefinition), "trader.live", "Guarded live execution",
      "Controlled live rollout after paper acceptance", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P3,
      UMI_EXPERIENCE_OWNER_APPLICATION, "umicom.trading" }
};

static const UmiApplicationExperienceDefinition DEFINITION = {
    sizeof(UmiApplicationExperienceDefinition),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.trader",
    "Umicom Trader",
    "trading",
    PANELS,
    COUNT_OF(PANELS),
    LAYOUTS,
    COUNT_OF(LAYOUTS),
    FEATURES,
    COUNT_OF(FEATURES)
};

const UmiApplicationExperienceDefinition *umi_application_experience_trader(void)
{
    return &DEFINITION;
}
