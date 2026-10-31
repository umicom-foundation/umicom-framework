/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/broker_workspace.c
 *
 * PURPOSE:
 *   Present broker account observations with one explicitly owned local connection session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/workspace_gtk4.h"
#include "umicom/broker_connectivity/access_gtk4.h"
#include "umicom/broker_connectivity/connection_guidance.h"
#include "window_removal_private.h"
#include "umicom/broker_connectivity/watchlist.h"
#include <string.h>
#include <inttypes.h>
#include <limits.h>
#ifdef UMI_IBKR_HAS_HISTORY_CHART
#include "umicom/chart/adapters/cairo_renderer.h"
#endif
#include "broker_workspace_history_canvas.inc"

struct UmiBrokerWorkspaceGtk {
    unsigned references;
    gboolean closed, refreshing, hasSnapshot;
    gboolean opening; /* Serialises reentrant native creation and presentation. */
    GWeakRef parent;
    GObject *observer;
    UmiBrokerAccessPlan plan;
    GtkWindow *connection;
    GtkWidget *root, *open, *status, *account, *summary, *positions, *accounts, *read, *guidance;
    GtkWidget *quoteContract, *quoteExchange, *quoteStart, *quoteStop, *quoteStatus, *quoteNotice;
    GtkWidget *quoteDataType;
    GtkWidget *quoteValues[6], *quoteStates[6];
    GtkWidget *optionValues[UMI_IBKR_OPTION_BASIS_COUNT][UMI_IBKR_OPTION_MEASURE_COUNT];
    GtkWidget *optionStates[UMI_IBKR_OPTION_BASIS_COUNT][UMI_IBKR_OPTION_MEASURE_COUNT];
    GtkWidget *optionAttributes[UMI_IBKR_OPTION_BASIS_COUNT];
    gboolean quoteBusy, hasQuote;
    UmiIbkrQuoteSnapshot quote;
    GtkWidget *searchPattern, *searchStart, *searchAbandon, *searchResults, *searchApply;
    GtkWidget *searchStatus, *searchNotice;
    gboolean searchBusy, searchRendering, hasSearch;
    UmiIbkrSymbolSearchSnapshot search;
    GtkWidget *detailsContract, *detailsExchange, *detailsRead, *detailsAbandon, *detailsQuote;
    GtkWidget *detailsStatus, *detailsNotice, *detailsTable;
    GtkWidget *instrumentSymbol, *instrumentType, *instrumentRoute, *instrumentCurrency, *instrumentExpiry;
    GtkWidget *instrumentFind, *instrumentUse, *instrumentRows;
    gboolean detailsBusy, hasDetails;
    UmiIbkrContractDetailsSnapshot details;
    GtkWidget *optionsContract, *optionsSymbol, *optionsType, *optionsFilter;
    GtkWidget *optionsChain, *optionsExpiry, *optionsStrike, *optionsRight, *optionsCurrency, *optionsRoute;
    GtkWidget *optionsRead, *optionsAbandon, *optionsResolve, *optionsStatus, *optionsNotice;
    gboolean optionsBusy, optionsRendering, hasOptions, optionsItemValid;
    uint32_t optionsDisplayedRequest;
    UmiIbkrOptionChainSnapshot options;
    UmiIbkrOptionChain optionsItem;
    GtkWidget *ordersAll, *ordersRead, *ordersStatus, *ordersNotice, *ordersTable;
    GtkWidget *ticketConfirm, *ticketContract, *ticketExchange, *ticketSide, *ticketQuantity, *ticketPrice;
    GtkWidget *ticketCancelId, *ticketSubmit, *ticketReview, *ticketNotice, *ticketStatus, *ticketButtons[7];
    gboolean ticketBusy;
    uint64_t ticketToken, ticketReviewedAt;
    gboolean ordersBusy, hasOrders;
    UmiIbkrOrderReport orders;
    GtkWidget *completedApiOnly, *completedRead, *completedStatus, *completedNotice, *completedTable;
    gboolean completedBusy, hasCompleted;
    UmiIbkrCompletedOrderReport completed;
    GtkWidget *tradesAccounts, *tradesRead, *tradesAbandon, *tradesStatus, *tradesNotice, *tradesTable;
    gboolean tradesBusy, hasTrades;
    UmiIbkrExecutionSnapshot trades;
    GtkWidget *historySize, *historyDuration, *historyKind, *historyEnd, *historyRth;
    GtkWidget *historyRead, *historyCancel, *historyVisible, *historyOffset;
    GtkWidget *historyStatus, *historyNotice, *historyDetails, *historyDrawing;
    WorkspaceHistoryCanvas *historyCanvas;
    gboolean hasHistory, historyBusy;
    UmiIbkrHistoricalSnapshot history;
    GtkWidget *streamKind, *streamRth, *streamStart, *streamStop, *streamVisible, *streamOffset;
    GtkWidget *streamStatus, *streamNotice, *streamDetails, *streamDrawing;
    WorkspaceHistoryCanvas *streamCanvas;
    gboolean hasStream, streamBusy;
    UmiIbkrRealtimeSnapshot stream;
    GtkWidget *depthRows, *depthSmart, *depthStart, *depthStop, *depthStatus, *depthNotice;
    GtkWidget *depthCells[UMI_IBKR_DEPTH_ROW_LIMIT][9];
    GtkWidget *depthTickets[UMI_IBKR_DEPTH_ROW_LIMIT][2], *historyTickets[2];
    gboolean depthBusy, hasDepth;
    UmiIbkrDepthSnapshot depth;
    GtkWidget *pnlAccounts, *pnlModel, *pnlContract, *pnlStart, *pnlStop, *pnlStatus, *pnlNotice;
    GtkWidget *pnlValues[5], *pnlEvidence[5];
    gboolean pnlBusy, hasPnl;
    UmiIbkrPnlSnapshot pnl;
    GtkWidget *scannerInstrument, *scannerLocation, *scannerCode, *scannerRows, *scannerStart, *scannerStop;
    GtkWidget *scannerResults, *scannerApply, *scannerTable, *scannerStatus, *scannerNotice;
    gboolean scannerBusy, scannerRendering, hasScanner;
    UmiIbkrScannerSnapshot scanner;
    UmiIbkrScannerRow scannerItems[UMI_IBKR_SCANNER_ROW_LIMIT];
    GtkWidget *tapeKind, *tapeIgnoreSize, *tapeStart, *tapeStop, *tapeStatus, *tapeNotice;
    GtkWidget *tapeCells[UMI_IBKR_TIME_SALES_VIEW_LIMIT][13];
    gboolean hasTape, tapeBusy;
    UmiIbkrTimeSalesReport tape;
    GtkWidget *watchName, *watchLabel, *watchPath, *watchRows, *watchNotice, *watchButtons[5];
    gboolean watchBusy;
    UmiIbkrWatchlist watchlist;
    gboolean layoutBusy;
    GtkWidget *split, *panes[2], *pageWidgets[UMI_BROKER_PAGE_COUNT];
    GtkWidget *pageChoice, *pageShow, *pageMove, *splitChoice;
    guint source;
    UmiIbkrConnectionSnapshot snapshot;
};

