/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/broker_workspace_pnl.inc
 *
 * PURPOSE:
 *   Present account and position profit and loss with captured scope, missing-value evidence and stale status.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Keep broker scope and reported numeric text together. A changed selection
 * prepares a new request; it never relabels the current stream's observations. */
static void WorkspacePnlRender(UmiBrokerWorkspaceGtk *owner)
{
    gboolean active = owner->hasPnl && owner->pnl.subscribed;
    gboolean ready = owner->connection != NULL && owner->snapshot.state == UMI_IBKR_READY &&
        !owner->snapshot.stale && !owner->pnlBusy;
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->pnlAccounts));
    gboolean account = (size_t)selected < owner->snapshot.accountCount &&
        selected < UMI_IBKR_ACCOUNT_LIMIT;
    GtkWidget *controls[] = {owner->pnlStart, owner->pnlStop, owner->pnlAccounts,
        owner->pnlModel, owner->pnlContract};
    gboolean enabled[] = {ready && account && !active, ready && active,
        !active && !owner->pnlBusy, !active && !owner->pnlBusy, !active && !owner->pnlBusy};
    for (size_t i = 0U; i < G_N_ELEMENTS(controls); ++i) {
        if (owner->closed) return;
        gtk_widget_set_sensitive(controls[i], enabled[i]);
    }
    if (owner->closed) return;
    UmiIbkrPnlSnapshot *capture = &owner->pnl;
    char *status = owner->hasPnl
        ? g_strdup_printf("Account %s • model %s • contract %u • %s%s\n%s",
            capture->selection.account, capture->selection.modelCode[0] ? capture->selection.modelCode : "(all models)",
            (unsigned)capture->selection.contractId,
            capture->failed ? "Failed" : capture->subscribed ? "Subscribed" : "Stopped",
            capture->stale ? " • STALE / unavailable" : " • Received observations", capture->message)
        : g_strdup("No P&L subscription. Choose an advertised account, then subscribe.");
    LabelText(owner->pnlStatus, status); g_free(status);
    const UmiIbkrPnlNumber *numbers[] = {&capture->daily, &capture->unrealized, &capture->realized,
        &capture->position, &capture->marketValue};
    for (size_t i = 0U; i < G_N_ELEMENTS(numbers); ++i) {
        if (owner->closed) return;
        const UmiIbkrPnlNumber *number = numbers[i];
        gboolean applicable = i < 3U || (owner->hasPnl && capture->selection.contractId != 0U);
        const char *value = !applicable ? "Not part of account P&L" :
            !owner->hasPnl || !capture->received ? "Waiting" :
            number->exact ? number->reportedText : "Unavailable";
        LabelText(owner->pnlValues[i], value);
        if (owner->closed) return;
        char *evidence = !applicable ? g_strdup("Select a contract for position P&L") :
            !owner->hasPnl || !capture->received ? g_strdup("No observation received") :
            !number->exact ? g_strdup_printf("%sProvider value cannot be represented exactly%s%s",
                capture->stale ? "STALE • " : "", number->reportedText[0] ? ": " : "", number->reportedText) :
            g_strdup(capture->stale ? "STALE • retained observation" : "Observed; currency not supplied");
        LabelText(owner->pnlEvidence[i], evidence); g_free(evidence);
    }
}
static void WorkspacePnlRefresh(UmiBrokerWorkspaceGtk *owner)
{
    UmiIbkrPnlSnapshot capture;
    UmiStatus status = owner->connection == NULL ? UMI_STATUS_CANCELLED :
        UmiIbkrGtkCopyPnl(owner->connection, 15000U, &capture);
    if (status == UMI_STATUS_OK) {
        owner->pnl = capture; owner->hasPnl = TRUE;
    } else if (status == UMI_STATUS_NOT_FOUND) {
        memset(&owner->pnl, 0, sizeof(owner->pnl)); owner->hasPnl = FALSE; owner->pnl.stale = true;
    } else owner->pnl.stale = true;
    WorkspacePnlRender(owner);
}
static UmiStatus WorkspacePnlSelection(UmiBrokerWorkspaceGtk *owner, UmiIbkrPnlSelection *out)
{
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->pnlAccounts));
    const char *model = gtk_editable_get_text(GTK_EDITABLE(owner->pnlModel));
    const char *id = gtk_editable_get_text(GTK_EDITABLE(owner->pnlContract));
    guint64 contract = 0U;
    UmiIbkrPnlSelection selection = {0};
    if ((size_t)selected >= owner->snapshot.accountCount || selected >= UMI_IBKR_ACCOUNT_LIMIT ||
        strlen(model) >= sizeof(selection.modelCode) ||
        (id[0] != '\0' && !g_ascii_string_to_unsigned(id, 10, 1, (guint64)INT_MAX, &contract, NULL)))
        return UMI_STATUS_INVALID_ARGUMENT;
    memcpy(selection.account, owner->snapshot.accounts[selected], sizeof(selection.account));
    selection.account[sizeof(selection.account) - 1U] = '\0';
    strcpy(selection.modelCode, model); selection.contractId = (uint32_t)contract;
    *out = selection; return UMI_STATUS_OK;
}
static void WorkspacePnlStart(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->pnlBusy) return;
    ++owner->references; owner->pnlBusy = TRUE;
    (void)UmiBrokerWorkspaceGtkRefresh(owner);
    if (!owner->closed && owner->connection != NULL && owner->snapshot.state == UMI_IBKR_READY &&
        !owner->snapshot.stale && !(owner->hasPnl && owner->pnl.subscribed)) {
        UmiIbkrPnlSelection selection;
        UmiStatus status = WorkspacePnlSelection(owner, &selection);
        if (status == UMI_STATUS_OK) status = UmiIbkrGtkSubscribePnl(owner->connection, &selection);
        if (!owner->closed) WorkspacePnlRefresh(owner);
        if (!owner->closed) {
            char *notice = status == UMI_STATUS_OK ? g_strdup("P&L requested. Waiting is not a zero balance.") :
                g_strdup_printf("P&L not started: %s. Check the account, model and optional contract.", umi_status_text(status));
            LabelText(owner->pnlNotice, notice); g_free(notice);
        }
    }
    if (!owner->closed) { owner->pnlBusy = FALSE; WorkspacePnlRender(owner); }
    Release(owner);
}
static void WorkspacePnlStop(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->pnlBusy || !owner->hasPnl || !owner->pnl.subscribed) return;
    ++owner->references; owner->pnlBusy = TRUE;
    uint32_t expected = owner->pnl.requestId;
    (void)UmiBrokerWorkspaceGtkRefresh(owner);
    if (!owner->closed && owner->connection != NULL) {
        UmiStatus status = owner->pnl.requestId != expected ? UMI_STATUS_NOT_FOUND :
            !owner->pnl.subscribed ? UMI_STATUS_INVALID_STATE : UmiIbkrGtkCancelPnl(owner->connection, expected);
        if (!owner->closed) WorkspacePnlRefresh(owner);
        if (!owner->closed) LabelText(owner->pnlNotice, status == UMI_STATUS_OK ?
            "P&L stopped. Retained values are historical; orders and other subscriptions are unchanged." :
            "P&L was not stopped. Review the current subscription before trying again.");
    }
    if (!owner->closed) { owner->pnlBusy = FALSE; WorkspacePnlRender(owner); }
    Release(owner);
}
static GtkWidget *WorkspacePnlPage(UmiBrokerWorkspaceGtk *owner)
{
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8), *controls = gtk_flow_box_new();
    gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(controls), GTK_SELECTION_NONE);
    gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(controls), 1U);
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(controls), 5U);
    owner->pnlAccounts = gtk_drop_down_new(NULL, NULL);
    owner->pnlModel = gtk_entry_new(); owner->pnlContract = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(owner->pnlModel), 63);
    gtk_entry_set_max_length(GTK_ENTRY(owner->pnlContract), 10);
    owner->pnlStart = gtk_button_new_with_label("Subscribe to P&L");
    owner->pnlStop = gtk_button_new_with_label("Stop P&L");
    owner->pnlStatus = Cell("", FALSE); owner->pnlNotice = Cell("", FALSE);
    GtkWidget *fields[] = {owner->pnlAccounts, owner->pnlModel, owner->pnlContract, owner->pnlStart,
        owner->pnlStop, owner->pnlStatus, owner->pnlNotice};
    const char *names[] = {"accounts", "model", "contract", "start", "stop", "status", "notice"};
    const char *labels[] = {"Account", "Model (optional)", "Contract ID (empty for account)"};
    for (size_t i = 0U; i < G_N_ELEMENTS(fields); ++i) {
        char *name = g_strconcat("broker-workspace-pnl-", names[i], NULL);
        gtk_widget_set_name(fields[i], name); g_free(name);
        if (i < 3U) {
            GtkWidget *field = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
            gtk_box_append(GTK_BOX(field), Cell(labels[i], TRUE));
            gtk_box_append(GTK_BOX(field), fields[i]); gtk_flow_box_insert(GTK_FLOW_BOX(controls), field, -1);
        } else if (i < 5U) gtk_flow_box_insert(GTK_FLOW_BOX(controls), fields[i], -1);
    }
    gtk_box_append(GTK_BOX(page), controls);
    gtk_label_set_wrap(GTK_LABEL(owner->pnlStatus), TRUE);
    gtk_label_set_wrap(GTK_LABEL(owner->pnlNotice), TRUE);
    gtk_box_append(GTK_BOX(page), owner->pnlStatus); gtk_box_append(GTK_BOX(page), owner->pnlNotice);
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20U); gtk_grid_set_row_spacing(GTK_GRID(grid), 8U);
    const char *metrics[] = {"Daily P&L", "Unrealized P&L", "Realized P&L", "Position", "Market value"};
    gtk_grid_attach(GTK_GRID(grid), Cell("Metric", TRUE), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), Cell("Reported value", TRUE), 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), Cell("Evidence", TRUE), 2, 0, 1, 1);
    for (size_t i = 0U; i < G_N_ELEMENTS(metrics); ++i) {
        owner->pnlValues[i] = Cell("", FALSE); owner->pnlEvidence[i] = Cell("", FALSE);
        char *valueName = g_strdup_printf("broker-workspace-pnl-value-%zu", i);
        char *evidenceName = g_strdup_printf("broker-workspace-pnl-evidence-%zu", i);
        gtk_widget_set_name(owner->pnlValues[i], valueName); gtk_widget_set_name(owner->pnlEvidence[i], evidenceName);
        g_free(valueName); g_free(evidenceName);
        gtk_label_set_wrap(GTK_LABEL(owner->pnlEvidence[i]), TRUE);
        gtk_grid_attach(GTK_GRID(grid), Cell(metrics[i], TRUE), 0, (int)i + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), owner->pnlValues[i], 1, (int)i + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), owner->pnlEvidence[i], 2, (int)i + 1, 1, 1);
    }
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), grid);
    gtk_widget_set_hexpand(scroll, TRUE); gtk_widget_set_vexpand(scroll, TRUE);
    gtk_box_append(GTK_BOX(page), scroll);
    GtkWidget *help = Cell("P&L follows the broker's reset schedule and valuation settings. These callbacks contain no currency field. "
        "Do not combine different accounts or infer the currency from another selected instrument.", FALSE);
    gtk_label_set_wrap(GTK_LABEL(help), TRUE); gtk_box_append(GTK_BOX(page), help);
    g_signal_connect(owner->pnlStart, "clicked", G_CALLBACK(WorkspacePnlStart), owner);
    g_signal_connect(owner->pnlStop, "clicked", G_CALLBACK(WorkspacePnlStop), owner);
    return page;
}
