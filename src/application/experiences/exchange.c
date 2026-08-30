/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experiences/exchange.c
 *
 * PURPOSE:
 *   Define the canonical Framework-owned product experience for Umicom Commodity Exchange.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experiences/exchange.h"

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

static const UmiExperiencePanelDefinition PANELS[] = {
    { sizeof(UmiExperiencePanelDefinition), "markets", "Markets",
      "Commodity taxonomy and filters", "umicom.commodity", "commodity.green", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "marketplace", "Physical Marketplace",
      "Listings and physical market evidence", "umicom.exchange", "commodity.green", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "matching", "Matching",
      "Eligibility, ranking and landed cost", "umicom.exchange", "commodity.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "rfq", "RFQ / Tender",
      "Buyer requirements and supplier offers", "umicom.exchange", "commodity.green", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "inventory", "Inventory",
      "Physical lots, quantities and locations", "umicom.commodity", "commodity.green", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "lot-editor", "Lot Editor",
      "Specifications, documents and commercial terms", "umicom.commodity", "commodity.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "contracts", "Deal Room",
      "Contract and trade lifecycle", "umicom.exchange", "finance.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED | UMI_EXPERIENCE_PANEL_CRITICAL },
    { sizeof(UmiExperiencePanelDefinition), "documents", "Documents",
      "Certificates, title and evidence", "umicom.audit", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE },
    { sizeof(UmiExperiencePanelDefinition), "logistics", "Logistics",
      "Shipping and service-provider marketplace", "umicom.commodity", "operations.green", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "warehouse", "Warehouse",
      "Receipts, custody, reservations and release", "umicom.commodity", "operations.green", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED | UMI_EXPERIENCE_PANEL_CRITICAL },
    { sizeof(UmiExperiencePanelDefinition), "surveillance", "Market Supervision",
      "Participants, exceptions and abuse controls", "umicom.surveillance", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CRITICAL },
    { sizeof(UmiExperiencePanelDefinition), "reconciliation", "Physical Reconciliation",
      "Ordered/reserved/shipped/received/settled control", "umicom.reconciliation", "finance.blue", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "context-inspector", "Context Inspector",
      "Linked commodity/lot/deal context", "umicom.application.federation", "commodity.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED }
};

static const char *const LAYOUT_MARKETPLACE[] = {"markets", "marketplace", "matching", "rfq", "context-inspector"};
static const char *const LAYOUT_SUPPLIER[] = {"inventory", "lot-editor", "rfq", "contracts", "documents"};
static const char *const LAYOUT_OPERATIONS[] = {"logistics", "warehouse", "contracts", "surveillance", "reconciliation"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "marketplace", "Marketplace",
      "Physical commodity discovery and matching", LAYOUT_MARKETPLACE, COUNT_OF(LAYOUT_MARKETPLACE), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR },
    { sizeof(UmiExperienceLayoutDefinition), "supplier", "Supplier",
      "Inventory lots, offers and buyer enquiries", LAYOUT_SUPPLIER, COUNT_OF(LAYOUT_SUPPLIER), UMI_EXPERIENCE_LAYOUT_LOCKABLE },
    { sizeof(UmiExperienceLayoutDefinition), "operations", "Operations",
      "Delivery, warehouse and market supervision", LAYOUT_OPERATIONS, COUNT_OF(LAYOUT_OPERATIONS), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR }
};

static const UmiExperienceFeatureDefinition FEATURES[] = {
    { sizeof(UmiExperienceFeatureDefinition), "exchange.commodity", "Commodity/lot model",
      "Specifications, inventory and warehouse state", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.commodity" },
    { sizeof(UmiExperienceFeatureDefinition), "exchange.matching", "Exchange matching",
      "Eligibility, ranking and allocation", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.exchange" },
    { sizeof(UmiExperienceFeatureDefinition), "exchange.rfq", "RFQ/tenders",
      "Bespoke physical sourcing workflows", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.exchange" },
    { sizeof(UmiExperienceFeatureDefinition), "exchange.auctions", "Auctions",
      "Seller and reverse auctions", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.exchange" },
    { sizeof(UmiExperienceFeatureDefinition), "exchange.delivery", "Physical delivery",
      "Logistics, warehouse and receipt lifecycle", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.commodity" },
    { sizeof(UmiExperienceFeatureDefinition), "exchange.settlement", "Settlement/accounting",
      "Financial settlement and reconciliation", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.accounting" },
    { sizeof(UmiExperienceFeatureDefinition), "exchange.ui", "Marketplace clients",
      "Buyer/supplier/provider/operations UI", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_APPLICATION, "umicom.ui" },
    { sizeof(UmiExperienceFeatureDefinition), "exchange.surveillance", "Surveillance",
      "Inventory/price/participant abuse controls", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.surveillance" }
};

static const UmiApplicationExperienceDefinition DEFINITION = {
    sizeof(UmiApplicationExperienceDefinition),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.exchange",
    "Umicom Commodity Exchange",
    "marketplace",
    PANELS,
    COUNT_OF(PANELS),
    LAYOUTS,
    COUNT_OF(LAYOUTS),
    FEATURES,
    COUNT_OF(FEATURES)
};

const UmiApplicationExperienceDefinition *umi_application_experience_exchange(void)
{
    return &DEFINITION;
}
