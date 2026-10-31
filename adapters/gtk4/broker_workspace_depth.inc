/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/broker_workspace_depth.inc
 *
 * PURPOSE:
 *   Present provider-ordered market depth with exact values, stable cells and explicit subscription ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Depth updates are positional. Keep one set of native labels and project the
 * provider's row order into them; neither sorting nor repainting creates a new
 * subscription. Bid and ask availability are independent. */
static void WorkspaceDepthStale(UmiBrokerWorkspaceGtk *owner)
{
    owner->depth.stale = true;
    for (size_t i = 0U; i < UMI_IBKR_DEPTH_ROW_LIMIT; ++i)
        owner->depth.bids[i].stale = owner->depth.asks[i].stale = true;
}
static const char *WorkspaceDepthNumber(UmiDecimal value, char *buffer, size_t capacity)
{
    return UmiDecimalFormat(value, buffer, capacity) == UMI_STATUS_OK ? buffer : "Unavailable";
}
static void WorkspaceDepthRender(UmiBrokerWorkspaceGtk *owner)
{
    gboolean active = owner->hasDepth && owner->depth.subscribed;
    gboolean ready = owner->connection != NULL && owner->snapshot.state == UMI_IBKR_READY &&
        !owner->snapshot.stale && !owner->depthBusy;
    GtkWidget *controls[] = {owner->depthStart, owner->depthStop, owner->depthRows, owner->depthSmart};
    gboolean enabled[] = {ready && !active, ready && active, !active && !owner->depthBusy,
        !active && !owner->depthBusy};
    for (size_t i = 0U; i < G_N_ELEMENTS(controls); ++i) {
        if (owner->closed) return;
        gtk_widget_set_sensitive(controls[i], enabled[i]);
    }
    if (owner->closed) return;
    UmiIbkrDepthSnapshot *capture = &owner->depth;
    char *status = owner->hasDepth
        ? g_strdup_printf("Contract %u • %s • %s • %zu bids / %zu asks • resets %" PRIu64 " • %s%s\n%s",
            (unsigned)capture->selection.contractId, capture->selection.exchange,
            capture->selection.smartDepth ? "SMART aggregated depth" : "Direct depth",
            capture->bidCount, capture->askCount, capture->resetCount,
            capture->failed ? "Failed" : capture->subscribed ? "Subscribed" : "Stopped",
            capture->stale ? " • STALE / unavailable" : " • Received observations",
            capture->message)
        : g_strdup("No depth stream. Select an instrument in Market data, then subscribe.");
    LabelText(owner->depthStatus, status);
    g_free(status);
    if (owner->closed) return;
    for (size_t i = 0U; i < UMI_IBKR_DEPTH_ROW_LIMIT; ++i) {
        gboolean bid = owner->hasDepth && i < capture->bidCount;
        gboolean ask = owner->hasDepth && i < capture->askCount;
        char level[24], bp[96], bs[96], ap[96], as[96];
        (void)g_snprintf(level, sizeof(level), "%zu", i + 1U);
        const UmiIbkrDepthRow *b = &capture->bids[i], *a = &capture->asks[i];
        for (size_t side = 0U; side < 2U; ++side) {
            if (owner->closed) return;
            gtk_widget_set_sensitive(owner->depthTickets[i][side], ready && active &&
                !capture->stale && !capture->failed && (side ? bid && !b->stale : ask && !a->stale));
        }
        const char *values[] = {
            bid && b->hasMarketMaker ? b->marketMaker : "—",
            bid ? WorkspaceDepthNumber(b->size, bs, sizeof(bs)) : "—",
            bid ? WorkspaceDepthNumber(b->price, bp, sizeof(bp)) : "—",
            !bid ? "Not received" : capture->stale || b->stale ? "STALE" : "Observed",
            level,
            ask ? WorkspaceDepthNumber(a->price, ap, sizeof(ap)) : "—",
            ask ? WorkspaceDepthNumber(a->size, as, sizeof(as)) : "—",
            ask && a->hasMarketMaker ? a->marketMaker : "—",
            !ask ? "Not received" : capture->stale || a->stale ? "STALE" : "Observed"};
        for (size_t column = 0U; column < G_N_ELEMENTS(values); ++column) {
            if (owner->closed) return;
            GtkWidget *cell = owner->depthCells[i][column];
            gtk_widget_set_visible(cell, bid || ask);
            if (owner->closed) return;
            /* Avoid replacing equal labels: a quiet refresh must not disturb a
             * user's text selection or trigger avoidable native notifications. */
            char *display = g_utf8_make_valid(values[column], -1);
            if (strcmp(gtk_label_get_text(GTK_LABEL(cell)), display) != 0)
                gtk_label_set_text(GTK_LABEL(cell), display);
            g_free(display);
        }
    }
}
static void WorkspaceDepthRefresh(UmiBrokerWorkspaceGtk *owner)
{
    UmiIbkrDepthSnapshot *capture = g_new0(UmiIbkrDepthSnapshot, 1);
    UmiStatus status = owner->connection == NULL ? UMI_STATUS_CANCELLED :
        UmiIbkrGtkCopyDepth(owner->connection, 15000U, capture);
    if (status == UMI_STATUS_OK) {
        owner->depth = *capture;
        owner->hasDepth = TRUE;
    } else if (status == UMI_STATUS_NOT_FOUND) {
        memset(&owner->depth, 0, sizeof(owner->depth));
        owner->hasDepth = FALSE;
        WorkspaceDepthStale(owner);
    } else WorkspaceDepthStale(owner);
    g_free(capture);
    WorkspaceDepthRender(owner);
}
static UmiStatus WorkspaceDepthSelection(UmiBrokerWorkspaceGtk *owner, UmiIbkrDepthSelection *out)
{
    guint64 contract;
    const char *id = gtk_editable_get_text(GTK_EDITABLE(owner->quoteContract));
    const char *exchange = gtk_editable_get_text(GTK_EDITABLE(owner->quoteExchange));
    UmiIbkrDepthSelection selection = {0};
    gboolean smart = gtk_check_button_get_active(GTK_CHECK_BUTTON(owner->depthSmart));
    if (!g_ascii_string_to_unsigned(id, 10, 1, (guint64)INT_MAX, &contract, NULL) ||
        exchange[0] == '\0' || strlen(exchange) >= sizeof(selection.exchange) ||
        smart != (strcmp(exchange, "SMART") == 0)) return UMI_STATUS_INVALID_ARGUMENT;
    selection.contractId = (uint32_t)contract;
    strcpy(selection.exchange, exchange);
    selection.rows = (uint32_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(owner->depthRows));
    selection.smartDepth = smart != FALSE;
    *out = selection;
    return UMI_STATUS_OK;
}
static void WorkspaceDepthStart(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->depthBusy) return;
    ++owner->references;
    owner->depthBusy = TRUE;
    (void)UmiBrokerWorkspaceGtkRefresh(owner);
    if (!owner->closed && owner->connection != NULL && owner->snapshot.state == UMI_IBKR_READY &&
        !owner->snapshot.stale && !(owner->hasDepth && owner->depth.subscribed)) {
        UmiIbkrDepthSelection selection;
        UmiStatus status = WorkspaceDepthSelection(owner, &selection);
        if (status == UMI_STATUS_OK) status = UmiIbkrGtkSubscribeDepth(owner->connection, &selection);
        if (!owner->closed) WorkspaceDepthRefresh(owner);
        if (!owner->closed) {
            char *notice = status == UMI_STATUS_OK
                ? g_strdup("Depth requested. Each side fills as observations arrive.")
                : g_strdup_printf("Depth not started: %s. Check the contract, exchange, SMART setting and data permissions.",
                    umi_status_text(status));
            LabelText(owner->depthNotice, notice);
            g_free(notice);
        }
    }
    if (!owner->closed) {
        owner->depthBusy = FALSE;
        WorkspaceDepthRender(owner);
    }
    Release(owner);
}
static void WorkspaceDepthStop(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->depthBusy || !owner->hasDepth || !owner->depth.subscribed) return;
    ++owner->references;
    uint32_t expected = owner->depth.requestId;
    owner->depthBusy = TRUE;
    (void)UmiBrokerWorkspaceGtkRefresh(owner);
    if (!owner->closed && owner->connection != NULL) {
        UmiStatus status = owner->depth.requestId != expected ? UMI_STATUS_NOT_FOUND :
            !owner->depth.subscribed ? UMI_STATUS_INVALID_STATE :
                UmiIbkrGtkCancelDepth(owner->connection, expected);
        if (!owner->closed) WorkspaceDepthRefresh(owner);
        if (!owner->closed) LabelText(owner->depthNotice, status == UMI_STATUS_OK
            ? "Depth stopped. Retained levels are historical; quotes and orders are unchanged."
            : "Depth was not stopped. Review the current subscription before trying again.");
    }
    if (!owner->closed) {
        owner->depthBusy = FALSE;
        WorkspaceDepthRender(owner);
    }
    Release(owner);
}
/* A ladder price is an observation, not an execution instruction. Copy the
 * current row into the shared review ticket; stale or missing sides cannot arm it. */