static void Release(UmiBrokerWorkspaceGtk *owner)
{
    if (--owner->references != 0U) return;
    g_weak_ref_clear(&owner->parent);
    for (unsigned page = 0U; page < UMI_BROKER_PAGE_COUNT; ++page)
        g_clear_object(&owner->pageWidgets[page]);
    g_object_unref(owner->root);
    g_free(owner);
}

/* A separate observation receiver retires with the controller, even when
 * another owner keeps the widget alive after Destroy. It prevents callbacks
 * from using freed controller memory through a retained native window. */
static void Retire(UmiBrokerWorkspaceGtk *owner)
{
    if (owner->closed) return;
    owner->closed = TRUE;
    g_signal_handlers_disconnect_by_data(owner->pageShow, owner);
    g_signal_handlers_disconnect_by_data(owner->pageMove, owner);
    g_signal_handlers_disconnect_by_data(owner->splitChoice, owner);
    g_signal_handlers_disconnect_by_data(owner->open, owner);
    g_signal_handlers_disconnect_by_data(owner->read, owner);
    for (size_t i = 0U; i < G_N_ELEMENTS(owner->watchButtons); ++i)
        g_signal_handlers_disconnect_by_data(owner->watchButtons[i], owner);
    g_signal_handlers_disconnect_by_data(owner->ordersRead, owner);
    GtkWidget *ticketFields[] = {owner->ticketConfirm, owner->ticketContract, owner->ticketExchange,
        owner->ticketSide, owner->ticketQuantity, owner->ticketPrice};
    for (size_t i = 0U; i < G_N_ELEMENTS(ticketFields); ++i)
        g_signal_handlers_disconnect_by_data(ticketFields[i], owner);
    for (size_t i = 0U; i < G_N_ELEMENTS(owner->ticketButtons); ++i)
        g_signal_handlers_disconnect_by_data(owner->ticketButtons[i], owner);
    g_signal_handlers_disconnect_by_data(owner->completedRead, owner);
    g_signal_handlers_disconnect_by_data(owner->tradesRead, owner);
    g_signal_handlers_disconnect_by_data(owner->tradesAbandon, owner);
    g_signal_handlers_disconnect_by_data(owner->tapeStart, owner);
    g_signal_handlers_disconnect_by_data(owner->tapeStop, owner);
    g_signal_handlers_disconnect_by_data(owner->depthStart, owner);
    g_signal_handlers_disconnect_by_data(owner->depthStop, owner);
    for (size_t row = 0U; row < UMI_IBKR_DEPTH_ROW_LIMIT; ++row)
        for (size_t side = 0U; side < 2U; ++side)
            g_signal_handlers_disconnect_by_data(owner->depthTickets[row][side], owner);
    for (size_t side = 0U; side < 2U; ++side)
        g_signal_handlers_disconnect_by_data(owner->historyTickets[side], owner);
    g_signal_handlers_disconnect_by_data(owner->pnlStart, owner);
    g_signal_handlers_disconnect_by_data(owner->pnlStop, owner);
    g_signal_handlers_disconnect_by_data(owner->scannerStart, owner);
    g_signal_handlers_disconnect_by_data(owner->scannerStop, owner);
    g_signal_handlers_disconnect_by_data(owner->scannerApply, owner);
    g_signal_handlers_disconnect_by_data(owner->scannerResults, owner);
    g_signal_handlers_disconnect_by_data(owner->historyRead, owner);
    g_signal_handlers_disconnect_by_data(owner->historyCancel, owner);
    g_signal_handlers_disconnect_by_data(owner->historyVisible, owner);
    g_signal_handlers_disconnect_by_data(owner->historyOffset, owner);
    g_signal_handlers_disconnect_by_data(owner->streamStart, owner);
    g_signal_handlers_disconnect_by_data(owner->streamStop, owner);
    g_signal_handlers_disconnect_by_data(owner->streamVisible, owner);
    g_signal_handlers_disconnect_by_data(owner->streamOffset, owner);
    WorkspaceHistoryClear(owner->streamCanvas);
    gtk_widget_queue_draw(owner->streamDrawing);
    WorkspaceHistoryClear(owner->historyCanvas);
    gtk_widget_queue_draw(owner->historyDrawing);
    g_signal_handlers_disconnect_by_data(owner->quoteStart, owner);
    g_signal_handlers_disconnect_by_data(owner->quoteStop, owner);
    g_signal_handlers_disconnect_by_data(owner->searchStart, owner);
    g_signal_handlers_disconnect_by_data(owner->searchAbandon, owner);
    g_signal_handlers_disconnect_by_data(owner->searchApply, owner);
    g_signal_handlers_disconnect_by_data(owner->searchResults, owner);
    g_signal_handlers_disconnect_by_data(owner->instrumentFind, owner);
    g_signal_handlers_disconnect_by_data(owner->instrumentUse, owner);
    g_signal_handlers_disconnect_by_data(owner->detailsRead, owner);
    g_signal_handlers_disconnect_by_data(owner->detailsAbandon, owner);
    g_signal_handlers_disconnect_by_data(owner->detailsQuote, owner);
    GtkWidget *optionControls[] = {owner->optionsRead, owner->optionsAbandon, owner->optionsResolve,
        owner->optionsChain, owner->optionsExpiry, owner->optionsStrike};
    for (size_t i = 0U; i < G_N_ELEMENTS(optionControls); ++i)
        g_signal_handlers_disconnect_by_data(optionControls[i], owner);
    if (owner->source != 0U) {
        guint source = owner->source;
        owner->source = 0U;
        (void)g_source_remove(source);
    }
    g_clear_object(&owner->observer);
    GtkWindow *connection = owner->connection;
    owner->connection = NULL;
    if (connection != NULL) {
        gtk_window_destroy(connection);
        g_object_unref(connection);
    }
}

