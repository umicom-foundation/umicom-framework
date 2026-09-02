/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout/test_session_recovery.c
 *
 * PURPOSE:
 *   Verify session focus, open-panel state and crash-recovery planning.
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
        test_allocate_layout("layout.session");
    UmiWorkbenchLayoutSession session;
    UmiWorkbenchOpenPanel panel;
    UmiWorkbenchRecoveryJournal *journal =
        (UmiWorkbenchRecoveryJournal *)calloc(1U, sizeof(*journal));
    UmiWorkbenchRecoveryPlan plan;

    TEST_REQUIRE(document != NULL && journal != NULL, "Test state must exist");
    umi_workbench_layout_session_init(
        &session,
        "session.development",
        "developer",
        "workspace.framework");
    TEST_STATUS_OK(umi_workbench_layout_session_set_active_application(
        &session, "org.umicom.studio"));
    TEST_STATUS_OK(umi_workbench_layout_session_set_active_layout(
        &session, document->identity.layout_id));

    (void)memset(&panel, 0, sizeof(panel));
    panel.structure_size = sizeof(panel);
    test_copy_text(
        panel.instance_id,
        sizeof(panel.instance_id),
        "panel.editor.1");
    test_copy_text(panel.panel_id, sizeof(panel.panel_id), "editor");
    test_copy_text(
        panel.owner_application_id,
        sizeof(panel.owner_application_id),
        "org.umicom.studio");
    test_copy_text(panel.node_id, sizeof(panel.node_id), "editor");
    panel.visible = true;
    panel.active = true;
    panel.dirty = true;
    panel.revision = 1U;
    TEST_STATUS_OK(umi_workbench_layout_session_open_panel(&session, &panel));
    TEST_STATUS_OK(umi_workbench_layout_session_set_focus(
        &session, "editor", "panel.editor.1"));
    TEST_REQUIRE(
        umi_workbench_layout_session_dirty_panel_count(&session) == 1U,
        "Dirty panel evidence must remain in the session");

    umi_workbench_recovery_journal_init(journal);
    TEST_STATUS_OK(umi_workbench_recovery_journal_checkpoint_layout(
        journal,
        "recovery.layout",
        document,
        session.session_id,
        "correlation.recovery",
        3000U));
    TEST_STATUS_OK(umi_workbench_recovery_journal_checkpoint_session(
        journal,
        "recovery.session",
        &session,
        document->identity.layout_id,
        "correlation.recovery",
        3010U));
    TEST_STATUS_OK(umi_workbench_recovery_plan_build(
        journal, session.session_id, &plan));
    TEST_REQUIRE(plan.recovery_required, "Unclean session must require recovery");
    TEST_REQUIRE(
        strcmp(plan.layout_id, document->identity.layout_id) == 0,
        "Recovery plan must identify the last checkpointed layout");

    TEST_STATUS_OK(umi_workbench_recovery_journal_mark_clean_shutdown(
        journal,
        "recovery.clean",
        session.session_id,
        3100U));
    TEST_STATUS_OK(umi_workbench_recovery_plan_build(
        journal, session.session_id, &plan));
    TEST_REQUIRE(
        !plan.recovery_required,
        "Clean shutdown marker must close the recovery window");

    free(journal);
    free(document);
    return 0;
}
