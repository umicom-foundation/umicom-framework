/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/workspace_options_projection.inc
 *
 * PURPOSE:
 *   Verify native option selection identity, incomplete captures and reentrant retirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

static UmiIbkrOptionChainSnapshot options;
static UmiIbkrOptionChain optionItems[2];
static gboolean optionsAvailable;
static unsigned optionReads, optionAbandons, optionResolutions;
static UmiIbkrOptionSelection optionSelected;
static UmiStatus optionReadStatus = UMI_STATUS_OK, optionCopyStatus = UMI_STATUS_OK;
static size_t optionFailedItem = SIZE_MAX;
UmiStatus UmiIbkrGtkReadOptionChains(GtkWindow *window, const UmiIbkrOptionChainQuery *query)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (optionReadStatus != UMI_STATUS_OK) return optionReadStatus;
    memset(&options, 0, sizeof(options)); optionsAvailable = TRUE; ++optionReads;
    options.requestId = 800U + optionReads; options.query = *query; return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkCopyOptionChains(GtkWindow *window, uint64_t age, UmiIbkrOptionChainSnapshot *out)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (age != 60000U) return UMI_STATUS_INVALID_ARGUMENT;
    if (optionCopyStatus != UMI_STATUS_OK) return optionCopyStatus;
    if (!optionsAvailable) return UMI_STATUS_NOT_FOUND;
    *out = options; return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkCopyOptionChain(GtkWindow *window, uint32_t request, size_t index, UmiIbkrOptionChain *out)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (request != options.requestId || index >= options.count || index >= 2U) return UMI_STATUS_NOT_FOUND;
    if (index == optionFailedItem) return UMI_STATUS_UNAVAILABLE;
    *out = optionItems[index]; return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkAbandonOptionChains(GtkWindow *window, uint32_t request)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (!optionsAvailable || request != options.requestId) return UMI_STATUS_NOT_FOUND;
    ++optionAbandons; options.abandoned = options.stale = true; return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkResolveOption(GtkWindow *window, const UmiIbkrOptionSelection *selection, uint64_t age)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (age != 60000U || selection->currency[0] == '\0' || selection->exchange[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (selection->chainRequest != options.requestId) return UMI_STATUS_NOT_FOUND;
    if (!options.complete || options.stale) return UMI_STATUS_INVALID_STATE;
    ++optionResolutions; optionSelected = *selection;
    detailsAvailable = TRUE; details.optionLookup = true; details.requestId = 900U;
    strcpy(details.requestedOption.symbol, options.query.underlyingSymbol);
    strcpy(details.requestedOption.expiry, optionItems[selection->chainIndex].expirations[selection->expiryIndex]);
    strcpy(details.requestedOption.strike, optionItems[selection->chainIndex].strikes[selection->strikeIndex].reportedText);
    details.requestedOption.right[0] = selection->right;
    strcpy(details.requestedOption.exchange, selection->exchange);
    return UMI_STATUS_OK;
}
static void OptionsRows(void)
{
    options.complete = true; options.count = 2U;
    for (size_t i = 0U; i < 2U; ++i) {
        UmiIbkrOptionChain *item = &optionItems[i];
        item->underlyingContractId = 123U; strcpy(item->exchange, i == 0U ? "SMART" : "SECOND");
        strcpy(item->tradingClass, "CLASS"); strcpy(item->multiplier, "100");
        item->expiryCount = item->strikeCount = 2U;
        strcpy(item->expirations[0], "20270115"); strcpy(item->expirations[1], "20270219");
        strcpy(item->strikes[0].reportedText, "100.000000001");
        strcpy(item->strikes[1].reportedText, "200.000000002");
    }
}
static void OptionsChangeIntent(GObject *object, GParamSpec *spec, gpointer data)
{
    (void)object; (void)spec;
    gtk_editable_set_text(GTK_EDITABLE(data), "OTHER");
}
static int OptionsProjection(const char *test, GtkWidget *root, UmiBrokerWorkspaceGtk **workspace)
{
    GtkWidget *read = Find(root, "broker-workspace-options-read"), *abandon = Find(root, "broker-workspace-options-abandon");
    GtkWidget *resolve = Find(root, "broker-workspace-options-resolve"), *contract = Find(root, "broker-workspace-options-contract");
    GtkWidget *symbol = Find(root, "broker-workspace-options-symbol"), *chain = Find(root, "broker-workspace-options-chain");
    GtkWidget *expiry = Find(root, "broker-workspace-options-expiry"), *strike = Find(root, "broker-workspace-options-strike");
    GtkWidget *right = Find(root, "broker-workspace-options-right"), *currency = Find(root, "broker-workspace-options-currency");
    GtkWidget *route = Find(root, "broker-workspace-options-route"), *status = Find(root, "broker-workspace-options-status");
    GtkWidget *notice = Find(root, "broker-workspace-options-notice");
    CHECK(read != NULL && chain != NULL && resolve != NULL);
    CHECK(gtk_widget_get_sensitive(read) && !gtk_widget_get_sensitive(resolve) && !gtk_widget_get_sensitive(abandon));
    if (!strcmp(test, "options-retained")) {
        UmiBrokerWorkspaceGtkDestroy(*workspace); *workspace = NULL;
        g_signal_emit_by_name(read, "clicked"); g_signal_emit_by_name(resolve, "clicked");
        g_signal_emit_by_name(abandon, "clicked");
        CHECK(optionReads == 0U && optionResolutions == 0U && optionAbandons == 0U); return 0;
    }
    gtk_editable_set_text(GTK_EDITABLE(contract), "123"); gtk_editable_set_text(GTK_EDITABLE(symbol), "EXAMPLE");
    gtk_editable_set_text(GTK_EDITABLE(currency), "USD"); gtk_editable_set_text(GTK_EDITABLE(route), "SMART");
    if (!strcmp(test, "options-long-symbol") || !strcmp(test, "options-long-filter")) {
        char text[121];
        for (size_t i = 0U; i < 60U; ++i) { text[i * 2U] = (char)0xc3; text[i * 2U + 1U] = (char)0xa9; }
        text[120] = 0;
        GtkWidget *field = !strcmp(test, "options-long-symbol") ? symbol : Find(root, "broker-workspace-options-filter");
        gtk_editable_set_text(GTK_EDITABLE(field), text);
        g_signal_emit_by_name(read, "clicked"); CHECK(optionReads == 0U); return 0;
    }
    if (!strcmp(test, "options-invalid")) {
        gtk_editable_set_text(GTK_EDITABLE(contract), "-1");
        g_signal_emit_by_name(read, "clicked"); CHECK(optionReads == 0U && Contains(notice, "positive")); return 0;
    }
    if (!strcmp(test, "options-rejected")) {
        optionReadStatus = UMI_STATUS_NOT_IMPLEMENTED;
        g_signal_emit_by_name(read, "clicked"); CHECK(optionReads == 0U && gtk_widget_get_sensitive(read)); return 0;
    }
    if (!strcmp(test, "options-external")) {
        optionsAvailable = TRUE; options.requestId = 799U; options.query.underlyingContractId = 456U;
        g_signal_emit_by_name(read, "clicked");
        CHECK(optionReads == 0U && Contains(status, "456") && !gtk_widget_get_sensitive(read)); return 0;
    }
    if (!strcmp(test, "options-intent"))
        g_signal_connect(read, "notify::sensitive", G_CALLBACK(OptionsChangeIntent), symbol);
    g_signal_emit_by_name(read, "clicked");
    g_signal_handlers_disconnect_by_data(read, symbol);
    CHECK(optionReads == 1U && !strcmp(options.query.underlyingSymbol, "EXAMPLE"));
    CHECK(options.query.underlyingContractId == 123U && !strcmp(options.query.underlyingSecurityType, "STK"));
    CHECK(!gtk_widget_get_sensitive(read) && gtk_widget_get_sensitive(abandon));
    g_signal_emit_by_name(read, "clicked"); CHECK(optionReads == 1U);
    if (!strcmp(test, "options-abandon") || !strcmp(test, "options-replaced-abandon")) {
        gboolean replaced = !strcmp(test, "options-replaced-abandon");
        if (replaced) ++options.requestId;
        g_signal_emit_by_name(abandon, "clicked");
        CHECK(optionAbandons == (replaced ? 0U : 1U));
        CHECK(cancellations == 0U); return 0;
    }
    OptionsRows();
    if (!strcmp(test, "options-partial")) options.complete = false;
    if (!strcmp(test, "options-empty")) options.count = 0U;
    if (!strcmp(test, "options-stale")) options.stale = true;
    if (!strcmp(test, "options-failed")) options.failed = true;
    if (!strcmp(test, "options-copy-failure")) optionCopyStatus = UMI_STATUS_BUSY;
    if (!strcmp(test, "options-item-failure")) optionFailedItem = 0U;
    if (!strcmp(test, "options-utf8")) memcpy(optionItems[1].tradingClass, "Class \xff", sizeof("Class \xff"));
    if (!strcmp(test, "options-reentrant"))
        g_signal_connect(status, "notify::label", G_CALLBACK(CloseDuringRender), workspace);
    if (!strcmp(test, "options-model-retired"))
        g_signal_connect(chain, "notify::model", G_CALLBACK(CloseDuringRender), workspace);
    UmiStatus refreshed = UmiBrokerWorkspaceGtkRefresh(*workspace);
    if (!strcmp(test, "options-reentrant") || !strcmp(test, "options-model-retired")) {
        CHECK(refreshed == UMI_STATUS_CANCELLED && *workspace == NULL);
        g_signal_handlers_disconnect_by_data(status, workspace);
        g_signal_handlers_disconnect_by_data(chain, workspace);
        g_signal_emit_by_name(resolve, "clicked"); CHECK(optionResolutions == 0U); return 0;
    }
    CHECK(refreshed == UMI_STATUS_OK);
    if (!strcmp(test, "options-partial") || !strcmp(test, "options-empty") ||
        !strcmp(test, "options-stale") || !strcmp(test, "options-failed") || !strcmp(test, "options-copy-failure")) {
        CHECK(!gtk_widget_get_sensitive(resolve)); CHECK(optionResolutions == 0U); return 0;
    }
    CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(chain)) == GTK_INVALID_LIST_POSITION);
    GtkStringList *model = GTK_STRING_LIST(gtk_drop_down_get_model(GTK_DROP_DOWN(chain)));
    CHECK(g_list_model_get_n_items(G_LIST_MODEL(model)) == 2U);
    CHECK(g_utf8_validate(gtk_string_list_get_string(model, 1U), -1, NULL));
    if (!strcmp(test, "options-item-failure")) {
        CHECK(strstr(gtk_string_list_get_string(model, 0U), "unavailable") != NULL);
        gtk_drop_down_set_selected(GTK_DROP_DOWN(chain), 0U); CHECK(!gtk_widget_get_sensitive(resolve));
    }
    gtk_drop_down_set_selected(GTK_DROP_DOWN(chain), 1U);
    CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(expiry)) == GTK_INVALID_LIST_POSITION);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(expiry), 1U);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(strike), 1U);
    CHECK(gtk_widget_get_sensitive(resolve));
    if (!strcmp(test, "options-long-currency") || !strcmp(test, "options-long-route")) {
        char text[121];
        for (size_t i = 0U; i < 60U; ++i) { text[i * 2U] = (char)0xc3; text[i * 2U + 1U] = (char)0xa9; }
        text[120] = 0;
        gtk_editable_set_text(GTK_EDITABLE(!strcmp(test, "options-long-currency") ? currency : route), text);
        g_signal_emit_by_name(resolve, "clicked");
        CHECK(optionResolutions == 0U && Contains(notice, "byte limit")); return 0;
    }
    if (!strcmp(test, "options-put")) gtk_drop_down_set_selected(GTK_DROP_DOWN(right), 1U);
    if (!strcmp(test, "options-empty-route")) gtk_editable_set_text(GTK_EDITABLE(route), "");
    if (!strcmp(test, "options-replaced-selection")) ++options.requestId;
    if (!strcmp(test, "options-disconnect") || !strcmp(test, "options-reconnect")) {
        gtk_window_destroy(inspector);
        CHECK(Contains(status, "STALE") && !gtk_widget_get_sensitive(resolve));
        g_signal_emit_by_name(resolve, "clicked"); CHECK(optionResolutions == 0U);
        if (!strcmp(test, "options-reconnect")) {
            optionsAvailable = FALSE;
            CHECK(UmiBrokerWorkspaceGtkOpenConnection(*workspace) == UMI_STATUS_OK);
            CHECK(!gtk_widget_get_sensitive(resolve) && Contains(status, "No option"));
            CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(chain))) == 0U);
        }
        return 0;
    }
    if (!strcmp(test, "options-selection-intent"))
        g_signal_connect(resolve, "notify::sensitive", G_CALLBACK(OptionsChangeIntent), route);
    g_signal_emit_by_name(resolve, "clicked");
    g_signal_handlers_disconnect_by_data(resolve, route);
    if (!strcmp(test, "options-replaced-selection") || !strcmp(test, "options-empty-route")) CHECK(optionResolutions == 0U);
    else {
        CHECK(optionResolutions == 1U && optionSelected.chainIndex == 1U);
        CHECK(optionSelected.expiryIndex == 1U && optionSelected.strikeIndex == 1U);
        CHECK(optionSelected.right == (!strcmp(test, "options-put") ? 'P' : 'C'));
        CHECK(!strcmp(optionSelected.exchange, "SMART"));
        CHECK(Contains(root, "Option lookup: EXAMPLE") && Contains(notice, "Review the completed response"));
    }
    CHECK(subscriptions == 0U && cancellations == 0U);
    return 0;
}