/* Provider text stays unchanged in the copied observation. GTK requires UTF-8,
 * so only the display projection substitutes invalid byte sequences. */
static void LabelText(GtkWidget *label, const char *text)
{
    char *display = g_utf8_make_valid(text, -1);
    /* A new transport frame is not a new label. Avoid disturbing selection
     * and emitting presentation notifications when the display is unchanged.
     * Preserve the former unconditional update for engineering review. */
#if 0
    gtk_label_set_text(GTK_LABEL(label), display);
#endif
    if (strcmp(gtk_label_get_text(GTK_LABEL(label)), display) != 0)
        gtk_label_set_text(GTK_LABEL(label), display);
    g_free(display);
}

static GtkWidget *Cell(const char *text, gboolean heading)
{
    GtkWidget *label = gtk_label_new(NULL);
    LabelText(label, text);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_label_set_selectable(GTK_LABEL(label), !heading);
    gtk_widget_set_margin_start(label, 8);
    gtk_widget_set_margin_end(label, 8);
    gtk_widget_set_margin_top(label, 5);
    gtk_widget_set_margin_bottom(label, 5);
    if (heading) gtk_widget_add_css_class(label, "heading");
    return label;
}

static GtkWidget *AccountTable(const UmiIbkrConnectionSnapshot *snapshot)
{
    GtkWidget *grid = gtk_grid_new();
    const char *titles[] = {"Account value", "Reported value", "Currency"};
    for (int i = 0; i < 3; ++i) gtk_grid_attach(GTK_GRID(grid), Cell(titles[i], TRUE), i, 0, 1, 1);
    for (size_t i = 0U; i < snapshot->valueCount && i < UMI_IBKR_VALUE_LIMIT; ++i) {
        const UmiIbkrAccountValue *value = &snapshot->values[i];
        const char *cells[] = {value->tag, value->value, value->currency};
        for (int column = 0; column < 3; ++column)
            gtk_grid_attach(GTK_GRID(grid), Cell(cells[column], FALSE), column, (int)i + 1, 1, 1);
    }
    if (snapshot->valueCount == 0U)
        gtk_grid_attach(GTK_GRID(grid), Cell(snapshot->summaryComplete
            ? "Completed report contains no account values."
            : "Connect to the broker, then select an account and choose Read account.", FALSE), 0, 1, 3, 1);
    return grid;
}

static GtkWidget *PositionTable(const UmiIbkrConnectionSnapshot *snapshot)
{
    GtkWidget *grid = gtk_grid_new();
    const char *titles[] = {"Instrument", "Type", "Exchange", "Currency", "Quantity", "Average cost", "Contract ID"};
    for (int i = 0; i < 7; ++i) gtk_grid_attach(GTK_GRID(grid), Cell(titles[i], TRUE), i, 0, 1, 1);
    for (size_t i = 0U; i < snapshot->positionCount && i < UMI_IBKR_POSITION_LIMIT; ++i) {
        const UmiIbkrPositionObservation *p = &snapshot->positions[i];
        const char *cells[] = {p->localSymbol[0] != '\0' ? p->localSymbol : p->symbol,
            p->securityType, p->exchange, p->currency, p->quantity, p->averageCost, p->contractId};
        for (int column = 0; column < 7; ++column)
            gtk_grid_attach(GTK_GRID(grid), Cell(cells[column], FALSE), column, (int)i + 1, 1, 1);
    }
    if (snapshot->positionCount == 0U)
        gtk_grid_attach(GTK_GRID(grid), Cell(snapshot->positionsComplete
            ? "Completed report contains no positions."
            : "Positions have not completed; an empty table does not mean an empty account.", FALSE), 0, 1, 7, 1);
    return grid;
}

