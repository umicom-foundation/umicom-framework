/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/workspace_account_tables_projection.inc
 *
 * PURPOSE:
 *   Verify stable account tables during broker traffic and independent report updates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Exercise the real native tables with synthetic inspector observations.
 * Holding old widgets makes identity comparisons reliable even after a table
 * has been replaced, and lets us inspect retained text selection directly. */
static GtkWidget *AccountTableChild(GtkWidget *scroll)
{
    GtkWidget *child = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(scroll));
    return GTK_IS_VIEWPORT(child) ? gtk_viewport_get_child(GTK_VIEWPORT(child)) : child;
}
static int AccountTablesProjection(const char *name, GtkWidget *root,
                                   UmiBrokerWorkspaceGtk *workspace)
{
    observed.requestIssued = true;
    observed.requestedAtMilliseconds = 100U;
    observed.valueCount = observed.positionCount = 1U;
    strcpy(observed.selectedAccount, "TEST-A");
    strcpy(observed.values[0].tag, "NetLiquidation");
    strcpy(observed.values[0].value, "12345.000000001");
    strcpy(observed.values[0].currency, "USD");
    strcpy(observed.positions[0].localSymbol, "SYNTHETIC");
    strcpy(observed.positions[0].quantity, "-2.000000001");
    CHECK(UmiBrokerWorkspaceGtkRefresh(workspace) == UMI_STATUS_OK);
    GtkWidget *values = Find(root, "broker-workspace-values");
    GtkWidget *positions = Find(root, "broker-workspace-positions");
    CHECK(GTK_IS_SCROLLED_WINDOW(values) && GTK_IS_SCROLLED_WINDOW(positions));
    GtkWidget *oldValues = g_object_ref(AccountTableChild(values));
    GtkWidget *oldPositions = g_object_ref(AccountTableChild(positions));
    CHECK(GTK_IS_GRID(oldValues) && GTK_IS_GRID(oldPositions));
    GtkWidget *value = gtk_grid_get_child_at(GTK_GRID(oldValues), 1, 1);
    CHECK(GTK_IS_LABEL(value));
    gtk_label_select_region(GTK_LABEL(value), 1, 5);
    gboolean changeValues = FALSE, changePositions = FALSE;
    if (!strcmp(name, "table-transport")) {
        observed.framesReceived += 1000U;
        observed.ignoredFrames += 500U;
    } else if (!strcmp(name, "table-clock")) {
        observed.serverEpochSeconds = 100000U;
        observed.clockReceivedAtMilliseconds = 900U;
    } else if (!strcmp(name, "table-message")) {
        observed.providerCode = 2104;
        strcpy(observed.message, "Synthetic farm update");
    } else if (!strcmp(name, "table-stale")) {
        observed.stale = true;
        observed.state = UMI_IBKR_FAILED;
        observed.lastStatus = UMI_STATUS_IO_ERROR;
    } else if (!strcmp(name, "table-values")) {
        strcpy(observed.values[0].value, "98765.000000001");
        changeValues = TRUE;
    } else if (!strcmp(name, "table-positions")) {
        strcpy(observed.positions[0].quantity, "3.000000001");
        changePositions = TRUE;
    } else if (!strcmp(name, "table-summary-complete")) {
        observed.summaryComplete = true;
        changeValues = TRUE;
    } else if (!strcmp(name, "table-positions-complete")) {
        observed.positionsComplete = true;
        changePositions = TRUE;
    } else if (!strcmp(name, "table-clear-values")) {
        observed.valueCount = 0U;
        changeValues = TRUE;
    } else if (!strcmp(name, "table-clear-positions")) {
        observed.positionCount = 0U;
        changePositions = TRUE;
    } else if (!strcmp(name, "table-account")) {
        strcpy(observed.selectedAccount, "TEST-B");
        changeValues = changePositions = TRUE;
    } else if (!strcmp(name, "table-generation")) {
        ++observed.requestedAtMilliseconds;
        changeValues = changePositions = TRUE;
    } else if (!strcmp(name, "table-unused")) {
        strcpy(observed.values[UMI_IBKR_VALUE_LIMIT - 1U].value, "unused");
        strcpy(observed.positions[UMI_IBKR_POSITION_LIMIT - 1U].quantity, "unused");
    } else if (!strcmp(name, "table-selection")) {
        /* A sustained unrelated feed must leave the user's selected value
         * available for copying and must not send another account request. */
        for (unsigned i = 0U; i < 100U; ++i) {
            ++observed.framesReceived;
            CHECK(UmiBrokerWorkspaceGtkRefresh(workspace) == UMI_STATUS_OK);
            CHECK(AccountTableChild(values) == oldValues);
            CHECK(AccountTableChild(positions) == oldPositions);
        }
    } else if (!strcmp(name, "table-reconnect")) {
        gtk_window_destroy(inspector);
        memset(&observed, 0, sizeof(observed));
        observed.readOnly = true;
        CHECK(UmiBrokerWorkspaceGtkOpenConnection(workspace) == UMI_STATUS_OK);
        changeValues = changePositions = TRUE;
    } else if (strcmp(name, "table-unchanged")) {
        g_object_unref(oldValues); g_object_unref(oldPositions);
        return 2;
    }
    CHECK(UmiBrokerWorkspaceGtkRefresh(workspace) == UMI_STATUS_OK);
    CHECK((AccountTableChild(values) != oldValues) == changeValues);
    CHECK((AccountTableChild(positions) != oldPositions) == changePositions);
    CHECK(reads == 0U && subscriptions == 0U);
    if (!changeValues) {
        int first = 0, last = 0;
        CHECK(gtk_label_get_selection_bounds(GTK_LABEL(value), &first, &last));
        CHECK(first == 1 && last == 5);
    }
    if (!strcmp(name, "table-values")) CHECK(Contains(values, "98765.000000001"));
    if (!strcmp(name, "table-positions")) CHECK(Contains(positions, "3.000000001"));
    if (!strcmp(name, "table-clear-values") || !strcmp(name, "table-reconnect"))
        CHECK(!Contains(values, "12345.000000001"));
    if (!strcmp(name, "table-clear-positions") || !strcmp(name, "table-reconnect"))
        CHECK(!Contains(positions, "-2.000000001"));
    if (!strcmp(name, "table-message"))
        CHECK(Contains(Find(root, "broker-workspace-status"), "Synthetic farm update"));
    if (!strcmp(name, "table-stale"))
        CHECK(Contains(Find(root, "broker-workspace-status"), "STALE"));
    g_object_unref(oldValues);
    g_object_unref(oldPositions);
    return 0;
}
