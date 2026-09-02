/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experiences/marketplace.c
 *
 * PURPOSE:
 *   Define the canonical Framework-owned product experience for Umicom Marketplace.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experiences/marketplace.h"

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

/*
 * Marketplace distinguishes the customer journey from catalogue management.
 * Shared product, payment, order and operational panels can be rearranged and
 * saved without duplicating marketplace business logic. */

static const UmiExperiencePanelDefinition PANELS[] = {
    { sizeof(UmiExperiencePanelDefinition), "catalogue", "Catalogue",
      "Applications/modules/content packs", "umicom.product", "marketplace.blue", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "details", "Details",
      "Product metadata and evidence", "umicom.product", "marketplace.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "compatibility", "Compatibility",
      "Framework/platform compatibility", "umicom.abi", "marketplace.blue", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "install", "Install",
      "Install transaction and dependencies", "umicom.delivery", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CRITICAL },
    { sizeof(UmiExperiencePanelDefinition), "updates", "Updates",
      "Available updates and policies", "umicom.delivery", "operations.green", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "installed", "Installed",
      "Installed product inventory", "umicom.product", "marketplace.blue", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "channels", "Channels",
      "Release channels", "umicom.delivery", "marketplace.blue", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "trust", "Trust",
      "Signatures/provenance", "umicom.security", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "history", "History",
      "Install/update/rollback history", "umicom.delivery", "operations.green", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE }
};

static const char *const LAYOUT_MARKETPLACE[] = {"catalogue", "details", "compatibility", "install", "updates"};
static const char *const LAYOUT_MANAGE[] = {"installed", "updates", "channels", "trust", "history"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "marketplace", "Marketplace",
      "Discover, verify and install Umicom content", LAYOUT_MARKETPLACE, COUNT_OF(LAYOUT_MARKETPLACE), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_RESPONSIVE },
    { sizeof(UmiExperienceLayoutDefinition), "manage", "Manage",
      "Installed products and release channels", LAYOUT_MANAGE, COUNT_OF(LAYOUT_MANAGE), UMI_EXPERIENCE_LAYOUT_LOCKABLE }
};

static const UmiExperienceFeatureDefinition FEATURES[] = {
    { sizeof(UmiExperienceFeatureDefinition), "marketplace.catalogue", "Catalogue",
      "Product/package catalogue", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.product" },
    { sizeof(UmiExperienceFeatureDefinition), "marketplace.install", "Installation",
      "Install plans and transactions", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.delivery" },
    { sizeof(UmiExperienceFeatureDefinition), "marketplace.update", "Updates",
      "Release channels/update/rollback", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.delivery" },
    { sizeof(UmiExperienceFeatureDefinition), "marketplace.trust", "Trust",
      "Signatures/provenance/compatibility", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.security" },
    { sizeof(UmiExperienceFeatureDefinition), "marketplace.ui", "Standalone UI",
      "Marketplace application", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_APPLICATION, "umicom.ui" }
};

static const UmiApplicationExperienceDefinition DEFINITION = {
    sizeof(UmiApplicationExperienceDefinition),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.marketplace",
    "Umicom Marketplace",
    "marketplace",
    PANELS,
    COUNT_OF(PANELS),
    LAYOUTS,
    COUNT_OF(LAYOUTS),
    FEATURES,
    COUNT_OF(FEATURES)
};

/*
 * Provide the application experience marketplace operation used by this module and its
 * client applications.
 */
const UmiApplicationExperienceDefinition *umi_application_experience_marketplace(void)
{
    return &DEFINITION;
}
