/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_completion/test_report_native.c
 *
 * PURPOSE:
 *   Verify that accepted report bytes inform a draft without completing cards or approving stale evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "native_fixture.h"
#include "umicom/ui/gtk4/ctest_report.h"
typedef struct ReportContext {
    UmiKanbanBoard *board;
    unsigned applies;
} ReportContext;
static UmiStatus CaptureReview(gpointer data, const UmiKanbanCompletionReview *review,
    UmiKanbanCompletionPlan **out)
{
    ReportContext *c = data;
    UmiKanbanCompletionRecord record = {0};
    strcpy(record.card_id, "alpha"); record.decision = review->decision;
    memcpy(record.rationale, review->rationale, sizeof record.rationale);
    memcpy(record.evidence_sha256, review->evidence_sha256, sizeof record.evidence_sha256);
    return UmiKanbanCompletionPlanCreate(c->board, umi_kanban_board_revision(c->board),
        "done", review->reviewer, review->source_identity, review->configuration, &record, 1U, out);
}
static UmiStatus ApplyReview(gpointer data, const UmiKanbanCompletionPlan *plan)
{
    ReportContext *c = data; ++c->applies;
    return UmiKanbanCompletionPlanApply(plan, c->board, umi_kanban_board_revision(c->board));
}
static GtkWidget *Tagged(GtkWidget *root, const char *tag)
{
    GtkWidget *widget = umi_gtk4_automation_find_tagged_widget(root, tag);
    REQUIRE(widget); return widget;
}
static GtkWidget *Named(GtkWidget *root, const char *name)
{
    if (!strcmp(gtk_widget_get_name(root), name)) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Named(child, name);
        if (found) return found;
    }
    return NULL;
}
static void ReportWait(GtkWidget *root)
{
    gint64 end = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
    while (UmiGtk4CtestReportBusy(root) && g_get_monotonic_time() < end) {
        (void)UmiTestDispatchSlice(NULL, end, 32U); g_usleep(1000U);
    }
    REQUIRE(!UmiGtk4CtestReportBusy(root)); NativeDrain();
}
int main(int argc, char **argv)
{
    const char *const cases[] = {"normal", "mixed", "empty-suite", "no-report", "report-busy",
        "file-change", "draft-only", "approval-revoked", "hidden", "retired", "source-fields"};
    REQUIRE(argc == 2 && Known(argv[1], cases, sizeof cases / sizeof cases[0]));
    if (!gtk_init_check()) return 77;
    const char *name = argv[1];
    const char passed[] = "<testsuite tests='1'><testcase name='alpha' status='run' time='0'/></testsuite>";
    const char failed[] = "<testsuite tests='1' failures='1'><testcase name='alpha' status='fail' time='0'><failure message='Failed'/></testcase></testsuite>";
    const char *xml = !strcmp(name, "mixed") ? failed :
        !strcmp(name, "empty-suite") ? "<testsuite tests='0'/>" : passed;
    char directory[UMI_PATH_CAPACITY], file[UMI_PATH_CAPACITY], report_path[UMI_PATH_CAPACITY];
    FixtureDirectory(directory); FixturePath(file, directory, "review.umicompletion");
    FixturePath(report_path, directory, "report.xml");
    UmiOutputFile *output = NULL;
    REQUIRE(UmiOutputFileCreate(report_path, &output) == UMI_STATUS_OK);
    REQUIRE(UmiOutputFileWrite(output, xml, strlen(xml)) == UMI_STATUS_OK);
    REQUIRE(UmiOutputFileClose(output) == UMI_STATUS_OK); UmiOutputFileDestroy(output);
    ReportContext context = {Ready(), 0U};
    GtkWidget *root = UmiKanbanCompletionGtk4Create(CaptureReview, ApplyReview, &context, NULL);
    REQUIRE(root); g_object_ref_sink(root);
    GtkWidget *expander = Named(root, "kanban-completion-report");
    REQUIRE(GTK_IS_EXPANDER(expander)); gtk_expander_set_expanded(GTK_EXPANDER(expander), TRUE);
    /* The shared saved-report constructor publishes ctest.report.panel.
     * Preserve the historical selector while checking the current root ID. */
#if 0
    GtkWidget *report = Tagged(root, "ctest.report"); g_object_ref(report);
#endif
    GtkWidget *report = Tagged(root, "ctest.report.panel"); g_object_ref(report);
    GtkWindow *window = GTK_WINDOW(gtk_window_new());
    gtk_window_set_child(window, root); gtk_window_present(window); NativeDrain();
    FillReview(root, file);
    if (!strcmp(name, "approval-revoked")) {
        ClickReview(root, "save"); NativeWait(root); Approve(root);
    }
    if (strcmp(name, "no-report")) {
        gtk_editable_set_text(GTK_EDITABLE(Tagged(report, "ctest.report.path")), report_path);
        g_signal_emit_by_name(Tagged(report, "ctest.report.open"), "clicked");
        if (strcmp(name, "report-busy")) ReportWait(report);
    }
    if (!strcmp(name, "file-change")) REQUIRE(g_file_set_contents(report_path, failed, -1, NULL));
    if (!strcmp(name, "hidden")) gtk_widget_set_visible(root, FALSE);
    if (!strcmp(name, "retired")) UmiKanbanCompletionGtk4Retire(root);
    char before[65]; g_strlcpy(before, gtk_editable_get_text(GTK_EDITABLE(Control(root, "digest"))), sizeof before);
    ClickReview(root, "use-report");
    bool copied = strcmp(name, "no-report") && strcmp(name, "report-busy") &&
                  strcmp(name, "hidden") && strcmp(name, "retired");
    char *digest = g_compute_checksum_for_string(G_CHECKSUM_SHA256, xml, -1);
    REQUIRE(!strcmp(gtk_editable_get_text(GTK_EDITABLE(Control(root, "digest"))), copied ? digest : before));
    REQUIRE(!strcmp(gtk_editable_get_text(GTK_EDITABLE(Control(root, "source"))), "synthetic-source"));
    REQUIRE(!strcmp(gtk_editable_get_text(GTK_EDITABLE(Control(root, "configuration"))), "Debug/native"));
    REQUIRE(context.applies == 0U); ExpectColumn(context.board, "alpha", "testing");
    if (!strcmp(name, "approval-revoked")) {
        REQUIRE(!gtk_check_button_get_active(GTK_CHECK_BUTTON(Control(root, "approval"))));
        Approve(root); ClickReview(root, "apply");
        REQUIRE(context.applies == 0U); ExpectColumn(context.board, "alpha", "testing");
    } else if (copied && strcmp(name, "draft-only")) {
        ClickReview(root, "save"); NativeWait(root);
        UmiKanbanCompletionPlan *saved = NULL;
        REQUIRE(UmiKanbanCompletionPlanLoadFile(file, &saved) == UMI_STATUS_OK);
        const UmiKanbanCompletionRecord *record = UmiKanbanCompletionPlanAt(saved, 0U);
        REQUIRE(record && !strcmp(record->evidence_sha256, digest));
        /* Failure or empty coverage is still visible; it must never be silently
         * promoted to success. Explicit manual review defines this plan's scope. */
        if (!strcmp(name, "mixed") || !strcmp(name, "empty-suite")) {
            const char *message = gtk_label_get_text(GTK_LABEL(Tagged(report, "ctest.report.evidence")));
            REQUIRE(strstr(message, !strcmp(name, "mixed") ? "1 failed" : "0 tests") != NULL);
        }
        UmiKanbanCompletionPlanDestroy(saved);
    }
    g_free(digest); UmiKanbanCompletionGtk4Retire(root);
    gtk_window_destroy(window); ReportWait(report); NativeWait(root);
    g_object_unref(report); g_object_unref(root); NativeDrain();
    umi_kanban_board_destroy(context.board);
    return 0;
}
