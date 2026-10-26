/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/kanban_completion_report.inc
 *
 * PURPOSE:
 *   Reuse accepted saved-report evidence as a completion-review draft without asserting a test pass.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


/* This optional bridge is composed only when the shared saved-report browser
 * is present. The core Kanban service remains independent of testing and GTK. */
static void UseReportEvidence(GtkButton *button, gpointer data)
{
    CompletionPanel *s = State(data);
    if (!Active(s, GTK_WIDGET(button))) return;
    GtkWidget *root = g_object_ref(s->root);
    s->pending = TRUE; s->abandoned = FALSE;
    UmiCtestReportEvidence evidence;
    UmiStatus status = UmiGtk4CtestReportReadEvidence(s->report_browser, &evidence);
    if (status == UMI_STATUS_OK) {
        /* A newly chosen evidence source needs a new saved/loaded review.
         * Keep the old immutable record for inspection, but revoke its action
         * readiness before setters can invoke observers or change focus. */
        s->accepted_ready = FALSE;
        gtk_check_button_set_active(GTK_CHECK_BUTTON(s->approval), FALSE);
        if (Live(s) && !s->abandoned)
            gtk_editable_set_text(GTK_EDITABLE(s->fields[3]), evidence.identity.sha256);
        if (Live(s) && !s->abandoned) {
            char message[768];
            g_snprintf(message, sizeof message,
                "Draft digest copied from the displayed report (%zu tests: %zu passed, "
                "%zu failed, %zu timed out, %zu skipped, %zu disabled, %zu not run). "
                "Review its scope and source yourself; save a new review before Apply. "
                "The prior retained decision and board are unchanged.",
                evidence.summary.tests, evidence.summary.passed, evidence.summary.failed,
                evidence.summary.timed_out, evidence.summary.skipped, evidence.summary.disabled,
                evidence.summary.not_run);
            Message(s, message);
        }
    } else {
        Message(s, status == UMI_STATUS_BUSY
            ? "Wait for the report browser to finish loading before choosing its evidence."
            : "Open a completed saved CTest report before choosing its evidence.");
    }
    s->pending = FALSE;
    g_object_unref(root);
}
static void CreateReportEvidence(CompletionPanel *s)
{
    s->report_browser = UmiGtk4CtestReportCreate();
    if (!s->report_browser) return;
    g_object_ref_sink(s->report_browser);
    GtkWidget *expander = gtk_expander_new("Inspect saved CTest evidence");
    gtk_widget_set_name(expander, "kanban-completion-report");
    GtkWidget *body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_box_append(GTK_BOX(body), s->report_browser);
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    Button(s, row, "Use displayed report digest", "kanban.completion.use-report",
        G_CALLBACK(UseReportEvidence), FALSE);
    gtk_box_append(GTK_BOX(body), row);
    gtk_expander_set_child(GTK_EXPANDER(expander), body);
    gtk_box_append(GTK_BOX(s->root), expander);
}