/* No conversion to floating point: financial decimals and provider-specific
 * unset values are displayed exactly as received. Completion markers belong
 * to separate account/position streams, not to the connection handshake. */
/* The former single table invalidation is retained below. Each report stream
 * now owns its table changes; unrelated price frames and clock observations
 * must not recreate selectable rows or reset a user's scroll position. */
#if 0
static void Render(UmiBrokerWorkspaceGtk *owner, gboolean replaceTables)
{
    const UmiIbkrConnectionSnapshot *s = &owner->snapshot;
    /* Preserve the original one-shot gate for review. Explicit refresh uses
     * the same connection and waits for both completion markers. */
#if 0
    gtk_widget_set_sensitive(owner->read, s->state == UMI_IBKR_READY &&
        !s->stale && !s->requestIssued && s->accountCount > 0U);
#endif
    gboolean canRead = s->state == UMI_IBKR_READY && !s->stale &&
        (!s->requestIssued || (s->summaryComplete && s->positionsComplete)) && s->accountCount > 0U;
    gtk_widget_set_sensitive(owner->read, canRead);
    if (owner->closed) return;
    gtk_widget_set_sensitive(owner->accounts, canRead);
    if (owner->closed) return;
    gtk_button_set_label(GTK_BUTTON(owner->read), s->requestIssued ? "Refresh account" : "Read account");
    if (owner->closed) return;
    char *account = g_strdup_printf("Account: %s",
        s->selectedAccount[0] != '\0' ? s->selectedAccount : "not selected");
    LabelText(owner->account, account);
    g_free(account);
    if (owner->closed) return;
    char *status = g_strdup_printf("Requested %s • %s • %s • Account values: %s • Positions: %s\n%s",
        s->requestedEnvironment == UMI_TRADING_LIVE ? "LIVE" : "PAPER",
        UmiIbkrConnectionStateName(s->state),
        s->stale ? "STALE — reconnect and read again" : "Read-only observation",
        s->summaryComplete ? "complete" : "not complete",
        s->positionsComplete ? "complete" : "not complete",
        s->message[0] != '\0' ? s->message :
            "Log in to local TWS or IB Gateway, then open Connection and account tools.");
    LabelText(owner->status, status);
    g_free(status);
    if (owner->closed) return;
    UmiIbkrConnectionGuidance guidance;
    if (UmiIbkrConnectionExplain(s, &guidance) == UMI_STATUS_OK) {
        char *help = g_strdup_printf("%s — %s", guidance.title, guidance.detail);
        LabelText(owner->guidance, help);
        g_free(help);
    }
    if (owner->closed || !replaceTables) return;
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(owner->summary), AccountTable(s));
    if (owner->closed) return;
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(owner->positions), PositionTable(s));
}

#endif
static void Render(UmiBrokerWorkspaceGtk *owner, gboolean replaceValues, gboolean replacePositions)
{
    const UmiIbkrConnectionSnapshot *s = &owner->snapshot;
    /* Preserve the original one-shot gate for review. Explicit refresh uses
     * the same connection and waits for both completion markers. */
#if 0
    gtk_widget_set_sensitive(owner->read, s->state == UMI_IBKR_READY &&
        !s->stale && !s->requestIssued && s->accountCount > 0U);
#endif
    gboolean canRead = s->state == UMI_IBKR_READY && !s->stale &&
        (!s->requestIssued || (s->summaryComplete && s->positionsComplete)) && s->accountCount > 0U;
    gtk_widget_set_sensitive(owner->read, canRead);
    if (owner->closed) return;
    gtk_widget_set_sensitive(owner->accounts, canRead);
    if (owner->closed) return;
    gtk_button_set_label(GTK_BUTTON(owner->read), s->requestIssued ? "Refresh account" : "Read account");
    if (owner->closed) return;
    char *account = g_strdup_printf("Account: %s",
        s->selectedAccount[0] != '\0' ? s->selectedAccount : "not selected");
    LabelText(owner->account, account);
    g_free(account);
    if (owner->closed) return;
    char *status = g_strdup_printf("Requested %s • %s • %s • Account values: %s • Positions: %s\n%s",
        s->requestedEnvironment == UMI_TRADING_LIVE ? "LIVE" : "PAPER",
        UmiIbkrConnectionStateName(s->state),
        /* Preserve the original observation-only badge; permission is now visible. */
#if 0
        s->stale ? "STALE — reconnect and read again" : "Read-only observation",
#endif
        s->stale ? "STALE — reconnect and read again" :
            s->readOnly ? "Read-only session" : "Order entry enabled — review every ticket",
        s->summaryComplete ? "complete" : "not complete",
        s->positionsComplete ? "complete" : "not complete",
        s->message[0] != '\0' ? s->message :
            "Log in to local TWS or IB Gateway, then open Connection and account tools.");
    LabelText(owner->status, status);
    g_free(status);
    if (owner->closed) return;
    UmiIbkrConnectionGuidance guidance;
    if (UmiIbkrConnectionExplain(s, &guidance) == UMI_STATUS_OK) {
        char *help = g_strdup_printf("%s — %s", guidance.title, guidance.detail);
        LabelText(owner->guidance, help);
        g_free(help);
    }
    if (owner->closed) return;
    if (replaceValues)
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(owner->summary), AccountTable(s));
    if (owner->closed) return;
    if (replacePositions)
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(owner->positions), PositionTable(s));
}

