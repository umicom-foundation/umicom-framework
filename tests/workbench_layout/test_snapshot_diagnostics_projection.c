/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout/test_snapshot_diagnostics_projection.c
 *
 * PURPOSE:
 *   Verify immutable frontend snapshots, health evidence and compatibility
 *   projection to the established flat Desktop layout contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchLayoutDocument *document =
        test_allocate_layout("layout.snapshot");
    UmiWorkbenchLayoutSnapshot *snapshot =
        (UmiWorkbenchLayoutSnapshot *)calloc(1U, sizeof(*snapshot));
    UmiWorkbenchLayoutHealth health;
    UmiWorkbenchLayoutValidationReport validation;
    UmiWorkbenchLayoutSession session;
    UmiWorkbenchLayoutProjectionOptions projection_options;
    UmiWorkbenchLayoutProjectionReport projection_report;
    UmiDesktopLayout desktop_layout;
    UmiWorkbenchLayoutDocument *imported =
        (UmiWorkbenchLayoutDocument *)calloc(1U, sizeof(*imported));
    UmiWorkbenchLayoutIdentity identity =
        test_layout_identity("layout.imported-desktop");
    const UmiWorkbenchLayoutNodeSnapshot *editor;

    TEST_REQUIRE(
        document != NULL && snapshot != NULL && imported != NULL,
        "Projection test state must be allocated");
    umi_workbench_layout_session_init(
        &session,
        "session.snapshot",
        "developer",
        "workspace.framework");
    TEST_STATUS_OK(umi_workbench_layout_session_set_active_layout(
        &session, document->identity.layout_id));
    TEST_STATUS_OK(umi_workbench_layout_session_set_focus(
        &session, "editor", ""));

    TEST_STATUS_OK(umi_workbench_layout_snapshot_build(
        document, &session, snapshot));
    TEST_REQUIRE(
        snapshot->node_count == document->node_count,
        "Snapshot must project every semantic node");
    TEST_REQUIRE(snapshot->panel_count == 4U, "Snapshot must count leaf panels");
    editor = umi_workbench_layout_snapshot_find(snapshot, "editor");
    TEST_REQUIRE(editor != NULL && editor->focused, "Focused editor must be marked");

    TEST_STATUS_OK(umi_workbench_layout_diagnose(
        document, NULL, NULL, &health, &validation));
    TEST_REQUIRE(health.valid, "Healthy layout must pass diagnostics");
    TEST_REQUIRE(health.maximum_depth >= 2U, "Health must measure tree depth");
    TEST_REQUIRE(
        health.application_owner_count == 1U,
        "One application owner must be identified");

    projection_options = umi_workbench_layout_projection_options_default();
    projection_options.include_container_windows = false;
    projection_options.include_hidden_nodes = false;
    TEST_STATUS_OK(umi_workbench_layout_project_desktop(
        document,
        &projection_options,
        &desktop_layout,
        &projection_report));
    TEST_REQUIRE(
        projection_report.projected_window_count == 4U,
        "Flat Desktop projection must expose four leaf panel windows");
    TEST_REQUIRE(projection_report.complete, "Projection must not truncate");

    TEST_STATUS_OK(umi_workbench_layout_import_desktop(
        &desktop_layout, &identity, imported, &projection_report));
    TEST_REQUIRE(
        imported->node_count >= desktop_layout.window_count,
        "Desktop import must create semantic nodes for every window");
    TEST_REQUIRE(
        strcmp(imported->identity.layout_id, "layout.imported-desktop") == 0,
        "Desktop import must use explicit target identity");

    free(imported);
    free(snapshot);
    free(document);
    return 0;
}
