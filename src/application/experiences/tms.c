/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experiences/tms.c
 *
 * PURPOSE:
 *   Define the canonical Framework-owned product experience for Umicom TMS.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experiences/tms.h"

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

/*
 * TMS separates front-office, middle-office and back-office responsibilities.
 * Trade, risk, settlement and accounting panels share governed treasury
 * services while each role starts with the information it needs. */

static const UmiExperiencePanelDefinition PANELS[] = {
    { sizeof(UmiExperiencePanelDefinition), "trade-capture", "Trade Capture",
      "Deal entry and lifecycle", "umicom.tms", "trading.red", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "market-data", "Market Data",
      "Quotes, curves and pricing environments", "umicom.market-data", "trading.red", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "positions", "Positions",
      "Position and portfolio evidence", "umicom.position", "trading.red", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "pricing", "Pricing",
      "NPV, cashflows and valuation evidence", "umicom.valuation", "trading.red", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "risk", "Risk",
      "Sensitivities, scenarios and exposures", "umicom.risk", "risk.amber", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "pnl", "P&L",
      "P&L and explain", "umicom.risk", "risk.amber", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "limits", "Limits",
      "Limit usage and breaches", "umicom.risk", "risk.amber", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CRITICAL },
    { sizeof(UmiExperiencePanelDefinition), "collateral", "Collateral",
      "Calls, inventory and eligibility", "umicom.collateral", "operations.green", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "task-station", "Task Station",
      "Workflow exceptions and approvals", "umicom.workflow", "operations.green", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED | UMI_EXPERIENCE_PANEL_CRITICAL },
    { sizeof(UmiExperiencePanelDefinition), "settlement", "Settlement",
      "Transfers and settlement lifecycle", "umicom.settlement", "operations.green", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED | UMI_EXPERIENCE_PANEL_CRITICAL },
    { sizeof(UmiExperiencePanelDefinition), "messaging", "Messages",
      "Confirmations and external messages", "umicom.integration", "operations.green", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "accounting", "Accounting",
      "Events, journals and ledgers", "umicom.accounting", "finance.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "reconciliation", "Reconciliation",
      "Cash, positions, settlement and GL control", "umicom.reconciliation", "finance.blue", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "engine-ops", "Engine Operations",
      "Servers, engines, queues and schedules", "umicom.operations", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "context-inspector", "Context Inspector",
      "Linked trade/account/context", "umicom.application.federation", "trading.red", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED }
};

static const char *const LAYOUT_FRONT_OFFICE[] = {"trade-capture", "market-data", "positions", "pricing", "context-inspector"};
static const char *const LAYOUT_MIDDLE_OFFICE[] = {"risk", "pnl", "limits", "collateral", "task-station"};
static const char *const LAYOUT_BACK_OFFICE[] = {"task-station", "settlement", "messaging", "accounting", "reconciliation", "engine-ops"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "front-office", "Front Office",
      "Trade capture, pricing and positions", LAYOUT_FRONT_OFFICE, COUNT_OF(LAYOUT_FRONT_OFFICE), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR },
    { sizeof(UmiExperienceLayoutDefinition), "middle-office", "Middle Office",
      "Risk, P&L, limits and collateral", LAYOUT_MIDDLE_OFFICE, COUNT_OF(LAYOUT_MIDDLE_OFFICE), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR },
    { sizeof(UmiExperienceLayoutDefinition), "back-office", "Back Office",
      "Settlement, messaging, accounting and exceptions", LAYOUT_BACK_OFFICE, COUNT_OF(LAYOUT_BACK_OFFICE), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR }
};

static const UmiExperienceFeatureDefinition FEATURES[] = {
    { sizeof(UmiExperienceFeatureDefinition), "tms.trade", "Trade lifecycle",
      "Canonical trade capture and lifecycle", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.tms" },
    { sizeof(UmiExperienceFeatureDefinition), "tms.risk", "Risk/valuation",
      "Valuation grid, risk and limits", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.risk" },
    { sizeof(UmiExperienceFeatureDefinition), "tms.workflow", "Task Station",
      "Workflow and exception processing", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.workflow" },
    { sizeof(UmiExperienceFeatureDefinition), "tms.settlement", "Settlement",
      "Transfers, confirmations and settlement", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.settlement" },
    { sizeof(UmiExperienceFeatureDefinition), "tms.collateral", "Collateral",
      "Margin/collateral workflows", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.collateral" },
    { sizeof(UmiExperienceFeatureDefinition), "tms.accounting", "Accounting",
      "Event-driven accounting and reconciliation", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.accounting" },
    { sizeof(UmiExperienceFeatureDefinition), "tms.ui", "GTK/Web workbench",
      "Front/Middle/Back Office product clients", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_APPLICATION, "umicom.ui" },
    { sizeof(UmiExperienceFeatureDefinition), "tms.operations", "Engine operations",
      "Server/engine/scheduler operational evidence", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.operations" }
};

static const UmiApplicationExperienceDefinition DEFINITION = {
    sizeof(UmiApplicationExperienceDefinition),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.tms",
    "Umicom TMS",
    "front-office",
    PANELS,
    COUNT_OF(PANELS),
    LAYOUTS,
    COUNT_OF(LAYOUTS),
    FEATURES,
    COUNT_OF(FEATURES)
};

/*
 * Provide the application experience tms operation used by this module and its client
 * applications.
 */
const UmiApplicationExperienceDefinition *umi_application_experience_tms(void)
{
    return &DEFINITION;
}