#include "broker_workspace_search.inc"
#include "broker_workspace_details.inc"
#include "broker_workspace_options.inc"
#include "broker_workspace_option_risk.inc"
#include "broker_workspace_quotes.inc"
#include "broker_workspace_ticket.inc"
#include "broker_workspace_orders.inc"
#include "broker_workspace_completed.inc"
#include "broker_workspace_trades.inc"
#include "broker_workspace_history.inc"
#include "broker_workspace_streaming.inc"
#include "broker_workspace_depth.inc"
#include "broker_workspace_time_sales.inc"
#include "broker_workspace_pnl.inc"
#include "broker_workspace_scanner.inc"
#include "broker_workspace_watchlist.inc"
#include "broker_workspace_layout.inc"

static void ConnectionRemoved(GtkWidget *widget, gpointer data)
{
    UmiBrokerWorkspaceGtk *owner = data;
    ++owner->references;
    if (GTK_WIDGET(owner->connection) == widget) {
        GtkWindow *window = owner->connection;
        owner->connection = NULL;
        if (owner->hasSnapshot) {
            owner->snapshot.stale = true;
            owner->snapshot.state = UMI_IBKR_DISCONNECTED;
        }
        g_object_unref(window);
        WorkspaceQuoteStale(owner);
        WorkspaceOrdersStale(owner);
        WorkspaceCompletedStale(owner);
        if (owner->hasTrades) owner->trades.stale = true;
        owner->history.stale = true;
        owner->history.pending = false;
        if (!owner->closed) Render(owner, FALSE, FALSE);
        if (!owner->closed) WorkspaceQuoteRender(owner, TRUE);
        if (owner->hasDetails) owner->details.stale = true;
        if (!owner->closed) WorkspaceDetailsRender(owner, TRUE);
        owner->options.stale = true;
        if (!owner->closed) WorkspaceOptionsRender(owner);
        if (owner->hasSearch) owner->search.stale = true;
        if (!owner->closed) WorkspaceSearchRender(owner, TRUE);
        if (!owner->closed) WorkspaceOrdersRender(owner, TRUE);
        if (!owner->closed) WorkspaceCompletedRender(owner, TRUE);
        if (!owner->closed) WorkspaceTradesRender(owner, TRUE);
        if (!owner->closed) WorkspaceHistoryRender(owner);
        owner->stream.stale = true;
        owner->stream.active = false;
        if (!owner->closed) WorkspaceStreamRender(owner);
        WorkspaceTapeStale(owner);
        if (!owner->closed) WorkspaceTapeRender(owner);
        WorkspaceDepthStale(owner);
        if (!owner->closed) WorkspaceDepthRender(owner);
        owner->pnl.stale = true;
        if (!owner->closed) WorkspacePnlRender(owner);
        owner->scanner.stale = true;
        if (!owner->closed) WorkspaceScannerRender(owner);
    }
    Release(owner);
}
static void ParentRemoved(GtkWidget *widget, gpointer data)
{
    (void)widget;
    UmiBrokerWorkspaceGtk *owner = data;
    ++owner->references;
    Retire(owner);
    Release(owner);
}

