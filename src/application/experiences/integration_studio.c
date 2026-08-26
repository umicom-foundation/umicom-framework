/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experiences/integration_studio.c
 *
 * PURPOSE:
 *   Define the canonical Framework-owned product experience for Umicom Integration Studio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experiences/integration_studio.h"

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

static const UmiExperiencePanelDefinition PANELS[] = {
    { sizeof(UmiExperiencePanelDefinition), "connections", "Connections",
      "Integration connections/providers", "umicom.integration", "integration.teal", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "api", "API Contracts",
      "API and schema contracts", "umicom.integration", "integration.teal", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "mapping", "Mapping",
      "Field/data mapping designer", "umicom.integration", "integration.teal", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "workflow", "Workflow",
      "Integration workflow designer", "umicom.integration", "integration.teal", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR },
    { sizeof(UmiExperiencePanelDefinition), "inspector", "Inspector",
      "Step/connection properties", "umicom.integration", "integration.teal", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "monitoring", "Monitoring",
      "Integration health and throughput", "umicom.operations", "operations.green", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "runs", "Runs",
      "Execution history", "umicom.integration", "operations.green", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "messages", "Messages",
      "Message/event inspection", "umicom.messaging", "operations.green", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "dead-letter", "Dead Letter",
      "Failed/unhandled messages", "umicom.messaging", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CRITICAL },
    { sizeof(UmiExperiencePanelDefinition), "operations", "Operations",
      "Retry/recovery/evidence", "umicom.resilience", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE }
};

static const char *const LAYOUT_DESIGN[] = {"connections", "api", "mapping", "workflow", "inspector"};
static const char *const LAYOUT_OPERATE[] = {"monitoring", "runs", "messages", "dead-letter", "operations"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "design", "Design",
      "Connections, mappings and workflows", LAYOUT_DESIGN, COUNT_OF(LAYOUT_DESIGN), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR },
    { sizeof(UmiExperienceLayoutDefinition), "operate", "Operate",
      "Executions, monitoring and failures", LAYOUT_OPERATE, COUNT_OF(LAYOUT_OPERATE), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR }
};

static const UmiExperienceFeatureDefinition FEATURES[] = {
    { sizeof(UmiExperienceFeatureDefinition), "integration.connections", "Connections",
      "Provider-neutral connection catalogue", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.integration" },
    { sizeof(UmiExperienceFeatureDefinition), "integration.mapping", "Mapping",
      "Schema and field mappings", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.integration" },
    { sizeof(UmiExperienceFeatureDefinition), "integration.workflow", "Workflow",
      "Integration workflow/simulation", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.integration" },
    { sizeof(UmiExperienceFeatureDefinition), "integration.monitoring", "Monitoring",
      "Execution history, health and failures", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.operations" },
    { sizeof(UmiExperienceFeatureDefinition), "integration.ui", "Standalone UI",
      "Dedicated Integration Studio", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_APPLICATION, "umicom.ui" }
};

static const UmiApplicationExperienceDefinition DEFINITION = {
    sizeof(UmiApplicationExperienceDefinition),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.integration-studio",
    "Umicom Integration Studio",
    "design",
    PANELS,
    COUNT_OF(PANELS),
    LAYOUTS,
    COUNT_OF(LAYOUTS),
    FEATURES,
    COUNT_OF(FEATURES)
};

const UmiApplicationExperienceDefinition *umi_application_experience_integration_studio(void)
{
    return &DEFINITION;
}
