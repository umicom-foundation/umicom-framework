/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experiences/bank.c
 *
 * PURPOSE:
 *   Define the canonical Framework-owned product experience for Umicom Bank.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experiences/bank.h"

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

/*
 * Bank provides everyday banking, global-money and digital-asset views.
 * Layouts never own balances or payment logic; they present secure Framework
 * services and can be customised without changing financial records. */

static const UmiExperiencePanelDefinition PANELS[] = {
    { sizeof(UmiExperiencePanelDefinition), "overview", "Money Overview",
      "Balances, cash flow and quick actions", "umicom.banking", "bank.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "accounts", "Accounts",
      "Fiat and savings account balances", "umicom.banking", "bank.blue", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "payments", "Payments",
      "Domestic/international transfers", "umicom.payments", "bank.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED | UMI_EXPERIENCE_PANEL_CRITICAL },
    { sizeof(UmiExperiencePanelDefinition), "cards", "Cards",
      "Physical/virtual cards and controls", "umicom.banking", "bank.blue", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CRITICAL },
    { sizeof(UmiExperiencePanelDefinition), "fx", "FX & Transfers",
      "Quotes, fees, recipient value and routing", "umicom.payments", "bank.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "beneficiaries", "Beneficiaries",
      "Recipients and trusted payment relationships", "umicom.payments", "bank.blue", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "external-accounts", "Connected Banks",
      "Open Banking balances and transactions", "umicom.integration", "bank.blue", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "crypto", "Crypto Vault",
      "Custodial and linked digital assets", "umicom.digital-asset", "asset.violet", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED | UMI_EXPERIENCE_PANEL_CRITICAL },
    { sizeof(UmiExperiencePanelDefinition), "vaulted-assets", "eGold / eSilver",
      "Vaulted physical asset allocations", "umicom.digital-asset", "asset.gold", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "umicoin", "UmiCoin",
      "Reserve/NAV/security-register view", "umicom.digital-asset", "asset.gold", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CRITICAL },
    { sizeof(UmiExperiencePanelDefinition), "insights", "Insights",
      "Budgeting, categories and forecasts", "umicom.analytics", "bank.blue", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE },
    { sizeof(UmiExperiencePanelDefinition), "security", "Security",
      "Devices, fraud and approval controls", "umicom.security", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CRITICAL },
    { sizeof(UmiExperiencePanelDefinition), "audit", "Reserve & Audit",
      "Reconciliation and reserve evidence", "umicom.audit", "operations.green", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE }
};

static const char *const LAYOUT_BANKING[] = {"overview", "accounts", "payments", "cards", "insights", "security"};
static const char *const LAYOUT_GLOBAL_MONEY[] = {"accounts", "fx", "payments", "beneficiaries", "external-accounts"};
static const char *const LAYOUT_DIGITAL_ASSETS[] = {"crypto", "vaulted-assets", "umicoin", "security", "audit"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "banking", "Everyday Banking",
      "Accounts, payments, cards and cash flow", LAYOUT_BANKING, COUNT_OF(LAYOUT_BANKING), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_RESPONSIVE },
    { sizeof(UmiExperienceLayoutDefinition), "global-money", "Global Money",
      "Multi-currency, FX and international transfers", LAYOUT_GLOBAL_MONEY, COUNT_OF(LAYOUT_GLOBAL_MONEY), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_RESPONSIVE },
    { sizeof(UmiExperienceLayoutDefinition), "digital-assets", "Digital Assets",
      "Crypto, vaulted assets and reserve evidence", LAYOUT_DIGITAL_ASSETS, COUNT_OF(LAYOUT_DIGITAL_ASSETS), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_RESPONSIVE }
};

static const UmiExperienceFeatureDefinition FEATURES[] = {
    { sizeof(UmiExperienceFeatureDefinition), "bank.accounts", "Banking accounts",
      "Current/savings and multi-currency account core", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.banking" },
    { sizeof(UmiExperienceFeatureDefinition), "bank.payments", "Payments",
      "Domestic/international payment lifecycle", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.payments" },
    { sizeof(UmiExperienceFeatureDefinition), "bank.accounting", "Shared ledger",
      "Accounting and reconciliation core", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.accounting" },
    { sizeof(UmiExperienceFeatureDefinition), "bank.ui", "Web/desktop/mobile composition",
      "Consumer-facing application clients", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_APPLICATION, "umicom.ui" },
    { sizeof(UmiExperienceFeatureDefinition), "bank.cards", "Card issuing boundary",
      "Physical/virtual card provider abstraction", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER, "umicom.banking.cards" },
    { sizeof(UmiExperienceFeatureDefinition), "bank.open-banking", "Open Banking",
      "External bank aggregation/initiation adapters", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER, "umicom.integration" },
    { sizeof(UmiExperienceFeatureDefinition), "bank.crypto", "Crypto custody",
      "Digital asset custody and network adapters", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.digital-asset" },
    { sizeof(UmiExperienceFeatureDefinition), "bank.metals", "eGold/eSilver",
      "Vaulted asset allocation and redemption", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.digital-asset" },
    { sizeof(UmiExperienceFeatureDefinition), "bank.umicoin", "UmiCoin",
      "Reserve/NAV/security product workflow", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P3,
      UMI_EXPERIENCE_OWNER_APPLICATION, "umicom.digital-asset" },
    { sizeof(UmiExperienceFeatureDefinition), "bank.compliance", "KYC/AML/fraud",
      "Regulated controls and evidence", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.security" }
};

static const UmiApplicationExperienceDefinition DEFINITION = {
    sizeof(UmiApplicationExperienceDefinition),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.bank",
    "Umicom Bank",
    "banking",
    PANELS,
    COUNT_OF(PANELS),
    LAYOUTS,
    COUNT_OF(LAYOUTS),
    FEATURES,
    COUNT_OF(FEATURES)
};

/*
 * Provide the application experience bank operation used by this module and its client
 * applications.
 */
const UmiApplicationExperienceDefinition *umi_application_experience_bank(void)
{
    return &DEFINITION;
}