UmiStatus UmiBrokerWorkspaceGtkRefresh(UmiBrokerWorkspaceGtk *owner)
{
    if (owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->closed) return UMI_STATUS_CANCELLED;
    if (owner->refreshing) return UMI_STATUS_OK;
    ++owner->references;
    owner->refreshing = TRUE;
    UmiStatus status = UMI_STATUS_OK;
    if (owner->connection != NULL) {
        UmiIbkrConnectionSnapshot *snapshot = g_new0(UmiIbkrConnectionSnapshot, 1);
        status = UmiIbkrGtkCopySnapshot(owner->connection, snapshot);
        if (status == UMI_STATUS_OK &&
            (!owner->hasSnapshot || memcmp(snapshot, &owner->snapshot, sizeof(*snapshot)) != 0)) {
            /* Replace all streams together; reconnecting must clear previous
             * account rows immediately, before the next account is selected. */
            gboolean accountsChanged = !owner->hasSnapshot ||
                snapshot->accountCount != owner->snapshot.accountCount ||
                memcmp(snapshot->accounts, owner->snapshot.accounts, sizeof(snapshot->accounts)) != 0;
            /* Account identity and report generation invalidate both tables.
             * Otherwise only rows and the corresponding completion marker
             * affect the table. Ignore unused array slots and transport counters. */
            gboolean reportChanged = !owner->hasSnapshot ||
                strcmp(snapshot->selectedAccount, owner->snapshot.selectedAccount) != 0 ||
                snapshot->requestIssued != owner->snapshot.requestIssued ||
                snapshot->requestedAtMilliseconds != owner->snapshot.requestedAtMilliseconds;
            size_t valueCount = MIN(snapshot->valueCount, (size_t)UMI_IBKR_VALUE_LIMIT);
            size_t positionCount = MIN(snapshot->positionCount, (size_t)UMI_IBKR_POSITION_LIMIT);
            gboolean valuesChanged = reportChanged ||
                snapshot->summaryComplete != owner->snapshot.summaryComplete ||
                snapshot->valueCount != owner->snapshot.valueCount ||
                memcmp(snapshot->values, owner->snapshot.values,
                       valueCount * sizeof(snapshot->values[0])) != 0;
            gboolean positionsChanged = reportChanged ||
                snapshot->positionsComplete != owner->snapshot.positionsComplete ||
                snapshot->positionCount != owner->snapshot.positionCount ||
                memcmp(snapshot->positions, owner->snapshot.positions,
                       positionCount * sizeof(snapshot->positions[0])) != 0;
            owner->snapshot = *snapshot;
            owner->hasSnapshot = TRUE;
            if (accountsChanged) {
                GtkStringList *accounts = gtk_string_list_new(NULL);
                for (size_t i = 0U; i < snapshot->accountCount && i < UMI_IBKR_ACCOUNT_LIMIT; ++i) {
                    char *display = g_utf8_make_valid(snapshot->accounts[i], -1);
                    gtk_string_list_append(accounts, display);
                    g_free(display);
                }
                gtk_drop_down_set_model(GTK_DROP_DOWN(owner->accounts), G_LIST_MODEL(accounts));
                if (!owner->closed)
                    gtk_drop_down_set_model(GTK_DROP_DOWN(owner->tradesAccounts), G_LIST_MODEL(accounts));
                if (!owner->closed)
                    gtk_drop_down_set_model(GTK_DROP_DOWN(owner->pnlAccounts), G_LIST_MODEL(accounts));
                g_object_unref(accounts);
            }
            /* Preserve the previous full-table refresh for review. */
#if 0
            if (!owner->closed) Render(owner, TRUE);
#endif
            if (!owner->closed) Render(owner, valuesChanged, positionsChanged);
        }
        g_free(snapshot);
        if (!owner->closed) WorkspaceQuoteRefresh(owner);
        if (!owner->closed) WorkspaceSearchRefresh(owner);
        if (!owner->closed) WorkspaceDetailsRefresh(owner);
        if (!owner->closed) WorkspaceOptionsRefresh(owner);
        if (!owner->closed) WorkspaceOrdersRefresh(owner);
        if (!owner->closed) WorkspaceTicketRefresh(owner);
        if (!owner->closed) WorkspaceCompletedRefresh(owner);
        if (!owner->closed) WorkspaceTradesRefresh(owner);
        if (!owner->closed) WorkspaceHistoryRefresh(owner);
        if (!owner->closed) WorkspaceStreamRefresh(owner);
        if (!owner->closed) WorkspaceDepthRefresh(owner);
        if (!owner->closed) WorkspaceTapeRefresh(owner);
        if (!owner->closed) WorkspacePnlRefresh(owner);
        if (!owner->closed) WorkspaceScannerRefresh(owner);
    }
    if (owner->closed) status = UMI_STATUS_CANCELLED;
    owner->refreshing = FALSE;
    Release(owner);
    return status;
}

static gboolean RefreshTick(gpointer data)
{
    UmiBrokerWorkspaceGtk *owner = data;
    ++owner->references;
    (void)UmiBrokerWorkspaceGtkRefresh(owner);
    gboolean keep = !owner->closed;
    if (!keep) owner->source = 0U;
    Release(owner);
    return keep ? G_SOURCE_CONTINUE : G_SOURCE_REMOVE;
}

UmiStatus UmiBrokerWorkspaceGtkOpenConnection(UmiBrokerWorkspaceGtk *owner)
{
    if (owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->closed) return UMI_STATUS_CANCELLED;
    /* Native association and presentation may invoke the host again. Keep the
     * connection slot reserved until the first operation has completed. */
    if (owner->opening) return UMI_STATUS_BUSY;
    owner->opening = TRUE;
    ++owner->references;
    GtkWindow *parent = g_weak_ref_get(&owner->parent);
    UmiStatus status = UMI_STATUS_CANCELLED;
    if (parent != NULL && UmiGtk4WindowRegistered(parent)) {
        if (owner->connection == NULL) {
            GtkWindow *window = UmiIbkrGtkCreateForPlan(parent, &owner->plan);
            if (window != NULL && (owner->closed || !UmiGtk4WindowRegistered(parent))) {
                gtk_window_destroy(window);
            } else if (window != NULL) {
                owner->connection = g_object_ref(window);
                UmiGtk4ObserveWindowRemoval(window, owner->observer, ConnectionRemoved, owner);
            }
        }
        if (!owner->closed && owner->connection != NULL) {
            GtkWindow *window = g_object_ref(owner->connection);
            gtk_window_present(window);
            g_object_unref(window);
            status = owner->closed ? UMI_STATUS_CANCELLED : UmiBrokerWorkspaceGtkRefresh(owner);
        } else if (!owner->closed) status = UMI_STATUS_UNAVAILABLE;
    }
    g_clear_object(&parent);
    owner->opening = FALSE;
    Release(owner);
    return status;
}

/* Both the workspace and inspector request through the same transport owner.
 * The original service enforced one report per session. Explicit refresh now
 * uses request-scoped positions and new summary IDs after capture completion;
 * membership, incomplete-response and connection-lifetime guards still apply. */
