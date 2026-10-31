/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/workspace_completed_projection.inc
 *
 * PURPOSE:
 *   Verify completed-order scope, partial results, stale data and reentrant native retirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

static UmiIbkrCompletedOrderReport completed;
static gboolean completedAvailable;
static unsigned completedReads;
static UmiStatus completedReadStatus = UMI_STATUS_OK, completedCopyStatus = UMI_STATUS_OK;
/* Native projection tests replace only the inspector boundary. Core wire
 * decoding, exact values and atomic copies are tested separately. */
UmiStatus UmiIbkrGtkReadCompletedOrders(GtkWindow *window, bool apiOnly)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (completedReadStatus != UMI_STATUS_OK) return completedReadStatus;
    if (completedAvailable && completed.capture.requested) return UMI_STATUS_INVALID_STATE;
    ++completedReads; completedAvailable = TRUE; memset(&completed, 0, sizeof(completed));
    completed.capture.requested = completed.capture.pending = completed.capture.stale = true;
    completed.capture.apiOnly = apiOnly;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkCopyCompletedOrders(GtkWindow *window, uint64_t age, UmiIbkrCompletedOrderReport *out)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (age != 15000U) return UMI_STATUS_INVALID_ARGUMENT;
    if (completedCopyStatus != UMI_STATUS_OK) return completedCopyStatus;
    if (!completedAvailable) return UMI_STATUS_NOT_FOUND;
    *out = completed; return UMI_STATUS_OK;
}
static void CompletedRows(void)
{
    completed.capture.count = 1U; completed.capture.pending = completed.capture.stale = false;
    completed.capture.complete = true;
    UmiIbkrCompletedOrder *row = &completed.rows[0];
    row->permanentId = 1234567890123456789ULL;
    strcpy(row->order.account, "OTHER-ACCOUNT"); strcpy(row->order.localSymbol, "EXAMPLE-FUTURE");
    strcpy(row->order.securityType, "FUT"); strcpy(row->order.currency, "EUR");
    strcpy(row->order.action, "BUY"); strcpy(row->order.orderType, "MKT");
    strcpy(row->order.totalQuantity.reportedText, "9007199254740993.123456789");
    strcpy(row->filledQuantity.reportedText, "0.000000001");
    strcpy(row->status, "Cancelled"); strcpy(row->completedStatus, "Cancelled after partial fill");
    strcpy(row->completedTime, "20261030 12:00:00 BrokerZone");
}
static void CompletedChangeIntent(GObject *object, GParamSpec *pspec, gpointer data)
{
    (void)object; (void)pspec;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(data), FALSE);
}
static int CompletedProjection(const char *test, GtkWidget *root, UmiBrokerWorkspaceGtk **workspace)
{
    GtkWidget *read = Find(root, "broker-workspace-completed-read");
    GtkWidget *api = Find(root, "broker-workspace-completed-api-only");
    GtkWidget *state = Find(root, "broker-workspace-completed-status");
    GtkWidget *notice = Find(root, "broker-workspace-completed-notice");
    GtkWidget *table = Find(root, "broker-workspace-completed-table");
    CHECK(read != NULL && api != NULL && table != NULL);
    CHECK(gtk_widget_get_sensitive(read) && gtk_check_button_get_active(GTK_CHECK_BUTTON(api)));
    if (strcmp(test, "completed-retained") == 0) {
        UmiBrokerWorkspaceGtkDestroy(*workspace); *workspace = NULL;
        g_signal_emit_by_name(read, "clicked"); CHECK(completedReads == 0U); return 0;
    }
    if (strcmp(test, "completed-rejected") == 0) {
        completedReadStatus = UMI_STATUS_CAPACITY_EXCEEDED;
        g_signal_emit_by_name(read, "clicked");
        CHECK(completedReads == 0U && gtk_widget_get_sensitive(read));
        CHECK(strlen(gtk_label_get_text(GTK_LABEL(notice))) != 0U); return 0;
    }
    if (strcmp(test, "completed-external") == 0) {
        completedAvailable = TRUE; completed.capture.requested = completed.capture.pending = true;
        completed.capture.apiOnly = false;
        g_signal_emit_by_name(read, "clicked");
        CHECK(completedReads == 0U && !gtk_widget_get_sensitive(read));
        CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(api)));
        CHECK(Contains(state, "All completed orders")); return 0;
    }
    if (strcmp(test, "completed-all") == 0)
        gtk_check_button_set_active(GTK_CHECK_BUTTON(api), FALSE);
    if (strcmp(test, "completed-scope-intent") == 0)
        g_signal_connect(read, "notify::sensitive", G_CALLBACK(CompletedChangeIntent), api);
    g_signal_emit_by_name(read, "clicked");
    CHECK(completedReads == 1U && completed.capture.requested && completed.capture.pending);
    CHECK(completed.capture.apiOnly == (strcmp(test, "completed-all") != 0));
    g_signal_handlers_disconnect_by_data(read, api);
    CHECK(!gtk_widget_get_sensitive(read) && !gtk_widget_get_sensitive(api));
    g_signal_emit_by_name(read, "clicked"); CHECK(completedReads == 1U);
    CompletedRows();
    if (strcmp(test, "completed-empty") == 0) completed.capture.count = 0U;
    if (strcmp(test, "completed-partial") == 0) {
        completed.capture.complete = false; completed.capture.pending = completed.capture.stale = true;
    }
    if (strcmp(test, "completed-stale") == 0) completed.capture.stale = true;
    if (strcmp(test, "completed-row-stale") == 0) completed.rows[0].stale = true;
    if (strcmp(test, "completed-failed") == 0) {
        completed.capture.complete = false; completed.capture.failed = completed.capture.stale = true;
    }
    if (strcmp(test, "completed-utf8") == 0) {
        memcpy(completed.rows[0].order.localSymbol, "Broken \xff", sizeof("Broken \xff"));
        memcpy(completed.capture.message, "Message \xff", sizeof("Message \xff"));
    }
    if (strcmp(test, "completed-reentrant") == 0)
        g_signal_connect(state, "notify::label", G_CALLBACK(CloseDuringRender), workspace);
    UmiStatus refreshed = UmiBrokerWorkspaceGtkRefresh(*workspace);
    if (strcmp(test, "completed-reentrant") == 0) {
        CHECK(refreshed == UMI_STATUS_CANCELLED && *workspace == NULL);
        g_signal_handlers_disconnect_by_data(state, workspace);
        g_signal_emit_by_name(read, "clicked"); CHECK(completedReads == 1U); return 0;
    }
    CHECK(refreshed == UMI_STATUS_OK);
    CHECK(!gtk_widget_get_sensitive(read));
    if (strcmp(test, "completed-empty") == 0)
        CHECK(Contains(table, "Completed capture contains no orders"));
    else {
        CHECK(Contains(table, "1234567890123456789") && Contains(table, "OTHER-ACCOUNT"));
        CHECK(Contains(table, "9007199254740993.123456789") && Contains(table, "0.000000001"));
        CHECK(Contains(table, "Cancelled after partial fill") && Contains(table, "Not reported"));
        CHECK(Contains(table, "20261030 12:00:00 BrokerZone"));
    }
    if (strcmp(test, "completed-partial") == 0) CHECK(Contains(state, "Receiving"));
    if (strcmp(test, "completed-failed") == 0) CHECK(Contains(state, "Failed"));
    if (strcmp(test, "completed-stale") == 0 || strcmp(test, "completed-row-stale") == 0 ||
        strcmp(test, "completed-failed") == 0 || strcmp(test, "completed-partial") == 0)
        CHECK(Contains(table, "STALE / partial"));
    if (strcmp(test, "completed-utf8") == 0) {
        CHECK(Contains(table, "Broken \xef\xbf\xbd"));
        CHECK(g_utf8_validate(gtk_label_get_text(GTK_LABEL(state)), -1, NULL));
        CHECK((unsigned char)completed.rows[0].order.localSymbol[7] == 0xffU);
    }
    if (strcmp(test, "completed-copy-failure") == 0) {
        completedCopyStatus = UMI_STATUS_BUSY;
        CHECK(UmiBrokerWorkspaceGtkRefresh(*workspace) == UMI_STATUS_OK);
        CHECK(Contains(table, "OTHER-ACCOUNT") && Contains(state, "STALE"));
    }
    if (strcmp(test, "completed-account-scope") == 0) {
        strcpy(observed.selectedAccount, "TEST-B");
        CHECK(UmiBrokerWorkspaceGtkRefresh(*workspace) == UMI_STATUS_OK);
        CHECK(Contains(table, "OTHER-ACCOUNT"));
    }
    if (strcmp(test, "completed-reconnect") == 0) {
        gtk_window_destroy(inspector); CHECK(Contains(state, "STALE"));
        completedAvailable = FALSE;
        CHECK(UmiBrokerWorkspaceGtkOpenConnection(*workspace) == UMI_STATUS_OK);
        CHECK(!Contains(table, "OTHER-ACCOUNT") && gtk_widget_get_sensitive(read));
    }
    return 0;
}
