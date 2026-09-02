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

/*
 * Trader offers trading, research, strategy, market and options workspaces.
 * Layout changes affect presentation only; order submission still passes
 * through explicit permissions, risk controls and execution adapters. */

static const UmiExperiencePanelDefinition PANELS[] = {
    { sizeof(UmiExperiencePanelDefinition), "watchlist", "Watchlist",
      "Canonical instruments and market evidence", "umicom.trading", "trading.red", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED | UMI_EXPERIENCE_PANEL_MULTI_INSTANCE },
    { sizeof(UmiExperiencePanelDefinition), "chart", "Chart",
      "Price, volume and analysis workspace", "umicom.chart", "trading.red", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED | UMI_EXPERIENCE_PANEL_MULTI_INSTANCE },
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
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_INSTANCE },
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
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE },
    /* Time and sales is a linked read-only tape; order actions remain in the
     * separately permissioned order-entry and price-ladder panels. */
    { sizeof(UmiExperiencePanelDefinition), "time-and-sales", "Time and Sales",
      "Sequence-checked trades, direction, price, size and filters", "umicom.trading", "trading.red", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED | UMI_EXPERIENCE_PANEL_READ_ONLY | UMI_EXPERIENCE_PANEL_MULTI_INSTANCE },
    /* Options analysis is isolated from order submission so strategy review can
     * never bypass order, permission, acknowledgement or risk controls. */
    { sizeof(UmiExperiencePanelDefinition), "options-chain", "Options Chain",
      "Expiries, strikes, calls, puts, sensitivities and strategy selection", "umicom.trading.options", "research.gold", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    /* Strategy analysis owns research requests and evidence, not live execution. */
    { sizeof(UmiExperiencePanelDefinition), "strategy-analysis", "Strategy Analysis",
      "Backtests, optimisation, walk-forward analysis and simulation evidence", "umicom.trading.research", "research.gold", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    /* Economic events and market news remain provider-neutral read-only inputs. */
    { sizeof(UmiExperiencePanelDefinition), "economic-calendar", "Economic Calendar",
      "Economic events, expected values, outcomes and linked instruments", "umicom.trading.research", "research.gold", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED | UMI_EXPERIENCE_PANEL_READ_ONLY },
    /* Alerts are defined separately from notifications so conditions, actions
     * and order permissions can be reviewed before activation. */
    { sizeof(UmiExperiencePanelDefinition), "alerts", "Alerts",
      "Price, indicator, drawing, strategy and risk alert rules", "umicom.trading", "operations.green", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    /* Price-ladder actions are explicitly critical because layout visibility
     * must never imply that one-click order permission has been granted. */
    { sizeof(UmiExperiencePanelDefinition), "price-ladder", "Price Ladder",
      "Depth ladder with guarded order and stop management", "umicom.trading", "trading.red", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED | UMI_EXPERIENCE_PANEL_CRITICAL },
    /* Trade performance is read-only evidence derived from immutable executions. */
    { sizeof(UmiExperiencePanelDefinition), "trade-performance", "Trade Performance",
      "Returns, drawdown, execution quality and strategy attribution", "umicom.trading.research", "operations.green", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_READ_ONLY },
    /* Fundamentals belong to research context and cannot grant trading authority. */
    { sizeof(UmiExperiencePanelDefinition), "fundamentals", "Fundamentals",
      "Company, instrument and economic research evidence", "umicom.trading.research", "research.gold", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED | UMI_EXPERIENCE_PANEL_READ_ONLY }
};

static const char *const LAYOUT_TRADING[] = {"watchlist", "chart", "depth", "order-entry", "blotter", "positions", "risk", "account"};
static const char *const LAYOUT_RESEARCH[] = {"watchlist", "chart", "scanner", "predictive-lab", "news", "context-inspector"};
static const char *const LAYOUT_STRATEGY_DEVELOPMENT[] = {"chart", "strategy", "replay", "predictive-lab", "output", "risk"};
/* Market analysis links discovery, chart, tape, events and fundamentals. */
static const char *const LAYOUT_MARKET_ANALYSIS[] = {"scanner", "watchlist", "chart", "time-and-sales", "economic-calendar", "fundamentals", "news"};
/* Options work keeps risk and guarded entry visible beside analysis. */
static const char *const LAYOUT_OPTIONS[] = {"watchlist", "options-chain", "chart", "order-entry", "risk", "positions"};
/* Strategy analysis separates research evidence from live execution surfaces. */
static const char *const LAYOUT_STRATEGY_ANALYSIS[] = {"strategy-analysis", "chart", "replay", "trade-performance", "output", "risk"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "trading", "Trading",
      "Execution, risk and market monitoring", LAYOUT_TRADING, COUNT_OF(LAYOUT_TRADING), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR | UMI_EXPERIENCE_LAYOUT_CONTEXT_LINKED },
    { sizeof(UmiExperienceLayoutDefinition), "research", "Research",
      "Charts, scanner, factors and predictive research", LAYOUT_RESEARCH, COUNT_OF(LAYOUT_RESEARCH), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR | UMI_EXPERIENCE_LAYOUT_CONTEXT_LINKED },
    { sizeof(UmiExperienceLayoutDefinition), "strategy-development", "Strategy Development",
      "Replay, strategy evidence and simulation", LAYOUT_STRATEGY_DEVELOPMENT, COUNT_OF(LAYOUT_STRATEGY_DEVELOPMENT), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_CONTEXT_LINKED },
    /* Users may clone this layout; the list is a starting recipe, not a limit. */
    { sizeof(UmiExperienceLayoutDefinition), "market-analysis", "Market Analysis",
      "Discovery, charts, trade tape, events, fundamentals and news", LAYOUT_MARKET_ANALYSIS, COUNT_OF(LAYOUT_MARKET_ANALYSIS), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR | UMI_EXPERIENCE_LAYOUT_CONTEXT_LINKED },
    /* Options layout keeps analysis and guarded risk controls context-linked. */
    { sizeof(UmiExperienceLayoutDefinition), "options", "Options",
      "Options analysis, chart, positions, order entry and risk", LAYOUT_OPTIONS, COUNT_OF(LAYOUT_OPTIONS), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR | UMI_EXPERIENCE_LAYOUT_CONTEXT_LINKED },
    /* Strategy analysis is simulation-first and may be placed on other monitors. */
    { sizeof(UmiExperienceLayoutDefinition), "strategy-analysis", "Strategy Analysis",
      "Backtest, optimisation, replay, performance and risk evidence", LAYOUT_STRATEGY_ANALYSIS, COUNT_OF(LAYOUT_STRATEGY_ANALYSIS), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR | UMI_EXPERIENCE_LAYOUT_CONTEXT_LINKED }
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
    { sizeof(UmiExperienceFeatureDefinition), "trader.paper-adapter", "Paper execution adapter",
      "Provider-neutral paper transport and reconciliation", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER, "umicom.trading.paper-adapter" },
    { sizeof(UmiExperienceFeatureDefinition), "trader.replay", "Replay/backtesting",
      "Deterministic replay and simulation", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.trading" },
    { sizeof(UmiExperienceFeatureDefinition), "trader.predictive", "Predictive research",
      "Features, factors, events, MFE/MAE and validation", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.trading" },
    { sizeof(UmiExperienceFeatureDefinition), "trader.live", "Guarded live execution",
      "Controlled live rollout after paper acceptance", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P3,
      UMI_EXPERIENCE_OWNER_APPLICATION, "umicom.trading" },
    /* The panel contract now exists, while a complete filtered tape projection
     * and native interaction remain Framework work. */
    { sizeof(UmiExperienceFeatureDefinition), "trader.time-and-sales", "Time and sales",
      "Sequence-checked trade tape, filtering, direction and pause state", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.trading" },
    /* Options contracts are not yet implemented in the trading engine and are
     * therefore recorded as planned despite the new layout and panel identity. */
    { sizeof(UmiExperienceFeatureDefinition), "trader.options", "Options analysis",
      "Chains, sensitivities, volatility, scenarios and reviewed strategies", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.trading.options" },
    /* Replay exists, but optimisation, walk-forward and multi-run analysis need
     * a reusable research coordinator before this becomes implemented. */
    { sizeof(UmiExperienceFeatureDefinition), "trader.strategy-analysis", "Strategy analysis",
      "Backtest, optimisation, walk-forward, simulation and evidence", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.trading.research" },
    /* Market discovery has panel foundations but needs saved/scheduled scan rules
     * and full indicator-column evaluation. */
    { sizeof(UmiExperienceFeatureDefinition), "trader.market-scanner", "Market scanner",
      "Saved screens, live ranking, filters, indicators and scheduled scans", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.trading" },
    /* External event and news providers remain adapters; no source is silently
     * assumed or bundled into the Framework. */
    { sizeof(UmiExperienceFeatureDefinition), "trader.market-research-feeds", "Market research feeds",
      "News, economic calendar, fundamentals and sentiment evidence", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER, "umicom.trading.research" },
    /* The order book and safety plane exist, but complete ladder gestures and
     * explicit one-click permission UX remain unfinished. */
    { sizeof(UmiExperienceFeatureDefinition), "trader.price-ladder", "Price ladder",
      "Depth, guarded order placement, order movement and stop management", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.trading" },
    /* Performance statistics exist as research foundations; complete report and
     * drill-down views remain to be implemented. */
    { sizeof(UmiExperienceFeatureDefinition), "trader.trade-performance", "Trade performance",
      "Returns, drawdown, execution quality and strategy attribution", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.trading.research" },
    /* Semantic layout, grouping and context links are shared and implemented;
     * native detached monitor windows still require final verification. */
    { sizeof(UmiExperienceFeatureDefinition), "trader.layout-customisation", "Trading layout customisation",
      "Dock, float, group, link, lock, save and recover trading windows", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P0,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.workbench-layout" }
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

/*
 * Provide the application experience trader operation used by this module and its client
 * applications.
 */
const UmiApplicationExperienceDefinition *umi_application_experience_trader(void)
{
    return &DEFINITION;
}