static void ReadClicked(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    /* Retain the previous one-shot guard for comparison with repeat capture. */
#if 0
    if (owner->closed || owner->connection == NULL || owner->snapshot.stale ||
        owner->snapshot.state != UMI_IBKR_READY || owner->snapshot.requestIssued) return;
#endif
    if (owner->closed || owner->connection == NULL || owner->snapshot.stale ||
        owner->snapshot.state != UMI_IBKR_READY ||
        (owner->snapshot.requestIssued &&
         (!owner->snapshot.summaryComplete || !owner->snapshot.positionsComplete))) return;
    ++owner->references;
    guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->accounts));
    if ((size_t)index < owner->snapshot.accountCount && index < UMI_IBKR_ACCOUNT_LIMIT) {
        char account[64];
        memcpy(account, owner->snapshot.accounts[index], sizeof(account));
        UmiStatus status = UmiIbkrGtkReadAccount(owner->connection, account);
        if (!owner->closed) (void)UmiBrokerWorkspaceGtkRefresh(owner);
        if (!owner->closed && status != UMI_STATUS_OK)
            gtk_label_set_text(GTK_LABEL(owner->status), umi_status_text(status));
    }
    Release(owner);
}

static void OpenClicked(GtkButton *button, gpointer data)
{
    (void)button;
    (void)UmiBrokerWorkspaceGtkOpenConnection(data);
}

UmiStatus UmiBrokerWorkspaceGtkCreate(GtkWindow *parent,
    const UmiBrokerAccessPlan *plan, UmiBrokerWorkspaceGtk **outWorkspace)
{
    if (parent == NULL || !GTK_IS_WINDOW(parent) || !UmiGtk4WindowRegistered(parent) ||
        outWorkspace == NULL || UmiBrokerAccessPlanValidate(plan) != UMI_STATUS_OK ||
        plan->mode == UMI_BROKER_ACCESS_GUEST) return UMI_STATUS_INVALID_ARGUMENT;
    UmiBrokerWorkspaceGtk *owner = g_new0(UmiBrokerWorkspaceGtk, 1);
    owner->references = 1U;
    owner->plan = *plan;
    owner->snapshot.readOnly = true;
    owner->orders.capture.stale = true;
    owner->completed.capture.stale = true;
    owner->snapshot.requestedEnvironment = plan->mode == UMI_BROKER_ACCESS_LIVE
        ? UMI_TRADING_LIVE : UMI_TRADING_PAPER;
    g_weak_ref_init(&owner->parent, parent);
    owner->observer = g_object_new(G_TYPE_OBJECT, NULL);
    owner->root = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 8));
    gtk_widget_set_name(owner->root, "broker-account-workspace");
    gtk_widget_set_margin_start(owner->root, 12);
    gtk_widget_set_margin_end(owner->root, 12);
    gtk_widget_set_margin_top(owner->root, 8);
    gtk_widget_set_margin_bottom(owner->root, 8);
    char *title = g_strdup_printf("%s account • %s", UmiBrokerAccessModeName(plan->mode),
        plan->program == UMI_IBKR_GATEWAY ? "IB Gateway" : "Trader Workstation");
    gtk_box_append(GTK_BOX(owner->root), Cell(title, TRUE));
    g_free(title);
    GtkWidget *guide = gtk_label_new("Authenticate in TWS / IB Gateway on this computer. "
        "Enable its socket API and match the port. This page displays account observations, "
        "not simulated funds. Orders cannot be submitted here.");
    gtk_label_set_wrap(GTK_LABEL(guide), TRUE);
    gtk_label_set_xalign(GTK_LABEL(guide), 0.0F);
    gtk_box_append(GTK_BOX(owner->root), guide);
    owner->open = gtk_button_new_with_label("Connection and account tools");
    gtk_widget_set_name(owner->open, "broker-workspace-connect");
    gtk_widget_set_halign(owner->open, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(owner->root), owner->open);
    owner->account = Cell("Account: not selected", TRUE);
    gtk_widget_set_name(owner->account, "broker-workspace-account");
    gtk_box_append(GTK_BOX(owner->root), owner->account);
    GtkWidget *selection = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    owner->accounts = gtk_drop_down_new(NULL, NULL);
    gtk_widget_set_name(owner->accounts, "broker-workspace-accounts");
    gtk_widget_set_tooltip_text(owner->accounts, "Accounts reported by this broker session");
    gtk_widget_set_hexpand(owner->accounts, TRUE);
    owner->read = gtk_button_new_with_label("Read account");
    gtk_widget_set_name(owner->read, "broker-workspace-read");
    gtk_box_append(GTK_BOX(selection), owner->accounts);
    gtk_box_append(GTK_BOX(selection), owner->read);
    gtk_box_append(GTK_BOX(owner->root), selection);
    owner->status = Cell("", FALSE);
    gtk_widget_set_name(owner->status, "broker-workspace-status");
    gtk_label_set_wrap(GTK_LABEL(owner->status), TRUE);
    gtk_box_append(GTK_BOX(owner->root), owner->status);
    owner->guidance = Cell("", FALSE);
    gtk_widget_set_name(owner->guidance, "broker-workspace-guidance");
    gtk_label_set_wrap(GTK_LABEL(owner->guidance), TRUE);
    gtk_box_append(GTK_BOX(owner->root), owner->guidance);
    GtkWidget *paned = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
    owner->summary = gtk_scrolled_window_new();
    owner->positions = gtk_scrolled_window_new();
    gtk_widget_set_name(owner->summary, "broker-workspace-values");
    gtk_widget_set_name(owner->positions, "broker-workspace-positions");
    gtk_paned_set_start_child(GTK_PANED(paned), owner->summary);
    gtk_paned_set_end_child(GTK_PANED(paned), owner->positions);
    gtk_paned_set_position(GTK_PANED(paned), 220);
    gtk_widget_set_vexpand(paned, TRUE);
    /* Keep the previous direct placement for review. Account and market-data
     * pages now share the available area without compressing every tool into
     * one vertical form; account panes remain independently resizable. */
