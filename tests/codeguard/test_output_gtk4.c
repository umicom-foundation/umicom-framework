/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/codeguard/test_output_gtk4.c
 *
 * PURPOSE:
 *   Verify current-context report presentation, paused clearing and copied-evidence lifetime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/codeguard/output_gtk4.h"
#include <stdio.h>
#include <string.h>
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; } } while (0)
static GtkWidget *find(GtkWidget *root, const char *tag)
{
    const char *name = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (name != NULL && strcmp(name, tag) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
        child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = find(child, tag); if (found != NULL) return found;
    }
    return NULL;
}
static char *text_of(GtkWidget *text)
{
    GtkTextIter start, end; GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    return gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
}
typedef struct Probe {
    GtkWidget *owner;
    UmiCodeGuardReviewSnapshot snapshot;
    UmiCodeGuardResult *result;
    unsigned changes;
    int release;
    UmiStatus nested;
} Probe;
static void changed(GtkTextBuffer *buffer, gpointer data)
{
    (void)buffer; Probe *probe = data;
    if (probe->changes++ != 0U) return;
    if (probe->release) {
        /* The public presenter must have finished every borrowed-result read
         * before any GTK callback can release the host's scan storage. */
        umi_codeguard_result_destroy(probe->result); probe->result = NULL;
        g_clear_object(&probe->owner);
    } else probe->nested = UmiCodeGuardOutputGtk4Update(probe->owner, 10U,
        &probe->snapshot, probe->result);
}
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1];
    const char *cases[] = { "ready", "success", "pending", "failed", "cancelled", "stale", "closed", "context-change", "report-loss", "oversize", "malformed", "unchanged", "invalid", "owner-release", "nested", "request-restart" }; int known = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (strcmp(name, cases[i]) == 0) known = 1;
    CHECK(known);
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    if (!gtk_init_check()) return 77;
    Probe probe = {.nested = UMI_STATUS_OK};
    probe.owner = UmiCodeGuardOutputGtk4Create(); CHECK(probe.owner != NULL);
    g_object_ref_sink(probe.owner);
    gpointer weak = probe.owner; g_object_add_weak_pointer(G_OBJECT(probe.owner), &weak);
    GtkWidget *text = find(probe.owner, "codeguard.output.text");
    GtkWidget *label = find(probe.owner, "codeguard.output.status");
    GtkWidget *follow = find(probe.owner, "codeguard.output.follow");
    GtkWidget *refresh = find(probe.owner, "codeguard.output.refresh");
    CHECK(text && label && follow && refresh);
    g_object_ref(text); g_object_ref(label); g_object_ref(follow); g_object_ref(refresh);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text)); g_object_ref(buffer);
    CHECK(umi_codeguard_result_create(4U, &probe.result) == UMI_STATUS_OK);
    UmiCodeGuardFinding finding = {0};
    strcpy(finding.rule_id, "CG.OUTPUT"); strcpy(finding.path, "src/old.c");
    strcpy(finding.message, "Review this saved file."); strcpy(finding.remediation, "Check the boundary.");
    finding.severity = UMI_CODEGUARD_HIGH; finding.category = UMI_CODEGUARD_CATEGORY_INPUT;
    finding.line = 1U; finding.confidence = 90U;
    CHECK(umi_codeguard_result_add(probe.result, &finding) == UMI_STATUS_OK);
    probe.snapshot.request_id = 1U; probe.snapshot.context_id = 10U;
    probe.snapshot.report_context_id = 10U; probe.snapshot.state = UMI_TASK_SUCCEEDED;
    probe.snapshot.status = probe.snapshot.operation_status = UMI_STATUS_OK;
    probe.snapshot.has_report = 1; probe.snapshot.summary = umi_codeguard_summary_build(probe.result);
    CHECK(UmiCodeGuardOutputGtk4Update(probe.owner, 10U, &probe.snapshot, probe.result) == UMI_STATUS_OK);
    char *initial = text_of(text); CHECK(strstr(initial, "CG.OUTPUT") != NULL);
    uint64_t context = 10U;
    const UmiCodeGuardResult *input = probe.result;
    int expect_empty = 0, run_update = 1;
    gulong handler = 0U;
    if (strcmp(name, "ready") == 0) {
        memset(&probe.snapshot, 0, sizeof probe.snapshot);
        probe.snapshot.status = probe.snapshot.operation_status = UMI_STATUS_NOT_FOUND;
        input = NULL; expect_empty = 1;
    } else if (strcmp(name, "pending") == 0) {
        probe.snapshot.request_id = 2U; probe.snapshot.pending = 1;
        probe.snapshot.state = UMI_TASK_RUNNING; probe.snapshot.status = UMI_STATUS_BUSY;
    } else if (strcmp(name, "failed") == 0 || strcmp(name, "cancelled") == 0) {
        probe.snapshot.request_id = 2U;
        probe.snapshot.state = strcmp(name, "failed") == 0 ? UMI_TASK_FAILED : UMI_TASK_CANCELLED;
        probe.snapshot.status = probe.snapshot.operation_status =
            strcmp(name, "failed") == 0 ? UMI_STATUS_IO_ERROR : UMI_STATUS_CANCELLED;
    } else if (strcmp(name, "stale") == 0 || strcmp(name, "closed") == 0 ||
        strcmp(name, "context-change") == 0 || strcmp(name, "report-loss") == 0) {
        gtk_check_button_set_active(GTK_CHECK_BUTTON(follow), FALSE);
        context = strcmp(name, "closed") == 0 ? 0U : strcmp(name, "report-loss") == 0 ? 10U : 11U;
        probe.snapshot.has_report = 0; memset(&probe.snapshot.summary, 0, sizeof probe.snapshot.summary);
        input = NULL; expect_empty = 1;
        if (strcmp(name, "stale") == 0) {
            probe.snapshot.stale = 1; probe.snapshot.pending = 1;
            probe.snapshot.cancel_requested = 1; probe.snapshot.state = UMI_TASK_RUNNING;
            probe.snapshot.status = UMI_STATUS_BUSY;
        }
    } else if (strcmp(name, "oversize") == 0 || strcmp(name, "malformed") == 0) {
        gtk_check_button_set_active(GTK_CHECK_BUTTON(follow), FALSE);
        probe.snapshot.request_id = 2U; expect_empty = 1;
        if (strcmp(name, "oversize") == 0) {
            memset(finding.message, 'x', sizeof finding.message - 1U);
            finding.message[sizeof finding.message - 1U] = '\0';
            for (unsigned i = 0U; i < 400U; ++i)
                CHECK(umi_codeguard_result_add(probe.result, &finding) == UMI_STATUS_OK);
        } else {
            memset(finding.message, 'x', sizeof finding.message);
            CHECK(umi_codeguard_result_add(probe.result, &finding) == UMI_STATUS_OK);
        }
        probe.snapshot.summary = umi_codeguard_summary_build(probe.result);
    } else if (strcmp(name, "invalid") == 0) {
        CHECK(UmiCodeGuardOutputGtk4Update(NULL, 10U, &probe.snapshot, input) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCodeGuardOutputGtk4Update(probe.owner, 10U, NULL, input) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCodeGuardOutputGtk4Update(probe.owner, 0U, &probe.snapshot, input) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCodeGuardOutputGtk4Update(probe.owner, 11U, &probe.snapshot, input) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCodeGuardOutputGtk4Update(probe.owner, 10U, &probe.snapshot, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        run_update = 0;
    } else if (strcmp(name, "unchanged") == 0) {
        GtkTextIter start, end; gtk_text_buffer_get_bounds(buffer, &start, &end);
        gtk_text_buffer_select_range(buffer, &start, &end);
    } else if (strcmp(name, "owner-release") == 0 || strcmp(name, "nested") == 0) {
        probe.snapshot.request_id = 2U;
        probe.release = strcmp(name, "owner-release") == 0;
        handler = g_signal_connect(buffer, "changed", G_CALLBACK(changed), &probe);
    } else if (strcmp(name, "request-restart") == 0) {
        gtk_check_button_set_active(GTK_CHECK_BUTTON(follow), FALSE);
        context = 11U; probe.snapshot.context_id = probe.snapshot.report_context_id = context;
        probe.snapshot.request_id = 1U; expect_empty = 1;
    }
    if (run_update) CHECK(UmiCodeGuardOutputGtk4Update(probe.owner, context, &probe.snapshot, input) == UMI_STATUS_OK);
    if (handler != 0U) g_signal_handler_disconnect(buffer, handler);
    if (probe.release) CHECK(probe.owner == NULL && weak == NULL && probe.result == NULL && probe.changes > 0U);
    else {
        char *after = text_of(text);
        if (expect_empty) CHECK(after[0] == '\0');
        else CHECK(strcmp(after, initial) == 0);
        g_free(after);
        if (strcmp(name, "unchanged") == 0) CHECK(gtk_text_buffer_get_has_selection(buffer));
        if (strcmp(name, "pending") == 0) CHECK(strstr(gtk_label_get_text(GTK_LABEL(label)), "running") != NULL);
        if (strcmp(name, "failed") == 0 || strcmp(name, "cancelled") == 0)
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(label)), umi_status_text(probe.snapshot.status)) != NULL);
        if (strcmp(name, "oversize") == 0 || strcmp(name, "malformed") == 0)
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(label)), "preview unavailable") != NULL);
        if (strcmp(name, "nested") == 0) CHECK(probe.changes > 0U && probe.nested == UMI_STATUS_BUSY);
        if (expect_empty) {
            g_signal_emit_by_name(refresh, "clicked");
            after = text_of(text);
            if (strcmp(name, "request-restart") == 0) CHECK(strstr(after, "CG.OUTPUT") != NULL);
            else CHECK(after[0] == '\0');
            g_free(after);
        }
        g_clear_object(&probe.owner); CHECK(weak == NULL);
    }
    g_free(initial); umi_codeguard_result_destroy(probe.result);
    g_clear_object(&buffer); g_clear_object(&text); g_clear_object(&label);
    g_clear_object(&follow); g_clear_object(&refresh);
    return 0;
}
