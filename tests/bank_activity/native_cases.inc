/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_activity/native_cases.inc
 * PURPOSE: Exercise the real activity page with memory-only state and retained controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static GtkWidget *ActivityFind(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = ActivityFind(child, id); if (found != NULL) return found;
    }
    return NULL;
}
static int VerifyNativeActivity(const char *name)
{
    GtkWidget *window; BankUi *ui = Create(&window); ActivityCreate(ui);
    GtkWidget *capture = ActivityFind(window, "bank.activity.capture");
    CHECK(GTK_IS_BUTTON(capture) && !gtk_widget_get_sensitive(ui->activityCopy));
    gtk_editable_set_text(GTK_EDITABLE(ui->activityAccount), "account");
    g_signal_emit_by_name(capture, "clicked"); CHECK(ui->activity != NULL && Contains(ui->activityText, "Booked: GBP 1000.00"));
    CHECK(gtk_widget_get_sensitive(ui->activityCopy));
    if (strcmp(name, "activity-capture") == 0) {
        CHECK(Contains(ui->activityText, "Matching postings: 1 of 1") && Contains(ui->activityText, "Captured revision: 3"));
    } else if (strcmp(name, "activity-account") == 0) {
        gtk_editable_set_text(GTK_EDITABLE(ui->activityAccount), "missing"); CHECK(ui->activity == NULL);
        g_signal_emit_by_name(capture, "clicked"); CHECK(ui->activity == NULL && Contains(ui->activityText, "Account not found"));
    } else if (strcmp(name, "activity-invalid-date") == 0 || strcmp(name, "activity-reversed-dates") == 0) {
        gtk_editable_set_text(GTK_EDITABLE(ui->activityFrom), strcmp(name, "activity-invalid-date") == 0 ? "2026-02-30" : "2026-10-02");
        gtk_editable_set_text(GTK_EDITABLE(ui->activityTo), "2026-10-01");
        g_signal_emit_by_name(capture, "clicked"); CHECK(ui->activity == NULL && !gtk_widget_get_sensitive(ui->activityCopy));
    } else if (strcmp(name, "activity-filter") == 0 || strcmp(name, "activity-direction") == 0) {
        if (strcmp(name, "activity-filter") == 0) gtk_editable_set_text(GTK_EDITABLE(ui->activityReference), "missing");
        else gtk_drop_down_set_selected(ui->activityDirection, UMI_BANK_ACTIVITY_DEBITS);
        CHECK(ui->activity == NULL && !gtk_widget_get_sensitive(ui->activityCopy));
        g_signal_emit_by_name(capture, "clicked"); CHECK(ui->activity != NULL && Contains(ui->activityText, "No postings match"));
        CHECK(Contains(ui->activityText, "Booked: GBP 1000.00"));
    } else if (strcmp(name, "activity-copy") == 0 || strcmp(name, "activity-cached") == 0) {
        if (strcmp(name, "activity-cached") == 0) {
            Fixture f = {0}; f.bank = ui->operations; f.serial = 100; Id(&f.operator.id, "operator"); f.operator.capabilities = UMI_BANK_CAP_TEST_FUNDING;
            UmiBankCommand c = Make(&f, UMI_BANK_TEST_CREDIT, "later"); Id(&c.sourceAccountId, "account"); c.amount = Cash(500); Send(&f, &f.operator, &c);
            Refresh(ui); CHECK(Contains(ui->activityText, "Captured revision: 3"));
        }
        GdkClipboard *clipboard = gtk_widget_get_clipboard(ui->activityCopy); gdk_clipboard_set_text(clipboard, "sentinel");
        g_signal_emit_by_name(ui->activityCopy, "clicked"); char *text = ReportClipboard(clipboard);
        CHECK(strstr(text, "\"activity-summary\"") != NULL && strstr(text, "\"100000\"") != NULL && strstr(text, "\"later\"") == NULL); g_free(text);
        gtk_editable_set_text(GTK_EDITABLE(ui->activityFrom), "bad");
        gdk_clipboard_set_text(clipboard, "unchanged"); g_signal_emit_by_name(ui->activityCopy, "clicked");
        text = ReportClipboard(clipboard); CHECK(strcmp(text, "unchanged") == 0); g_free(text);
    } else if (strcmp(name, "activity-close") == 0) {
        GtkWidget *copy = g_object_ref(ui->activityCopy), *entry = g_object_ref(GTK_WIDGET(ui->activityAccount)); g_object_ref(capture);
        gtk_window_destroy(GTK_WINDOW(window)); CHECK(ui->closed && ui->operations == NULL && ui->activity == NULL);
        g_signal_emit_by_name(capture, "clicked"); g_signal_emit_by_name(copy, "clicked"); gtk_editable_set_text(GTK_EDITABLE(entry), "retained");
        g_object_unref(window); window = NULL;
        g_signal_emit_by_name(capture, "clicked"); g_signal_emit_by_name(copy, "clicked");
        g_object_unref(copy); g_object_unref(entry); g_object_unref(capture);
    } else return 2;
    if (window != NULL) { gtk_window_destroy(GTK_WINDOW(window)); g_object_unref(window); }
    return 0;
}