#if 0
    gtk_box_append(GTK_BOX(owner->root), paned);
#endif
    /* Preserve the original single notebook for review. The shared layout
     * controller now keeps those pages in two independently resizable panes. */
#if 0
    GtkWidget *pages = gtk_notebook_new();
    gtk_widget_set_name(pages, "broker-workspace-pages");
    gtk_notebook_append_page(GTK_NOTEBOOK(pages), paned, gtk_label_new("Account and positions"));
    gtk_notebook_append_page(GTK_NOTEBOOK(pages), WorkspaceQuotePage(owner), gtk_label_new("Market data"));
    gtk_notebook_append_page(GTK_NOTEBOOK(pages), WorkspaceDetailsPage(owner), gtk_label_new("Instrument"));
    gtk_notebook_append_page(GTK_NOTEBOOK(pages), WorkspaceOptionsPage(owner), gtk_label_new("Options"));
    gtk_notebook_append_page(GTK_NOTEBOOK(pages), WorkspaceOrdersPage(owner), gtk_label_new("Open orders"));
    gtk_notebook_append_page(GTK_NOTEBOOK(pages), WorkspaceCompletedPage(owner), gtk_label_new("Completed orders"));
    gtk_notebook_append_page(GTK_NOTEBOOK(pages), WorkspaceTradesPage(owner), gtk_label_new("Trades"));
    gtk_notebook_append_page(GTK_NOTEBOOK(pages), WorkspaceHistoryPage(owner), gtk_label_new("Chart"));
    gtk_notebook_append_page(GTK_NOTEBOOK(pages), WorkspaceDepthPage(owner), gtk_label_new("Market depth"));
    gtk_notebook_append_page(GTK_NOTEBOOK(pages), WorkspaceTapePage(owner), gtk_label_new("Time & Sales"));
    gtk_notebook_append_page(GTK_NOTEBOOK(pages), WorkspacePnlPage(owner), gtk_label_new("Profit and loss"));
    gtk_notebook_append_page(GTK_NOTEBOOK(pages), WorkspaceScannerPage(owner), gtk_label_new("Scanner"));
    gtk_notebook_set_scrollable(GTK_NOTEBOOK(pages), TRUE);
    gtk_widget_set_vexpand(pages, TRUE);
    gtk_box_append(GTK_BOX(owner->root), pages);
#endif
    WorkspaceLayoutCreate(owner, paned);
    GtkWidget *note = gtk_label_new("Reports are point-in-time observations. Reconnect to request a new report. "
        "Paper / Live is the requested mode; verify the actual account in your broker application.");
    gtk_label_set_wrap(GTK_LABEL(note), TRUE);
    gtk_label_set_xalign(GTK_LABEL(note), 0.0F);
    gtk_box_append(GTK_BOX(owner->root), note);
    Render(owner, TRUE, TRUE);
    WorkspaceQuoteRender(owner, TRUE);
    WorkspaceSearchRender(owner, TRUE);
    WorkspaceDetailsRender(owner, TRUE);
    WorkspaceOptionsRender(owner);
    owner->trades.stale = true;
    WorkspaceOrdersRender(owner, TRUE);
    WorkspaceCompletedRender(owner, TRUE);
    WorkspaceTradesRender(owner, TRUE);
    owner->history.stale = true;
    WorkspaceHistoryRender(owner);
    owner->stream.stale = true;
    WorkspaceStreamRender(owner);
    WorkspaceDepthStale(owner);
    WorkspaceDepthRender(owner);
    WorkspaceTapeStale(owner);
    WorkspaceTapeRender(owner);
    owner->pnl.stale = true;
    WorkspacePnlRender(owner);
    owner->scanner.stale = true;
    WorkspaceScannerRender(owner);
    g_signal_connect(owner->open, "clicked", G_CALLBACK(OpenClicked), owner);
    g_signal_connect(owner->read, "clicked", G_CALLBACK(ReadClicked), owner);
    UmiGtk4ObserveWindowRemoval(parent, owner->observer, ParentRemoved, owner);
    if (owner->closed) { Release(owner); return UMI_STATUS_CANCELLED; }
    owner->source = g_timeout_add(250U, RefreshTick, owner);
    *outWorkspace = owner;
    return UMI_STATUS_OK;
}
GtkWidget *UmiBrokerWorkspaceGtkWidget(UmiBrokerWorkspaceGtk *owner)
{
    return owner != NULL && !owner->closed ? owner->root : NULL;
}
void UmiBrokerWorkspaceGtkDestroy(UmiBrokerWorkspaceGtk *owner)
{
    if (owner == NULL) return;
    Retire(owner);
    Release(owner);
}
