/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/workspace_option_risk_projection.inc
 *
 * PURPOSE:
 *   Verify option risk labels, independent bases and native reentrant retirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Native presentation uses the existing synthetic inspector boundary. Core
 * wire tests separately cover decoding; these cases exercise real GTK labels
 * and retirement while a label notification is being published. */
static int OptionRiskProjection(const char *name, GtkWidget *root,
                                 UmiBrokerWorkspaceGtk **workspace)
{
    GtkWidget *value = Find(root, "broker-workspace-option-value-0-1");
    GtkWidget *state = Find(root, "broker-workspace-option-state-0-1");
    GtkWidget *attribute = Find(root, "broker-workspace-option-basis-0");
    CHECK(GTK_IS_LABEL(value) && GTK_IS_LABEL(state) && GTK_IS_LABEL(attribute));
    CHECK(Contains(value, "Not received"));
    if (!strcmp(name, "risk-empty")) {
        CHECK(!quoteAvailable && subscriptions == 0U);
        return 0;
    }
    quoteAvailable = TRUE;
    quote.subscribed = true;
    quote.dataType = UMI_IBKR_DATA_REALTIME;
    quote.options[0].received = true;
    quote.options[0].attributeReceived = true;
    quote.options[0].tickAttribute = 1U;
    UmiIbkrQuoteValue *delta = &quote.options[0].values[UMI_IBKR_OPTION_DELTA];
    delta->received = true;
    delta->dataType = UMI_IBKR_DATA_REALTIME;
    strcpy(delta->text, "-0.1234567890123456789");
    if (!strcmp(name, "risk-unavailable")) { delta->unavailable = true; delta->stale = true; }
    else if (!strcmp(name, "risk-stale")) delta->stale = true;
    else if (!strcmp(name, "risk-delayed")) delta->dataType = UMI_IBKR_DATA_DELAYED;
    else if (!strcmp(name, "risk-legacy")) quote.options[0].attributeReceived = false;
    else if (!strcmp(name, "risk-unknown-basis")) quote.options[0].tickAttribute = 99U;
    else if (!strcmp(name, "risk-reentrant")) {
        g_signal_connect(value, "notify::label", G_CALLBACK(CloseDuringRender), workspace);
        CHECK(UmiBrokerWorkspaceGtkRefresh(*workspace) == UMI_STATUS_CANCELLED);
        CHECK(*workspace == NULL);
        g_signal_handlers_disconnect_by_data(value, workspace);
        return 0;
    }
    CHECK(UmiBrokerWorkspaceGtkRefresh(*workspace) == UMI_STATUS_OK);
    CHECK(Contains(value, !strcmp(name, "risk-unavailable") ? "Unavailable" : "-0.1234567890123456789"));
    CHECK(Contains(state, delta->stale ? "STALE" : "Received within 15 seconds"));
    CHECK(Contains(state, !strcmp(name, "risk-delayed") ? "delayed" : "real-time"));
    CHECK(Contains(attribute, !strcmp(name, "risk-legacy") ? "not supplied" :
        !strcmp(name, "risk-unknown-basis") ? "Unrecognised" : "Price-based"));
    CHECK(Contains(Find(root, "broker-workspace-option-value-1-1"), "Not received"));
    if (!strcmp(name, "risk-independent")) {
        quote.options[3].received = true;
        quote.options[3].values[1] = *delta;
        strcpy(quote.options[3].values[1].text, "0.75");
        CHECK(UmiBrokerWorkspaceGtkRefresh(*workspace) == UMI_STATUS_OK);
        CHECK(Contains(Find(root, "broker-workspace-option-value-3-1"), "0.75"));
        CHECK(Contains(value, "-0.1234567890123456789"));
    } else if (!strcmp(name, "risk-reconnect")) {
        gtk_window_destroy(inspector);
        CHECK(Contains(state, "STALE") && Contains(value, "-0.1234567890123456789"));
        quoteAvailable = FALSE;
        CHECK(UmiBrokerWorkspaceGtkOpenConnection(*workspace) == UMI_STATUS_OK);
        CHECK(Contains(value, "Not received") && Contains(attribute, "No calculation"));
    }
    CHECK(subscriptions == 0U && cancellations == 0U && reads == 0U);
    return 0;
}