static void WorkspaceDepthTicket(GtkButton *button, gpointer data)
{
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->connection == NULL) return;
    ++owner->references;
    guint slot = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), "depth-ticket"));
    size_t row = slot / 2U; gboolean sell = (slot % 2U) != 0U;
    UmiIbkrDepthSnapshot capture;
    if (row < UMI_IBKR_DEPTH_ROW_LIMIT &&
        UmiIbkrGtkCopyDepth(owner->connection, 15000U, &capture) == UMI_STATUS_OK &&
        !capture.stale && !capture.failed && capture.subscribed &&
        row < (sell ? capture.bidCount : capture.askCount)) {
        const UmiIbkrDepthRow *level = sell ? &capture.bids[row] : &capture.asks[row];
        char price[96];
        if (!level->stale && UmiDecimalFormat(level->price, price, sizeof price) == UMI_STATUS_OK)
            WorkspaceTicketSeed(owner, capture.selection.contractId, capture.selection.exchange, price, sell);
    }
    Release(owner);
}
static GtkWidget *WorkspaceDepthPage(UmiBrokerWorkspaceGtk *owner)
{
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *controls = gtk_flow_box_new();
    gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(controls), GTK_SELECTION_NONE);
    gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(controls), 1U);
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(controls), 4U);
    owner->depthRows = gtk_spin_button_new_with_range(1.0, UMI_IBKR_DEPTH_ROW_LIMIT, 1.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(owner->depthRows), 5.0);
    owner->depthSmart = gtk_check_button_new_with_label("SMART aggregation");
    owner->depthStart = gtk_button_new_with_label("Subscribe to depth");
    owner->depthStop = gtk_button_new_with_label("Stop depth");
    owner->depthStatus = Cell("", FALSE);
    owner->depthNotice = Cell("", FALSE);
    GtkWidget *fields[] = {owner->depthRows, owner->depthSmart, owner->depthStart, owner->depthStop,
        owner->depthStatus, owner->depthNotice};
    const char *names[] = {"rows", "smart", "start", "stop", "status", "notice"};
    for (size_t i = 0U; i < G_N_ELEMENTS(fields); ++i) {
        char *name = g_strconcat("broker-workspace-depth-", names[i], NULL);
        gtk_widget_set_name(fields[i], name); g_free(name);
    }
    GtkWidget *rows = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_box_append(GTK_BOX(rows), Cell("Maximum levels per side", TRUE));
    gtk_box_append(GTK_BOX(rows), owner->depthRows);
    gtk_flow_box_insert(GTK_FLOW_BOX(controls), rows, -1);
    gtk_flow_box_insert(GTK_FLOW_BOX(controls), owner->depthSmart, -1);
    gtk_flow_box_insert(GTK_FLOW_BOX(controls), owner->depthStart, -1);
    gtk_flow_box_insert(GTK_FLOW_BOX(controls), owner->depthStop, -1);
    GtkWidget *help = Cell("Uses the contract ID and exchange in Market data. SMART aggregation requires exchange SMART. "
        "Depth needs broker permissions. Levels follow provider order; there is no complete-book marker or fill guarantee.", FALSE);
    gtk_label_set_wrap(GTK_LABEL(help), TRUE);
    gtk_box_append(GTK_BOX(page), help);
    gtk_box_append(GTK_BOX(page), controls);
    gtk_label_set_wrap(GTK_LABEL(owner->depthStatus), TRUE);
    gtk_label_set_wrap(GTK_LABEL(owner->depthNotice), TRUE);
    gtk_box_append(GTK_BOX(page), owner->depthStatus);
    gtk_box_append(GTK_BOX(page), owner->depthNotice);
    GtkWidget *grid = gtk_grid_new(), *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_hexpand(scroll, TRUE); gtk_widget_set_vexpand(scroll, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), grid);
    gtk_widget_set_name(grid, "broker-workspace-depth-table");
    const char *headings[] = {"Bid venue / maker", "Bid size", "Bid price", "Bid evidence", "Level",
        "Ask price", "Ask size", "Ask venue / maker", "Ask evidence"};
    for (int c = 0; c < 9; ++c) gtk_grid_attach(GTK_GRID(grid), Cell(headings[c], TRUE), c, 0, 1, 1);
    for (size_t row = 0U; row < UMI_IBKR_DEPTH_ROW_LIMIT; ++row) {
        for (int c = 0; c < 9; ++c) {
            GtkWidget *cell = Cell("", FALSE);
            owner->depthCells[row][c] = cell;
            char *name = g_strdup_printf("broker-workspace-depth-cell-%zu-%d", row, c);
            gtk_widget_set_name(cell, name); g_free(name);
            if (c == 1 || c == 2 || c == 5 || c == 6)
                gtk_label_set_xalign(GTK_LABEL(cell), 1.0F);
            gtk_grid_attach(GTK_GRID(grid), cell, c, (int)row + 1, 1, 1);
            gtk_widget_set_visible(cell, FALSE);
        }
    }
    for (size_t row = 0U; row < UMI_IBKR_DEPTH_ROW_LIMIT; ++row) {
        for (size_t side = 0U; side < 2U; ++side) {
            GtkWidget *button = gtk_button_new_with_label(side ? "Sell at bid…" : "Buy at ask…");
            owner->depthTickets[row][side] = button;
            char *name = g_strdup_printf("broker-depth-ticket-%zu-%zu", row, side);
            gtk_widget_set_name(button, name); g_free(name);
            g_object_set_data(G_OBJECT(button), "depth-ticket", GUINT_TO_POINTER((guint)(row * 2U + side)));
            g_signal_connect(button, "clicked", G_CALLBACK(WorkspaceDepthTicket), owner);
            gtk_grid_attach(GTK_GRID(grid), button, 9 + (int)side, (int)row + 1, 1, 1);
            gtk_widget_set_sensitive(button, FALSE);
        }
    }
    gtk_box_append(GTK_BOX(page), scroll);
    g_signal_connect(owner->depthStart, "clicked", G_CALLBACK(WorkspaceDepthStart), owner);
    g_signal_connect(owner->depthStop, "clicked", G_CALLBACK(WorkspaceDepthStop), owner);
    return page;
}
