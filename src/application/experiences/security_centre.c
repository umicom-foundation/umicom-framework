/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experiences/security_centre.c
 *
 * PURPOSE:
 *   Define the canonical Framework-owned product experience for Umicom Security Centre.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experiences/security_centre.h"

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

/*
 * Security Centre provides security operations and trust-review layouts.
 * Identity, policy, audit and secret views require explicit capabilities;
 * changing their placement never changes the underlying security decision. */

static const UmiExperiencePanelDefinition PANELS[] = {
    { sizeof(UmiExperiencePanelDefinition), "identities", "Identities",
      "Identity directory", "umicom.security", "security.red", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "roles", "Roles",
      "Roles and assignments", "umicom.security", "security.red", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "permissions", "Permissions",
      "Permission catalogue/policy", "umicom.security", "security.red", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "secrets", "Secrets",
      "Secret inventory/providers", "umicom.security", "security.red", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CRITICAL },
    { sizeof(UmiExperiencePanelDefinition), "approvals", "Approvals",
      "Security approval queue", "umicom.security", "security.red", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CRITICAL },
    { sizeof(UmiExperiencePanelDefinition), "audit", "Audit",
      "Security audit evidence", "umicom.audit", "operations.green", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "packages", "Packages",
      "Package/signature trust", "umicom.security", "security.red", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "trust", "Trust",
      "Workspace/application trust", "umicom.security", "security.red", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "incidents", "Incidents",
      "Security incidents", "umicom.operations", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CRITICAL }
};

static const char *const LAYOUT_SECURITY[] = {"identities", "roles", "permissions", "secrets", "approvals", "audit"};
static const char *const LAYOUT_TRUST[] = {"packages", "trust", "audit", "incidents"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "security", "Security",
      "Identity, policy, secrets and approvals", LAYOUT_SECURITY, COUNT_OF(LAYOUT_SECURITY), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR },
    { sizeof(UmiExperienceLayoutDefinition), "trust", "Trust",
      "Packages, trust and evidence", LAYOUT_TRUST, COUNT_OF(LAYOUT_TRUST), UMI_EXPERIENCE_LAYOUT_LOCKABLE }
};

static const UmiExperienceFeatureDefinition FEATURES[] = {
    { sizeof(UmiExperienceFeatureDefinition), "security.identity", "Identity/roles",
      "Identity, role and permission controls", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.security" },
    { sizeof(UmiExperienceFeatureDefinition), "security.secrets", "Secrets",
      "Secret providers/inventory/redaction", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.security" },
    { sizeof(UmiExperienceFeatureDefinition), "security.approvals", "Approvals",
      "Human approval gates", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.security" },
    { sizeof(UmiExperienceFeatureDefinition), "security.trust", "Trust",
      "Workspace/package trust", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.security" },
    { sizeof(UmiExperienceFeatureDefinition), "security.ui", "Standalone UI",
      "Security Centre workbench", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_APPLICATION, "umicom.ui" }
};

static const UmiApplicationExperienceDefinition DEFINITION = {
    sizeof(UmiApplicationExperienceDefinition),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.security-centre",
    "Umicom Security Centre",
    "security",
    PANELS,
    COUNT_OF(PANELS),
    LAYOUTS,
    COUNT_OF(LAYOUTS),
    FEATURES,
    COUNT_OF(FEATURES)
};

/*
 * Provide the application experience security centre operation used by this module and its
 * client applications.
 */
const UmiApplicationExperienceDefinition *umi_application_experience_security_centre(void)
{
    return &DEFINITION;
}
