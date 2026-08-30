/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experiences/operations.c
 *
 * PURPOSE:
 *   Define the canonical Framework-owned product experience for Umicom Operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experiences/operations.h"

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

static const UmiExperiencePanelDefinition PANELS[] = {
    { sizeof(UmiExperiencePanelDefinition), "fleet", "Fleet",
      "Application/service fleet", "umicom.operations", "operations.green", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "health", "Health",
      "Readiness and health summary", "umicom.operations", "operations.green", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CRITICAL },
    { sizeof(UmiExperiencePanelDefinition), "metrics", "Metrics",
      "Resource/service metrics", "umicom.metrics", "operations.green", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "logs", "Logs",
      "Structured logs", "umicom.diagnostics", "operations.green", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "traces", "Traces",
      "Correlated traces", "umicom.tracing", "operations.green", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "incidents", "Incidents",
      "Incident evidence and actions", "umicom.operations", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CRITICAL },
    { sizeof(UmiExperiencePanelDefinition), "queues", "Queues",
      "Task/message queue health", "umicom.tasks", "operations.green", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "retries", "Retries",
      "Retry/circuit/resilience state", "umicom.resilience", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "crashes", "Crashes",
      "Crash registry and reports", "umicom.diagnostics", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE }
};

static const char *const LAYOUT_OPERATIONS[] = {"fleet", "health", "metrics", "logs", "traces", "incidents"};
static const char *const LAYOUT_RESILIENCE[] = {"health", "queues", "retries", "crashes", "incidents"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "operations", "Operations",
      "Fleet/service health and incidents", LAYOUT_OPERATIONS, COUNT_OF(LAYOUT_OPERATIONS), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR },
    { sizeof(UmiExperienceLayoutDefinition), "resilience", "Resilience",
      "Queues, retries, failures and recovery", LAYOUT_RESILIENCE, COUNT_OF(LAYOUT_RESILIENCE), UMI_EXPERIENCE_LAYOUT_LOCKABLE }
};

static const UmiExperienceFeatureDefinition FEATURES[] = {
    { sizeof(UmiExperienceFeatureDefinition), "operations.health", "Health/readiness",
      "Service/fleet health aggregation", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.operations" },
    { sizeof(UmiExperienceFeatureDefinition), "operations.observability", "Observability",
      "Metrics/logs/traces/correlation", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.metrics" },
    { sizeof(UmiExperienceFeatureDefinition), "operations.resilience", "Resilience",
      "Retry/circuit/bulkhead/recovery", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.resilience" },
    { sizeof(UmiExperienceFeatureDefinition), "operations.incidents", "Incidents",
      "Incident workflow/evidence", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.operations" },
    { sizeof(UmiExperienceFeatureDefinition), "operations.ui", "Standalone UI",
      "Operations workbench", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_APPLICATION, "umicom.ui" }
};

static const UmiApplicationExperienceDefinition DEFINITION = {
    sizeof(UmiApplicationExperienceDefinition),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.operations",
    "Umicom Operations",
    "operations",
    PANELS,
    COUNT_OF(PANELS),
    LAYOUTS,
    COUNT_OF(LAYOUTS),
    FEATURES,
    COUNT_OF(FEATURES)
};

const UmiApplicationExperienceDefinition *umi_application_experience_operations(void)
{
    return &DEFINITION;
}
