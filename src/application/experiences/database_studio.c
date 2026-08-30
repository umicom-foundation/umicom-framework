/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experiences/database_studio.c
 *
 * PURPOSE:
 *   Define the canonical Framework-owned product experience for Umicom Database Studio.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experiences/database_studio.h"

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

static const UmiExperiencePanelDefinition PANELS[] = {
    { sizeof(UmiExperiencePanelDefinition), "connections", "Connections",
      "Provider-neutral connections", "umicom.data", "database.blue", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "schema", "Schema",
      "Database schema explorer", "umicom.data", "database.blue", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "query", "Query Editor",
      "Query authoring", "umicom.data", "database.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR },
    { sizeof(UmiExperiencePanelDefinition), "results", "Results",
      "Virtual data grid results", "umicom.data", "database.blue", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "history", "History",
      "Query history", "umicom.data", "database.blue", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE },
    { sizeof(UmiExperiencePanelDefinition), "inspector", "Inspector",
      "Object/row metadata", "umicom.data", "database.blue", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "migrations", "Migrations",
      "Migration catalogue and execution", "umicom.data.migrations", "operations.green", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "lineage", "Lineage",
      "Data lineage", "umicom.data", "operations.green", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "backup", "Backup",
      "Backup/restore operations", "umicom.data", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CRITICAL },
    { sizeof(UmiExperiencePanelDefinition), "operations", "Operations",
      "Data Server health/operations", "umicom.operations", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE }
};

static const char *const LAYOUT_DATABASE[] = {"connections", "schema", "query", "results", "history", "inspector"};
static const char *const LAYOUT_OPERATIONS[] = {"migrations", "lineage", "backup", "operations", "results"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "database", "Database",
      "Schema, query and data operations", LAYOUT_DATABASE, COUNT_OF(LAYOUT_DATABASE), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR },
    { sizeof(UmiExperienceLayoutDefinition), "operations", "Operations",
      "Migrations, backup and lineage", LAYOUT_OPERATIONS, COUNT_OF(LAYOUT_OPERATIONS), UMI_EXPERIENCE_LAYOUT_LOCKABLE }
};

static const UmiExperienceFeatureDefinition FEATURES[] = {
    { sizeof(UmiExperienceFeatureDefinition), "database.explorer", "Database Explorer",
      "Connections/schema/query/results", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.data" },
    { sizeof(UmiExperienceFeatureDefinition), "database.migrations", "Migrations",
      "Migration catalogue and execution", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.data.migrations" },
    { sizeof(UmiExperienceFeatureDefinition), "database.transfer", "Import/export",
      "Transfer plans and data movement", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.data" },
    { sizeof(UmiExperienceFeatureDefinition), "database.backup", "Backup/restore",
      "Backup catalogue and recovery", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.data" },
    { sizeof(UmiExperienceFeatureDefinition), "database.ui", "Standalone UI",
      "Dedicated database workbench", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_APPLICATION, "umicom.ui" }
};

static const UmiApplicationExperienceDefinition DEFINITION = {
    sizeof(UmiApplicationExperienceDefinition),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.database-studio",
    "Umicom Database Studio",
    "database",
    PANELS,
    COUNT_OF(PANELS),
    LAYOUTS,
    COUNT_OF(LAYOUTS),
    FEATURES,
    COUNT_OF(FEATURES)
};

const UmiApplicationExperienceDefinition *umi_application_experience_database_studio(void)
{
    return &DEFINITION;
}
