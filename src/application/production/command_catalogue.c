/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/command_catalogue.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/command_catalogue.h"

#include <string.h>

static const UmiApplicationProductionControlCommand COMMANDS[] = {
    { "application.production.inspect", "Inspect Production Contract",
      "Application", "Inspect canonical product bindings and readiness.", 0 },
    { "application.production.panels", "Show Panel Bindings",
      "Application", "Show panels and reusable Framework components.", 0 },
    { "application.production.layouts", "Show Layout Bindings",
      "Application", "Show projected layouts and window counts.", 0 },
    { "application.production.features", "Show Feature Gaps",
      "Application", "Show Framework, application and adapter work.", 0 },
    { "application.production.capabilities", "Probe Capabilities",
      "Application", "Refresh required capability availability.", 1 },
    { "application.production.manifest", "Compare Manifest",
      "Application", "Compare contribution data with canonical metadata.", 0 },
    { "application.production.checkpoint", "Checkpoint Workspace",
      "Workspace", "Capture a bounded recoverable workspace checkpoint.", 1 },
    { "application.production.recover", "Recover Workspace",
      "Workspace", "Restore the latest compatible workspace checkpoint.", 1 },
    { "application.production.evidence", "Show Acceptance Evidence",
      "Quality", "Inspect required and recorded production evidence.", 0 },
    { "application.production.accept", "Evaluate Acceptance",
      "Quality", "Evaluate launch acceptance without bypassing gates.", 1 },
    { "application.production.portfolio", "Show Product Portfolio",
      "Application", "Inspect readiness across all canonical products.", 0 },
    { "application.production.diagnostics", "Show Production Diagnostics",
      "Quality", "Show drift, gaps and launch blockers.", 0 }
};

/*
 * Return the number of records represented by application production control command
 * without changing their state.
 */
size_t umi_application_production_control_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

/*
 * Find application production control command while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationProductionControlCommand *
umi_application_production_control_command_at(size_t index)
{
    return index < umi_application_production_control_command_count()
        ? &COMMANDS[index] : NULL;
}

/*
 * Find application production control command while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationProductionControlCommand *
umi_application_production_control_command_find(const char *command_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (command_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_application_production_control_command_count();
         ++index)
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(COMMANDS[index].command_id, command_id) == 0)
            return &COMMANDS[index];
    return NULL;
}

