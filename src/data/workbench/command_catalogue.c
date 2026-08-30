/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/workbench/command_catalogue.c
 *
 * PURPOSE:
 *   Define stable Database/SQL/modelling workbench commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/workbench/command_catalogue.h"

#include <string.h>

#define COMMAND(identifier, title, group, detail, mutates, dangerous, connection, transaction, selection) \
    { (uint32_t)sizeof(UmiDataWorkbenchCommand), UMI_DATA_WORKBENCH_API_VERSION, \
      identifier, title, group, detail, mutates, dangerous, connection, transaction, selection }

static const UmiDataWorkbenchCommand COMMANDS[] = {
    COMMAND("data.workbench.connection.new", "New Connection...", "Database", "Create a credential-reference-only connection profile.", 1, 0, 0, 0, 0),
    COMMAND("data.workbench.connection.edit", "Edit Connection...", "Database", "Edit the selected connection profile.", 1, 0, 0, 0, 1),
    COMMAND("data.workbench.connection.activate", "Activate Connection", "Database", "Activate the selected canonical connection.", 1, 0, 0, 0, 1),
    COMMAND("data.workbench.connection.refresh", "Refresh Connections", "Database", "Refresh connection presentation state.", 0, 0, 0, 0, 0),
    COMMAND("data.workbench.navigator.refresh", "Refresh Database Navigator", "Database", "Rebuild schemas, tables, views and columns.", 0, 0, 1, 0, 0),
    COMMAND("data.workbench.navigator.filter", "Filter Database Navigator", "Database", "Filter database navigation nodes.", 0, 0, 0, 0, 0),
    COMMAND("data.workbench.sql.new", "New SQL Document", "SQL", "Open a Framework SQL editor document.", 1, 0, 0, 0, 0),
    COMMAND("data.workbench.sql.execute", "Execute Statement", "SQL", "Execute the selected or complete governed statement.", 1, 0, 1, 0, 0),
    COMMAND("data.workbench.sql.explain", "Explain Query", "SQL", "Prepare an explain operation for the active statement.", 0, 0, 1, 0, 0),
    COMMAND("data.workbench.sql.cancel", "Cancel Query", "SQL", "Cancel an active provider operation.", 1, 0, 1, 0, 0),
    COMMAND("data.workbench.transaction.begin", "Begin Transaction", "SQL", "Begin a governed Data Server transaction.", 1, 0, 1, 0, 0),
    COMMAND("data.workbench.transaction.commit", "Commit Transaction", "SQL", "Commit the active Data Server transaction.", 1, 0, 1, 1, 0),
    COMMAND("data.workbench.transaction.rollback", "Roll Back Transaction", "SQL", "Roll back the active Data Server transaction.", 1, 1, 1, 1, 0),
    COMMAND("data.workbench.results.next-page", "Next Result Page", "Results", "Request the next bounded result page.", 0, 0, 1, 0, 0),
    COMMAND("data.workbench.results.previous-page", "Previous Result Page", "Results", "Request the previous bounded result page.", 0, 0, 1, 0, 0),
    COMMAND("data.workbench.results.copy", "Copy Result Selection", "Results", "Copy selected result cells through the frontend.", 0, 0, 1, 0, 1),
    COMMAND("data.workbench.results.export", "Export Results...", "Results", "Create a reviewed export plan for query results.", 0, 0, 1, 0, 0),
    COMMAND("data.workbench.builder.open", "Open Visual Query Builder", "Query Builder", "Open the backend-neutral query-plan builder.", 0, 0, 1, 0, 0),
    COMMAND("data.workbench.builder.preview", "Preview Generated SQL", "Query Builder", "Render review-only SQL from the canonical query plan.", 0, 0, 1, 0, 0),
    COMMAND("data.workbench.schema.compare", "Compare Schemas...", "Schema", "Compare canonical schema snapshots.", 0, 0, 1, 0, 0),
    COMMAND("data.workbench.schema.generate-migration", "Generate Migration Plan...", "Schema", "Prepare a reviewed migration plan from selected changes.", 1, 0, 1, 0, 1),
    COMMAND("data.workbench.er.open", "Open ER Diagram", "Modelling", "Project canonical schema metadata into an ER diagram.", 0, 0, 1, 0, 0),
    COMMAND("data.workbench.er.auto-layout", "Auto-layout ER Diagram", "Modelling", "Apply deterministic diagram layout.", 1, 0, 1, 0, 0),
    COMMAND("data.workbench.data.edit", "Edit Table Data", "Data", "Open optimistic reviewed table-data editing.", 1, 0, 1, 0, 1),
    COMMAND("data.workbench.data.validate", "Validate Pending Edits", "Data", "Validate row revisions before mutation.", 0, 0, 1, 0, 0),
    COMMAND("data.workbench.data.apply", "Apply Pending Edits", "Data", "Apply validated edits through the Data Server adapter.", 1, 0, 1, 1, 0),
    COMMAND("data.workbench.data.discard", "Discard Pending Edits...", "Data", "Discard the reviewed edit set.", 1, 1, 1, 0, 0),
    COMMAND("data.workbench.transfer.import", "Import Data...", "Transfer", "Create a validated import plan.", 1, 0, 1, 0, 0),
    COMMAND("data.workbench.transfer.export", "Export Data...", "Transfer", "Create a validated export plan.", 0, 0, 1, 0, 0),
    COMMAND("data.workbench.transfer.dry-run", "Validate Transfer", "Transfer", "Run plan validation without mutation.", 0, 0, 1, 0, 1),
    COMMAND("data.workbench.admin.open", "Open Data Server Administration", "Administration", "Open operational health and queue evidence.", 0, 0, 1, 0, 0),
    COMMAND("data.workbench.admin.refresh", "Refresh Data Server Health", "Administration", "Refresh safe server and service snapshots.", 0, 0, 1, 0, 0),
    COMMAND("data.workbench.admin.maintenance", "Toggle Maintenance Mode", "Administration", "Request governed maintenance-mode transition.", 1, 0, 1, 0, 0),
    COMMAND("data.workbench.history.clear", "Clear Query History", "SQL", "Clear bounded local query history.", 1, 1, 1, 0, 0)
};

#undef COMMAND

size_t umi_data_workbench_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

const UmiDataWorkbenchCommand *umi_data_workbench_command_at(size_t index)
{
    return index < umi_data_workbench_command_count() ? &COMMANDS[index] : NULL;
}

const UmiDataWorkbenchCommand *umi_data_workbench_command_find(
    const char *command_id)
{
    size_t index;
    if (command_id == NULL) return NULL;
    for (index = 0U; index < umi_data_workbench_command_count(); ++index) {
        if (strcmp(COMMANDS[index].command_id, command_id) == 0) {
            return &COMMANDS[index];
        }
    }
    return NULL;
}
