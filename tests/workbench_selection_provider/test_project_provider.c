/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_selection_provider/test_project_provider.c
 *
 * PURPOSE:
 *   Verify project workspace selections retain project/configuration/target identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/workbench_selection_provider/project.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiProjectWorkspaceSelectionSnapshot snapshot = {0};
    UmiWorkbenchSelection selection;
    snapshot.project.struct_size = (uint32_t)sizeof(snapshot.project);
    snapshot.project.api_version = 1U;
    (void)strcpy(snapshot.project.id, "umicom");
    (void)strcpy(snapshot.project.name, "Umicom");
    (void)strcpy(snapshot.project.root_uri, "C:/dev/umicom");
    (void)strcpy(snapshot.project.primary_language, "c");
    snapshot.project.enabled = 1;
    snapshot.project.revision = 4U;
    snapshot.has_configuration = 1;
    (void)strcpy(snapshot.configuration.id, "debug");
    snapshot.has_target = 1;
    (void)strcpy(snapshot.target.id, "studio");
    assert(umi_workbench_selection_provider_from_project_selection(
        &snapshot, "org.umicom.studio", "studio.project-explorer",
        "workspace", 100U, &selection) == UMI_STATUS_OK);
    assert(selection.kind == UMI_WORKBENCH_SELECTION_PROJECT);
    assert(strcmp(selection.subject_id, "umicom") == 0);
    assert(umi_workbench_selection_find_field(
        &selection, "configuration-id") != NULL);
    assert(umi_workbench_selection_find_field(
        &selection, "target-id") != NULL);

    return 0;
}
