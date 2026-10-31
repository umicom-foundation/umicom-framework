/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_workspace_projection.c
 *
 * PURPOSE:
 *   Verify exact account observations, independent completion, stale data and reentrant retirement.
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
#include "umicom/broker_connectivity/connection_gtk4.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)

/* Only the inspector boundary is replaced. The actual Framework workspace
 * widget, controller, notification handling and table rendering are exercised.
 * These synthetic observations never open a socket or contain real accounts. */
static UmiIbkrConnectionSnapshot observed;
static GtkWindow *inspector;
static unsigned reads;
static char requested[64];
GtkWindow *UmiIbkrGtkCreateForPlan(GtkWindow *parent, const UmiBrokerAccessPlan *plan)
{
    (void)plan;
    inspector = GTK_WINDOW(gtk_window_new());
    gtk_window_set_transient_for(inspector, parent);
    gtk_window_set_destroy_with_parent(inspector, TRUE);
    return inspector;
}
UmiStatus UmiIbkrGtkCopySnapshot(GtkWindow *window, UmiIbkrConnectionSnapshot *snapshot)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    *snapshot = observed;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkReadAccount(GtkWindow *window, const char *account)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    ++reads;
    (void)snprintf(requested, sizeof(requested), "%s", account);
    observed.requestIssued = true;
    (void)snprintf(observed.selectedAccount, sizeof(observed.selectedAccount), "%s", account);
    return UMI_STATUS_OK;
}


/* New execution controls share this existing synthetic inspector boundary.
 * Rejection is intentional: projection tests never grant trading authority. */
UmiStatus UmiIbkrGtkExecutionEnable(GtkWindow *w, const char *a, UmiTradingEnvironment m, bool v)
{ (void)w; (void)a; (void)m; (void)v; return UMI_STATUS_PERMISSION_DENIED; }
UmiStatus UmiIbkrGtkExecutionDisable(GtkWindow *w)
{ (void)w; return UMI_STATUS_OK; }
UmiStatus UmiIbkrGtkOrderPrepare(GtkWindow *w, const UmiIbkrOrderDraft *d, UmiIbkrOrderReview *r)
{ (void)w; (void)d; (void)r; return UMI_STATUS_PERMISSION_DENIED; }
UmiStatus UmiIbkrGtkOrderSubmit(GtkWindow *w, uint64_t t, uint32_t *id)
{ (void)w; (void)t; (void)id; return UMI_STATUS_PERMISSION_DENIED; }
UmiStatus UmiIbkrGtkOrderCancelSubmitted(GtkWindow *w, uint32_t id)
{ (void)w; (void)id; return UMI_STATUS_NOT_FOUND; }
UmiStatus UmiIbkrGtkExecutionCopy(GtkWindow *w, UmiIbkrExecutionReport *r)
{ (void)w; memset(r, 0, sizeof *r); return UMI_STATUS_OK; }
UmiStatus UmiIbkrGtkOrderReadPriceRule(GtkWindow *w, const char *route)
{ (void)w; (void)route; return UMI_STATUS_NOT_FOUND; }
UmiStatus UmiIbkrGtkSearchInstrument(GtkWindow *w, const UmiIbkrInstrumentQuery *q)
{ (void)w; (void)q; return UMI_STATUS_NOT_IMPLEMENTED; }

static UmiIbkrQuoteSnapshot quote;
static gboolean quoteAvailable;
static unsigned subscriptions, cancellations;
static UmiStatus cancelResult = UMI_STATUS_OK;
UmiStatus UmiIbkrGtkCopyQuote(GtkWindow *window, uint64_t maximumAge,
    UmiIbkrQuoteSnapshot *outQuote)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (maximumAge != 15000U) return UMI_STATUS_INVALID_ARGUMENT;
    if (!quoteAvailable) return UMI_STATUS_NOT_FOUND;
    *outQuote = quote;
    return UMI_STATUS_OK;
}
/* Preserve the original default-mode fixture for review; the replacement
 * records the requested mode separately from synthetic provider observations. */
#if 0
UmiStatus UmiIbkrGtkQuoteSubscribe(GtkWindow *window, const UmiIbkrQuoteContract *contract)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    ++subscriptions;
    memset(&quote, 0, sizeof(quote));
    quoteAvailable = TRUE;
    quote.requestId = 36000U + subscriptions;
    quote.contract = *contract;
    quote.subscribed = true;
    return UMI_STATUS_OK;
}
#endif
UmiStatus UmiIbkrGtkQuoteSubscribeWithDataType(GtkWindow *window,
    const UmiIbkrQuoteContract *contract, UmiIbkrMarketDataType type)
{
    if (type < UMI_IBKR_DATA_REALTIME || type > UMI_IBKR_DATA_DELAYED_FROZEN)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (window != inspector) return UMI_STATUS_CANCELLED;
    ++subscriptions;
    memset(&quote, 0, sizeof(quote));
    quoteAvailable = TRUE;
    quote.requestId = 36000U + subscriptions;
    quote.contract = *contract;
    quote.requestedDataType = type;
    quote.subscribed = true;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkQuoteSubscribe(GtkWindow *window, const UmiIbkrQuoteContract *contract)
{
    return UmiIbkrGtkQuoteSubscribeWithDataType(window, contract, UMI_IBKR_DATA_REALTIME);
}
UmiStatus UmiIbkrGtkQuoteCancel(GtkWindow *window)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    ++cancellations;
    if (cancelResult != UMI_STATUS_OK) return cancelResult;
    quote.subscribed = false;
    quote.bid.stale = quote.ask.stale = quote.last.stale = true;
    quote.bidSize.stale = quote.askSize.stale = quote.lastSize.stale = true;
    return UMI_STATUS_OK;
}


static UmiIbkrSymbolSearchSnapshot search;
static gboolean searchAvailable;
static unsigned searches, abandons;
static UmiStatus searchResult = UMI_STATUS_OK;
UmiStatus UmiIbkrGtkCopyInstrumentSearch(GtkWindow *window, UmiIbkrSymbolSearchSnapshot *out)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (!searchAvailable) return UMI_STATUS_NOT_FOUND;
    *out = search;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkSearchInstruments(GtkWindow *window, const char *pattern)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (searchResult != UMI_STATUS_OK) return searchResult;
    ++searches;
    memset(&search, 0, sizeof(search));
    searchAvailable = TRUE;
    search.requestId = 37000U + searches;
    (void)snprintf(search.pattern, sizeof(search.pattern), "%s", pattern);
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkAbandonInstrumentSearch(GtkWindow *window)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    ++abandons;
    search.failed = search.stale = true;
    return UMI_STATUS_OK;
}
/* Deliberately ambiguous names: choosing a name must not choose its first ID. */
static void SearchResults(void)
{
    search.complete = true;
    search.count = 2U;
    search.rows[0].contractId = 123U; search.rows[1].contractId = 456U;
    for (size_t i = 0U; i < 2U; ++i) {
        strcpy(search.rows[i].symbol, "EXAMPLE");
        strcpy(search.rows[i].securityType, "STK");
        strcpy(search.rows[i].currency, i == 0U ? "USD" : "GBP");
        strcpy(search.rows[i].primaryExchange, i == 0U ? "NASDAQ" : "LSE");
        strcpy(search.rows[i].description, "Synthetic contract candidate");
    }
}


static UmiIbkrOrderReport orderReport;
static gboolean orderAvailable;
static unsigned orderReads;
static UmiStatus orderReadResult = UMI_STATUS_OK;
UmiStatus UmiIbkrGtkCopyOrderReport(GtkWindow *window, uint64_t maximumAge,
    UmiIbkrOrderReport *out)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (maximumAge != 15000U) return UMI_STATUS_INVALID_ARGUMENT;
    if (!orderAvailable) return UMI_STATUS_NOT_FOUND;
    *out = orderReport;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkRecoverOrders(GtkWindow *window, UmiIbkrOrderScope scope)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (orderReadResult != UMI_STATUS_OK) return orderReadResult;
    ++orderReads;
    orderAvailable = TRUE;
    memset(&orderReport, 0, sizeof(orderReport));
    orderReport.capture.requested = orderReport.capture.pending = orderReport.capture.stale = true;
    orderReport.capture.scope = scope;
    return UMI_STATUS_OK;
}
static void OrderRows(void)
{
    orderReport.capture.pending = orderReport.capture.stale = false;
    orderReport.capture.complete = true; orderReport.capture.count = 1U;
    UmiIbkrRecoveredOrder *row = &orderReport.rows[0];
    row->key.clientId = 99; row->key.orderId = 12; row->permanentId = 800U;
    row->hasOpenOrder = true; row->statusStale = true;
    strcpy(row->open.account, "TEST-OTHER");
    strcpy(row->open.localSymbol, "SYNTHETIC-STOCK");
    strcpy(row->open.action, "BUY"); strcpy(row->open.orderType, "LMT");
    strcpy(row->open.totalQuantity.reportedText, "9007199254740993.123456789");
    strcpy(row->open.limitPrice.reportedText, "0.000000001");
}


static UmiIbkrExecutionSnapshot trades;
static gboolean tradesAvailable;
static unsigned tradeReads, tradeAbandons;
static UmiStatus tradeReadResult = UMI_STATUS_OK;
UmiStatus UmiIbkrGtkCopyExecutions(GtkWindow *window, UmiIbkrExecutionSnapshot *out)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (!tradesAvailable) return UMI_STATUS_NOT_FOUND;
    *out = trades;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkReadExecutions(GtkWindow *window, const char *account)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (tradeReadResult != UMI_STATUS_OK) return tradeReadResult;
    ++tradeReads;
    memset(&trades, 0, sizeof(trades));
    tradesAvailable = TRUE; trades.requestId = 39000U + tradeReads;
    (void)snprintf(trades.account, sizeof(trades.account), "%s", account);
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkAbandonExecutions(GtkWindow *window, uint32_t expected)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (trades.requestId != expected) return UMI_STATUS_NOT_FOUND;
    ++tradeAbandons; trades.failed = trades.stale = true;
    return UMI_STATUS_OK;
}
static void TradeRows(void)
{
    trades.complete = true; trades.count = 1U;
    UmiIbkrExecutionObservation *row = &trades.rows[0];
    strcpy(row->executionId, "SYNTHETIC-FILL.01");
    strcpy(row->account, trades.account);
    strcpy(row->symbol, "EXAMPLE"); strcpy(row->securityType, "STK");
    strcpy(row->time, "20261030 12:00:01 BrokerZone"); strcpy(row->currency, "USD");
    strcpy(row->side, "BOT"); strcpy(row->exchange, "SYNTHETIC");
    row->permanentOrderId = 77U;
    row->quantity = (UmiDecimal){9007199254740993LL, 0U};
    row->price = (UmiDecimal){1, 9U};
}


/* Synthetic historical observations exercise the public workspace boundary,
 * never a socket, account credential or a market-data subscription. */
static UmiIbkrHistoricalSnapshot history;
static UmiIbkrHistoricalBar historyBar;
static gboolean historyAvailable;
static unsigned historyReads, historyCancels, historyScenes;
static size_t historyFirst, historyCount;
static UmiStatus historyReadStatus = UMI_STATUS_OK, historyCancelStatus = UMI_STATUS_OK;
static UmiStatus historySceneStatus = UMI_STATUS_OK;
UmiStatus UmiIbkrGtkCopyHistory(GtkWindow *window, uint64_t age, UmiIbkrHistoricalSnapshot *out)
{
    (void)window; (void)age;
    if (!historyAvailable) return UMI_STATUS_NOT_FOUND;
    *out = history; return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkReadHistory(GtkWindow *window, const UmiIbkrHistoricalQuery *query)
{
    (void)window;
    if (historyReadStatus != UMI_STATUS_OK) return historyReadStatus;
    if (UmiIbkrHistoricalQueryValidate(query) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    ++historyReads; memset(&history, 0, sizeof(history)); historyAvailable = TRUE;
    history.requestId = 41000U + historyReads; history.query = *query;
    history.pending = history.stale = true;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkCancelHistory(GtkWindow *window, uint32_t request)
{
    (void)window;
    if (historyCancelStatus != UMI_STATUS_OK) return historyCancelStatus;
    if (!historyAvailable || request != history.requestId) return UMI_STATUS_NOT_FOUND;
    ++historyCancels; history.pending = false; history.cancelled = history.stale = true;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkCopyHistoryBar(GtkWindow *window, uint32_t request, size_t index, UmiIbkrHistoricalBar *out)
{
    (void)window;
    if (!historyAvailable || request != history.requestId || index >= history.count) return UMI_STATUS_NOT_FOUND;
    *out = historyBar;
    out->timeMilliseconds += (int64_t)index * 60000;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkCreateHistoryScene(GtkWindow *window, uint32_t request, size_t first,
    size_t count, uint64_t age, UmiChartRenderScene **out)
{
    (void)window; (void)age;
    if (!historyAvailable || request != history.requestId) return UMI_STATUS_NOT_FOUND;
    ++historyScenes; historyFirst = first; historyCount = count;
    if (historySceneStatus != UMI_STATUS_OK) return historySceneStatus;
#ifdef UMI_IBKR_HAS_HISTORY_MODEL
    return umi_chart_render_scene_create(32U, out);
#else
    (void)out;
    return UMI_STATUS_NOT_IMPLEMENTED;
#endif
}
static void HistoryRows(void)
{
    history.pending = history.stale = false; history.complete = true; history.count = 100U;
    memset(&historyBar, 0, sizeof(historyBar));
    historyBar.timeMilliseconds = 1793354400000LL;
    strcpy(historyBar.open.reportedText, "12.000000001");
    strcpy(historyBar.high.reportedText, "12.300000009");
    strcpy(historyBar.low.reportedText, "11.999999999");
    strcpy(historyBar.close.reportedText, "12.100000001");
    historyBar.volumeAvailable = true; strcpy(historyBar.volume.reportedText, "9007199254740993");
}


/* Replace only the session boundary. Positional levels and failures are
 * synthetic so native workspace ownership can be checked without a broker. */
static UmiIbkrDepthSnapshot depth;
static gboolean depthAvailable;
static unsigned depthStarts, depthStops;
static UmiStatus depthStartStatus = UMI_STATUS_OK, depthStopStatus = UMI_STATUS_OK;
UmiStatus UmiIbkrGtkCopyDepth(GtkWindow *window, uint64_t age, UmiIbkrDepthSnapshot *out)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (age != 15000U) return UMI_STATUS_INVALID_ARGUMENT;
    if (!depthAvailable) return UMI_STATUS_NOT_FOUND;
    *out = depth; return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkSubscribeDepth(GtkWindow *window, const UmiIbkrDepthSelection *selection)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (depthStartStatus != UMI_STATUS_OK) return depthStartStatus;
    if (depthAvailable && depth.subscribed) return UMI_STATUS_ALREADY_EXISTS;
    ++depthStarts; memset(&depth, 0, sizeof(depth)); depthAvailable = TRUE;
    depth.requestId = 42000U + depthStarts; depth.selection = *selection;
    depth.subscribed = depth.stale = true;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkCancelDepth(GtkWindow *window, uint32_t expected)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (!depthAvailable || expected != depth.requestId) return UMI_STATUS_NOT_FOUND;
    if (depthStopStatus != UMI_STATUS_OK) return depthStopStatus;
    ++depthStops; depth.subscribed = false; depth.stale = true;
    return UMI_STATUS_OK;
}
static void DepthRows(void)
{
    depth.received = true; depth.stale = false;
    depth.bidCount = 2U; depth.askCount = 1U;
    depth.bids[0].price = (UmiDecimal){12345678901LL, 9U};
    depth.bids[0].size = (UmiDecimal){9007199254740993LL, 0U};
    depth.bids[1].price = (UmiDecimal){12345678900LL, 9U};
    depth.bids[1].size = (UmiDecimal){0, 0U};
    depth.asks[0].price = (UmiDecimal){12345678902LL, 9U};
    depth.asks[0].size = (UmiDecimal){1, 3U};
    depth.bids[0].hasMarketMaker = depth.asks[0].hasMarketMaker = true;
    strcpy(depth.bids[0].marketMaker, "TEST-BID");
    strcpy(depth.asks[0].marketMaker, "TEST-ASK");
}


/* Synthetic account-scoped observations preserve the native projection and
 * lifetime paths while replacing only the session's I/O boundary. */
static UmiIbkrPnlSnapshot pnl;
static gboolean pnlAvailable;
static unsigned pnlStarts, pnlStops;
static UmiStatus pnlStartStatus = UMI_STATUS_OK, pnlStopStatus = UMI_STATUS_OK;
UmiStatus UmiIbkrGtkCopyPnl(GtkWindow *window, uint64_t age, UmiIbkrPnlSnapshot *out)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (age != 15000U) return UMI_STATUS_INVALID_ARGUMENT;
    if (!pnlAvailable) return UMI_STATUS_NOT_FOUND;
    *out = pnl; return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkSubscribePnl(GtkWindow *window, const UmiIbkrPnlSelection *selection)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (pnlStartStatus != UMI_STATUS_OK) return pnlStartStatus;
    if (pnlAvailable && pnl.subscribed) return UMI_STATUS_ALREADY_EXISTS;
    gboolean advertised = FALSE;
    for (size_t i = 0U; i < observed.accountCount && i < UMI_IBKR_ACCOUNT_LIMIT; ++i)
        if (!strcmp(selection->account, observed.accounts[i])) advertised = TRUE;
    if (!advertised) return UMI_STATUS_PERMISSION_DENIED;
    ++pnlStarts; memset(&pnl, 0, sizeof(pnl)); pnlAvailable = TRUE;
    pnl.requestId = 43000U + pnlStarts; pnl.selection = *selection; pnl.subscribed = pnl.stale = true;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkCancelPnl(GtkWindow *window, uint32_t expected)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (!pnlAvailable || expected != pnl.requestId) return UMI_STATUS_NOT_FOUND;
    if (pnlStopStatus != UMI_STATUS_OK) return pnlStopStatus;
    ++pnlStops; pnl.subscribed = false; pnl.stale = true; return UMI_STATUS_OK;
}
static void PnlValues(void)
{
    pnl.received = true; pnl.stale = false;
    UmiIbkrPnlNumber *numbers[] = {&pnl.daily, &pnl.unrealized, &pnl.realized, &pnl.position, &pnl.marketValue};
    const char *text[] = {"-123.450000001", "0", "9007199254740993", "-2", "10.123456789"};
    for (size_t i = 0U; i < G_N_ELEMENTS(numbers); ++i) {
        strcpy(numbers[i]->reportedText, text[i]); numbers[i]->exact = true;
    }
}

static GtkWidget *Find(GtkWidget *root, const char *name)
{
    if (strcmp(gtk_widget_get_name(root), name) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, name);
        if (found != NULL) return found;
    }
    return NULL;
}
static gboolean Contains(GtkWidget *root, const char *text)
{
    if (GTK_IS_LABEL(root) && strstr(gtk_label_get_text(GTK_LABEL(root)), text) != NULL) return TRUE;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child))
        if (Contains(child, text)) return TRUE;
    return FALSE;
}
static void CloseDuringRender(GObject *object, GParamSpec *pspec, gpointer data)
{
    (void)object; (void)pspec;
    UmiBrokerWorkspaceGtk **workspace = data;
    if (*workspace != NULL) {
        UmiBrokerWorkspaceGtk *closing = *workspace;
        *workspace = NULL;
        UmiBrokerWorkspaceGtkDestroy(closing);
    }
}
#include "workspace_scanner_projection.inc"
#include "workspace_completed_projection.inc"
#include "workspace_details_projection.inc"
#include "workspace_options_projection.inc"
#include "workspace_time_sales_projection.inc"
#include "workspace_streaming_projection.inc"
#include "workspace_account_tables_projection.inc"
#include "workspace_option_risk_projection.inc"

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!gtk_init_check()) return 77;
    UmiBrokerAccessPlan plan;
    CHECK(UmiBrokerAccessPlanCreate(UMI_BROKER_ACCESS_PAPER, &plan) == UMI_STATUS_OK);
    GtkWindow *parent = GTK_WINDOW(gtk_window_new());
    UmiBrokerWorkspaceGtk *workspace = NULL;
    CHECK(UmiBrokerWorkspaceGtkCreate(parent, &plan, &workspace) == UMI_STATUS_OK);
    GtkWidget *root = UmiBrokerWorkspaceGtkWidget(workspace);
    g_object_ref(root);
    gtk_window_set_child(parent, root);
    observed.state = UMI_IBKR_READY;
    observed.readOnly = true;
    observed.accountCount = 2U;
    (void)snprintf(observed.accounts[0], 64U, "TEST-A");
    (void)snprintf(observed.accounts[1], 64U, "TEST-B");
    CHECK(UmiBrokerWorkspaceGtkOpenConnection(workspace) == UMI_STATUS_OK);
    GtkWidget *read = Find(root, "broker-workspace-read");
    GtkWidget *status = Find(root, "broker-workspace-status");





    if (strncmp(argv[1], "risk-", 5U) == 0) {
        CHECK(OptionRiskProjection(argv[1], root, &workspace) == 0);
    } else if (strncmp(argv[1], "table-", 6U) == 0) {
        CHECK(AccountTablesProjection(argv[1], root, workspace) == 0);
    } else if (strncmp(argv[1], "stream-", 7U) == 0) {
        CHECK(StreamProjection(argv[1], root, &workspace) == 0);
    } else if (strncmp(argv[1], "tape-", 5U) == 0) {
        CHECK(TapeProjection(argv[1], root, &workspace) == 0);
    } else if (strncmp(argv[1], "options-", 8U) == 0) {
        CHECK(OptionsProjection(argv[1], root, &workspace) == 0);
    } else if (strncmp(argv[1], "details-", 8U) == 0) {
        CHECK(DetailsProjection(argv[1], root, &workspace) == 0);
    } else if (strncmp(argv[1], "completed-", 10U) == 0) {
        CHECK(CompletedProjection(argv[1], root, &workspace) == 0);
    } else if (strncmp(argv[1], "scanner-", 8U) == 0) {
        CHECK(ScannerProjection(argv[1], root, &workspace) == 0);
    } else if (strncmp(argv[1], "pnl-", 4U) == 0) {
        GtkWidget *start = Find(root, "broker-workspace-pnl-start");
        GtkWidget *stop = Find(root, "broker-workspace-pnl-stop");
        GtkWidget *accounts = Find(root, "broker-workspace-pnl-accounts");
        GtkWidget *model = Find(root, "broker-workspace-pnl-model");
        GtkWidget *contract = Find(root, "broker-workspace-pnl-contract");
        GtkWidget *state = Find(root, "broker-workspace-pnl-status");
        GtkWidget *notice = Find(root, "broker-workspace-pnl-notice");
        GtkWidget *daily = Find(root, "broker-workspace-pnl-value-0");
        GtkWidget *position = Find(root, "broker-workspace-pnl-value-3");
        GtkWidget *evidence = Find(root, "broker-workspace-pnl-evidence-0");
        CHECK(start != NULL && contract != NULL && accounts != NULL);
        CHECK(gtk_widget_get_sensitive(start) && !gtk_widget_get_sensitive(stop));
        CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(accounts))) == 2U);
        gtk_drop_down_set_selected(GTK_DROP_DOWN(accounts), 1U);
        gtk_editable_set_text(GTK_EDITABLE(model), "TEST-MODEL");
        if (strcmp(argv[1], "pnl-retained") == 0) {
            UmiBrokerWorkspaceGtkDestroy(workspace); workspace = NULL;
            g_signal_emit_by_name(start, "clicked"); g_signal_emit_by_name(stop, "clicked");
            CHECK(pnlStarts == 0U && pnlStops == 0U);
        } else if (strcmp(argv[1], "pnl-no-account") == 0) {
            observed.accountCount = 0U;
            CHECK(UmiBrokerWorkspaceGtkRefresh(workspace) == UMI_STATUS_OK);
            CHECK(!gtk_widget_get_sensitive(start));
            g_signal_emit_by_name(start, "clicked"); CHECK(pnlStarts == 0U);
        } else if (strcmp(argv[1], "pnl-external-request") == 0) {
            pnlAvailable = TRUE; pnl.subscribed = pnl.stale = true; pnl.requestId = 43U;
            strcpy(pnl.selection.account, "TEST-A");
            g_signal_emit_by_name(start, "clicked");
            CHECK(pnlStarts == 0U && !gtk_widget_get_sensitive(start) && gtk_widget_get_sensitive(stop));
            CHECK(Contains(state, "TEST-A"));
        } else if (strcmp(argv[1], "pnl-invalid") == 0) {
            const char *invalid[] = {"0", "-1", "2147483648", "1x"};
            for (size_t i = 0U; i < G_N_ELEMENTS(invalid); ++i) {
                gtk_editable_set_text(GTK_EDITABLE(contract), invalid[i]);
                g_signal_emit_by_name(start, "clicked"); CHECK(pnlStarts == 0U);
            }
            CHECK(Contains(notice, "P&L not started") && gtk_widget_get_sensitive(start));
        } else if (strcmp(argv[1], "pnl-rejected") == 0) {
            pnlStartStatus = UMI_STATUS_PERMISSION_DENIED;
            g_signal_emit_by_name(start, "clicked");
            CHECK(pnlStarts == 0U && gtk_widget_get_sensitive(start));
            CHECK(Contains(notice, umi_status_text(pnlStartStatus)));
        } else {
            if (strcmp(argv[1], "pnl-position") == 0) gtk_editable_set_text(GTK_EDITABLE(contract), "12345");
            g_signal_emit_by_name(start, "clicked");
            CHECK(pnlStarts == 1U && !strcmp(pnl.selection.account, "TEST-B"));
            CHECK(!strcmp(pnl.selection.modelCode, "TEST-MODEL"));
            CHECK(!gtk_widget_get_sensitive(start) && gtk_widget_get_sensitive(stop));
            CHECK(!gtk_widget_get_sensitive(accounts) && !gtk_widget_get_sensitive(model) && !gtk_widget_get_sensitive(contract));
            CHECK(Contains(daily, "Waiting") && Contains(evidence, "No observation"));
            g_signal_emit_by_name(start, "clicked"); CHECK(pnlStarts == 1U);
            if (strcmp(argv[1], "pnl-stop") == 0 || strcmp(argv[1], "pnl-stop-failure") == 0) {
                if (strcmp(argv[1], "pnl-stop-failure") == 0) pnlStopStatus = UMI_STATUS_CAPACITY_EXCEEDED;
                g_signal_emit_by_name(stop, "clicked");
                if (pnlStopStatus == UMI_STATUS_OK)
                    CHECK(pnlStops == 1U && Contains(state, "Stopped") && gtk_widget_get_sensitive(start));
                else CHECK(pnlStops == 0U && pnl.subscribed && gtk_widget_get_sensitive(stop));
            } else if (strcmp(argv[1], "pnl-request-replaced") == 0) {
                ++pnl.requestId; g_signal_emit_by_name(stop, "clicked");
                CHECK(pnlStops == 0U && pnl.subscribed && Contains(notice, "not stopped"));
            } else {
                PnlValues();
                if (strcmp(argv[1], "pnl-missing") == 0) {
                    pnl.daily.exact = false; strcpy(pnl.daily.reportedText, "1.7976931348623157e308");
                }
                if (strcmp(argv[1], "pnl-stale") == 0) pnl.stale = true;
                if (strcmp(argv[1], "pnl-failed") == 0) pnl.failed = pnl.stale = true;
                if (strcmp(argv[1], "pnl-utf8") == 0) memcpy(pnl.message, "Invalid \xff", sizeof("Invalid \xff"));
                if (strcmp(argv[1], "pnl-reentrant") == 0)
                    g_signal_connect(daily, "notify::label", G_CALLBACK(CloseDuringRender), &workspace);
                UmiStatus refreshed = UmiBrokerWorkspaceGtkRefresh(workspace);
                if (strcmp(argv[1], "pnl-reentrant") == 0) {
                    CHECK(workspace == NULL && refreshed == UMI_STATUS_CANCELLED);
                    g_signal_handlers_disconnect_by_data(daily, &workspace);
                } else {
                    CHECK(refreshed == UMI_STATUS_OK && Contains(state, "TEST-B") && Contains(state, "TEST-MODEL"));
                    CHECK(Contains(Find(root, "broker-workspace-pnl-value-1"), "0"));
                    CHECK(Contains(Find(root, "broker-workspace-pnl-value-2"), "9007199254740993"));
                    CHECK(UmiBrokerWorkspaceGtkRefresh(workspace) == UMI_STATUS_OK && pnlStarts == 1U);
                    if (strcmp(argv[1], "pnl-missing") == 0) {
                        CHECK(!strcmp(gtk_label_get_text(GTK_LABEL(daily)), "Unavailable"));
                        CHECK(Contains(evidence, "1.7976931348623157e308"));
                    } else CHECK(Contains(daily, "-123.450000001"));
                    if (strcmp(argv[1], "pnl-position") == 0) {
                        CHECK(pnl.selection.contractId == 12345U && Contains(position, "-2"));
                        CHECK(Contains(Find(root, "broker-workspace-pnl-value-4"), "10.123456789"));
                    } else CHECK(Contains(position, "Not part of account"));
                    if (strcmp(argv[1], "pnl-stale") == 0 || strcmp(argv[1], "pnl-failed") == 0) {
                        CHECK(Contains(state, "STALE") && Contains(evidence, "STALE") && gtk_widget_get_sensitive(stop));
                        if (pnl.failed) CHECK(Contains(state, "Failed"));
                    } else if (strcmp(argv[1], "pnl-reconnect") == 0) {
                        gtk_window_destroy(inspector);
                        CHECK(Contains(state, "STALE") && !gtk_widget_get_sensitive(stop));
                        pnlAvailable = FALSE;
                        CHECK(UmiBrokerWorkspaceGtkOpenConnection(workspace) == UMI_STATUS_OK);
                        CHECK(Contains(daily, "Waiting") && !Contains(state, "TEST-B"));
                    } else if (strcmp(argv[1], "pnl-scope-identity") == 0) {
                        gtk_drop_down_set_selected(GTK_DROP_DOWN(accounts), 0U);
                        gtk_editable_set_text(GTK_EDITABLE(model), "OTHER");
                        gtk_editable_set_text(GTK_EDITABLE(contract), "999");
                        CHECK(UmiBrokerWorkspaceGtkRefresh(workspace) == UMI_STATUS_OK);
                        CHECK(Contains(state, "TEST-B") && Contains(state, "TEST-MODEL") && !Contains(state, "OTHER"));
                    } else if (strcmp(argv[1], "pnl-utf8") == 0) {
                        CHECK(Contains(state, "Invalid \xef\xbf\xbd"));
                    } else if (strcmp(argv[1], "pnl-account") != 0 && strcmp(argv[1], "pnl-position") != 0 &&
                        strcmp(argv[1], "pnl-missing") != 0) return 2;
                }
            }
        }
        CHECK(subscriptions == 0U && cancellations == 0U && reads == 0U && orderReads == 0U);
    } else
    if (strncmp(argv[1], "depth-", 6U) == 0) {
        GtkWidget *start = Find(root, "broker-workspace-depth-start");
        GtkWidget *stop = Find(root, "broker-workspace-depth-stop");
        GtkWidget *smart = Find(root, "broker-workspace-depth-smart");
        GtkWidget *rows = Find(root, "broker-workspace-depth-rows");
        GtkWidget *state = Find(root, "broker-workspace-depth-status");
        GtkWidget *notice = Find(root, "broker-workspace-depth-notice");
        GtkWidget *contract = Find(root, "broker-workspace-contract");
        GtkWidget *exchange = Find(root, "broker-workspace-exchange");
        GtkWidget *bid = Find(root, "broker-workspace-depth-cell-0-2");
        GtkWidget *ask = Find(root, "broker-workspace-depth-cell-0-5");
        CHECK(start != NULL && contract != NULL && exchange != NULL);
        CHECK(gtk_widget_get_sensitive(start) && !gtk_widget_get_sensitive(stop));
        gtk_editable_set_text(GTK_EDITABLE(contract), "12345");
        gtk_editable_set_text(GTK_EDITABLE(exchange), "SYNTHETIC");
        if (strcmp(argv[1], "depth-retained") == 0) {
            UmiBrokerWorkspaceGtkDestroy(workspace); workspace = NULL;
            g_signal_emit_by_name(start, "clicked"); g_signal_emit_by_name(stop, "clicked");
            CHECK(depthStarts == 0U && depthStops == 0U);
        } else if (strcmp(argv[1], "depth-external-request") == 0) {
            depthAvailable = TRUE; depth.subscribed = depth.stale = true; depth.requestId = 42U;
            g_signal_emit_by_name(start, "clicked");
            CHECK(depthStarts == 0U && !gtk_widget_get_sensitive(start));
            CHECK(gtk_widget_get_sensitive(stop));
        } else if (strcmp(argv[1], "depth-invalid") == 0) {
            gtk_editable_set_text(GTK_EDITABLE(contract), "-1");
            g_signal_emit_by_name(start, "clicked"); CHECK(depthStarts == 0U);
            gtk_editable_set_text(GTK_EDITABLE(contract), "2147483648");
            g_signal_emit_by_name(start, "clicked"); CHECK(depthStarts == 0U);
            gtk_editable_set_text(GTK_EDITABLE(contract), "12345");
            gtk_editable_set_text(GTK_EDITABLE(exchange), "");
            g_signal_emit_by_name(start, "clicked"); CHECK(depthStarts == 0U);
            CHECK(Contains(notice, "Depth not started") && gtk_widget_get_sensitive(start));
        } else if (strcmp(argv[1], "depth-mismatch") == 0) {
            gtk_check_button_set_active(GTK_CHECK_BUTTON(smart), TRUE);
            g_signal_emit_by_name(start, "clicked"); CHECK(depthStarts == 0U);
            gtk_check_button_set_active(GTK_CHECK_BUTTON(smart), FALSE);
            gtk_editable_set_text(GTK_EDITABLE(exchange), "SMART");
            g_signal_emit_by_name(start, "clicked"); CHECK(depthStarts == 0U);
            CHECK(Contains(notice, "Depth not started"));
        } else if (strcmp(argv[1], "depth-rejected") == 0) {
            depthStartStatus = UMI_STATUS_CAPACITY_EXCEEDED;
            g_signal_emit_by_name(start, "clicked");
            CHECK(depthStarts == 0U && gtk_widget_get_sensitive(start));
            CHECK(Contains(notice, umi_status_text(depthStartStatus)));
        } else {
            if (strcmp(argv[1], "depth-smart") == 0) {
                gtk_check_button_set_active(GTK_CHECK_BUTTON(smart), TRUE);
                gtk_editable_set_text(GTK_EDITABLE(exchange), "SMART");
            }
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(rows), 7.0);
            g_signal_emit_by_name(start, "clicked");
            CHECK(depthStarts == 1U && depth.selection.contractId == 12345U && depth.selection.rows == 7U);
            CHECK(!gtk_widget_get_sensitive(start) && gtk_widget_get_sensitive(stop));
            CHECK(!gtk_widget_get_sensitive(rows) && !gtk_widget_get_sensitive(smart));
            CHECK(!gtk_widget_get_visible(bid) && Contains(state, "0 bids / 0 asks"));
            g_signal_emit_by_name(start, "clicked"); CHECK(depthStarts == 1U);
            if (strcmp(argv[1], "depth-stop") == 0 || strcmp(argv[1], "depth-stop-failure") == 0) {
                if (strcmp(argv[1], "depth-stop-failure") == 0) depthStopStatus = UMI_STATUS_CAPACITY_EXCEEDED;
                g_signal_emit_by_name(stop, "clicked");
                if (depthStopStatus == UMI_STATUS_OK)
                    CHECK(depthStops == 1U && Contains(state, "Stopped") && gtk_widget_get_sensitive(start));
                else CHECK(depthStops == 0U && depth.subscribed && gtk_widget_get_sensitive(stop));
            } else if (strcmp(argv[1], "depth-request-replaced") == 0) {
                ++depth.requestId;
                g_signal_emit_by_name(stop, "clicked");
                CHECK(depthStops == 0U && depth.subscribed && Contains(notice, "not stopped"));
            } else {
                DepthRows();
                if (strcmp(argv[1], "depth-one-sided") == 0) depth.askCount = 0U;
                if (strcmp(argv[1], "depth-row-stale") == 0) depth.bids[1].stale = true;
                if (strcmp(argv[1], "depth-stale") == 0) depth.stale = true;
                if (strcmp(argv[1], "depth-failed") == 0) depth.failed = depth.stale = true;
                if (strcmp(argv[1], "depth-utf8") == 0)
                    memcpy(depth.bids[0].marketMaker, "TEST \xff", sizeof("TEST \xff"));
                if (strcmp(argv[1], "depth-reentrant") == 0)
                    g_signal_connect(bid, "notify::label", G_CALLBACK(CloseDuringRender), &workspace);
                UmiStatus refreshed = UmiBrokerWorkspaceGtkRefresh(workspace);
                if (strcmp(argv[1], "depth-reentrant") == 0) {
                    CHECK(workspace == NULL && refreshed == UMI_STATUS_CANCELLED);
                    g_signal_handlers_disconnect_by_data(bid, &workspace);
                } else {
                    CHECK(refreshed == UMI_STATUS_OK && gtk_widget_get_visible(bid));
                    CHECK(Contains(bid, "12.345678901"));
                    CHECK(Contains(Find(root, "broker-workspace-depth-cell-0-1"), "9007199254740993"));
                    CHECK(Contains(Find(root, "broker-workspace-depth-cell-1-1"), "0"));
                    CHECK(Contains(state, "12345"));
                    CHECK(!gtk_widget_get_visible(Find(root, "broker-workspace-depth-cell-2-0")));
                    CHECK(UmiBrokerWorkspaceGtkRefresh(workspace) == UMI_STATUS_OK);
                    CHECK(bid == Find(root, "broker-workspace-depth-cell-0-2") && depthStarts == 1U);
                    if (strcmp(argv[1], "depth-one-sided") == 0) {
                        CHECK(Contains(ask, "—") && Contains(Find(root, "broker-workspace-depth-cell-0-8"), "Not received"));
                    } else CHECK(Contains(ask, "12.345678902"));
                    if (strcmp(argv[1], "depth-row-stale") == 0) {
                        CHECK(Contains(Find(root, "broker-workspace-depth-cell-1-3"), "STALE"));
                        CHECK(Contains(Find(root, "broker-workspace-depth-cell-0-3"), "Observed"));
                    } else if (strcmp(argv[1], "depth-stale") == 0 || strcmp(argv[1], "depth-failed") == 0) {
                        CHECK(Contains(Find(root, "broker-workspace-depth-cell-0-3"), "STALE"));
                        CHECK(Contains(Find(root, "broker-workspace-depth-cell-0-8"), "STALE"));
                        CHECK(gtk_widget_get_sensitive(stop));
                        if (depth.failed) CHECK(Contains(state, "Failed") && !gtk_widget_get_sensitive(start));
                    } else if (strcmp(argv[1], "depth-reset") == 0) {
                        depth.bidCount = depth.askCount = 0U; ++depth.resetCount; depth.received = false; depth.stale = true;
                        CHECK(UmiBrokerWorkspaceGtkRefresh(workspace) == UMI_STATUS_OK);
                        CHECK(!gtk_widget_get_visible(bid) && !gtk_widget_get_visible(ask));
                        CHECK(Contains(state, "resets 1") && gtk_widget_get_sensitive(stop));
                    } else if (strcmp(argv[1], "depth-reconnect") == 0) {
                        gtk_window_destroy(inspector);
                        CHECK(Contains(state, "STALE") && !gtk_widget_get_sensitive(stop));
                        depthAvailable = FALSE;
                        CHECK(UmiBrokerWorkspaceGtkOpenConnection(workspace) == UMI_STATUS_OK);
                        CHECK(!gtk_widget_get_visible(bid) && gtk_widget_get_sensitive(start));
                    } else if (strcmp(argv[1], "depth-query-identity") == 0) {
                        gtk_editable_set_text(GTK_EDITABLE(contract), "999");
                        gtk_editable_set_text(GTK_EDITABLE(exchange), "OTHER");
                        CHECK(UmiBrokerWorkspaceGtkRefresh(workspace) == UMI_STATUS_OK);
                        CHECK(Contains(state, "12345") && Contains(state, "SYNTHETIC") && !Contains(state, "OTHER"));
                    } else if (strcmp(argv[1], "depth-utf8") == 0) {
                        CHECK(Contains(Find(root, "broker-workspace-depth-cell-0-0"), "TEST \xef\xbf\xbd"));
                    } else if (strcmp(argv[1], "depth-smart") == 0) {
                        CHECK(depth.selection.smartDepth && !strcmp(depth.selection.exchange, "SMART"));
                        CHECK(Contains(state, "SMART aggregated depth"));
                    } else if (strcmp(argv[1], "depth-request") != 0 && strcmp(argv[1], "depth-one-sided") != 0) return 2;
                }
            }
        }
        CHECK(subscriptions == 0U && cancellations == 0U && reads == 0U && orderReads == 0U);
    } else
    if (strncmp(argv[1], "history-", 8U) == 0) {
        GtkWidget *start = Find(root, "broker-workspace-history-read");
        GtkWidget *cancel = Find(root, "broker-workspace-history-cancel");
        GtkWidget *visible = Find(root, "broker-workspace-history-visible");
        GtkWidget *offset = Find(root, "broker-workspace-history-offset");
        GtkWidget *state = Find(root, "broker-workspace-history-status");
        GtkWidget *details = Find(root, "broker-workspace-history-details");
        GtkWidget *notice = Find(root, "broker-workspace-history-notice");
        GtkWidget *contract = Find(root, "broker-workspace-contract");
        GtkWidget *exchange = Find(root, "broker-workspace-exchange");
        CHECK(start != NULL && gtk_widget_get_sensitive(start));
        CHECK(!gtk_widget_get_sensitive(cancel));
        gtk_editable_set_text(GTK_EDITABLE(contract), "12345");
        gtk_editable_set_text(GTK_EDITABLE(exchange), "SYNTHETIC");
        if (strcmp(argv[1], "history-retained") == 0) {
            UmiBrokerWorkspaceGtkDestroy(workspace); workspace = NULL;
            g_signal_emit_by_name(start, "clicked"); g_signal_emit_by_name(cancel, "clicked");
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(visible), 3.0);
            CHECK(historyReads == 0U && historyCancels == 0U && historyScenes == 0U);
        } else if (strcmp(argv[1], "history-external-request") == 0) {
            historyAvailable = TRUE; history.pending = history.stale = true; history.requestId = 41U;
            g_signal_emit_by_name(start, "clicked");
            CHECK(historyReads == 0U && !gtk_widget_get_sensitive(start));
        } else if (strcmp(argv[1], "history-invalid") == 0) {
            gtk_editable_set_text(GTK_EDITABLE(contract), "0");
            g_signal_emit_by_name(start, "clicked");
            CHECK(historyReads == 0U && Contains(notice, "Capture not started"));
            gtk_editable_set_text(GTK_EDITABLE(contract), "12345");
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(Find(root, "broker-workspace-history-duration")), 86400.0);
            g_signal_emit_by_name(start, "clicked");
            CHECK(historyReads == 0U); /* Too many one-minute bars for the bounded capture. */
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(Find(root, "broker-workspace-history-duration")), 3600.0);
            gtk_editable_set_text(GTK_EDITABLE(Find(root, "broker-workspace-history-end")), "20260230 12:00:00 UTC");
            g_signal_emit_by_name(start, "clicked");
            CHECK(historyReads == 0U && gtk_widget_get_sensitive(start));
        } else if (strcmp(argv[1], "history-pacing") == 0) {
            historyReadStatus = UMI_STATUS_BUSY;
            g_signal_emit_by_name(start, "clicked");
            CHECK(historyReads == 0U && Contains(notice, "15 seconds") && gtk_widget_get_sensitive(start));
        } else {
            g_signal_emit_by_name(start, "clicked");
            CHECK(historyReads == 1U && history.query.contract.contractId == 12345U);
            CHECK(!strcmp(history.query.contract.exchange, "SYNTHETIC"));
            CHECK(history.query.barSeconds == 60U && history.query.durationSeconds == 3600U);
            CHECK(!gtk_widget_get_sensitive(start) && gtk_widget_get_sensitive(cancel));
            g_signal_emit_by_name(start, "clicked"); CHECK(historyReads == 1U);
            if (strcmp(argv[1], "history-cancel") == 0 || strcmp(argv[1], "history-cancel-failure") == 0) {
                if (strcmp(argv[1], "history-cancel-failure") == 0) historyCancelStatus = UMI_STATUS_CAPACITY_EXCEEDED;
                g_signal_emit_by_name(cancel, "clicked");
                if (historyCancelStatus == UMI_STATUS_OK)
                    CHECK(historyCancels == 1U && Contains(state, "Cancelled") && gtk_widget_get_sensitive(start));
                else CHECK(historyCancels == 0U && history.pending && gtk_widget_get_sensitive(cancel));
            } else if (strcmp(argv[1], "history-request-replaced") == 0) {
                ++history.requestId;
                g_signal_emit_by_name(cancel, "clicked");
                CHECK(historyCancels == 0U && history.pending);
            } else {
                HistoryRows();
                if (strcmp(argv[1], "history-empty") == 0) history.count = 0U;
                if (strcmp(argv[1], "history-volume-unavailable") == 0) historyBar.volumeAvailable = false;
                if (strcmp(argv[1], "history-timeout") == 0) {
                    history.complete = false; history.failed = history.stale = true;
                    strcpy(history.message, "Synthetic historical capture timed out.");
                }
                if (strcmp(argv[1], "history-render-unavailable") == 0) historySceneStatus = UMI_STATUS_NOT_IMPLEMENTED;
                if (strcmp(argv[1], "history-utf8") == 0)
                    memcpy(history.message, "Invalid \xff", sizeof("Invalid \xff"));
                if (strcmp(argv[1], "history-reentrant") == 0)
                    g_signal_connect(state, "notify::label", G_CALLBACK(CloseDuringRender), &workspace);
                UmiStatus refreshed = UmiBrokerWorkspaceGtkRefresh(workspace);
                if (strcmp(argv[1], "history-reentrant") == 0) {
                    CHECK(workspace == NULL && refreshed == UMI_STATUS_CANCELLED);
                    g_signal_handlers_disconnect_by_data(state, &workspace);
                } else {
                    CHECK(refreshed == UMI_STATUS_OK);
                    CHECK(Contains(state, "12345") && Contains(state, "SYNTHETIC"));
                    if (strcmp(argv[1], "history-empty") == 0)
                        CHECK(Contains(details, "Completed capture contains no bars"));
                    else if (strcmp(argv[1], "history-timeout") == 0)
                        CHECK(Contains(state, "Failed") && Contains(details, "complete, current capture"));
                    else {
                        CHECK(Contains(details, "Showing bars 21") && Contains(details, "UTC"));
                        CHECK(Contains(details, "12.000000001") && Contains(details, "12.100000001"));
                        CHECK(Contains(details, historyBar.volumeAvailable ? "9007199254740993" : "Volume Unavailable"));
#ifdef UMI_IBKR_HAS_HISTORY_MODEL
                        CHECK(historyScenes == 1U && historyFirst == 20U && historyCount == 80U);
                        CHECK(UmiBrokerWorkspaceGtkRefresh(workspace) == UMI_STATUS_OK);
                        CHECK(historyScenes == 1U); /* No repeated scene allocations for an unchanged range. */
#endif
                        if (strcmp(argv[1], "history-range") == 0 || strcmp(argv[1], "history-range-clamp") == 0) {
                            gtk_spin_button_set_value(GTK_SPIN_BUTTON(visible), 20.0);
                            gtk_spin_button_set_value(GTK_SPIN_BUTTON(offset), 10.0);
                            CHECK(Contains(details, "Showing bars 71") && historyReads == 1U);
#ifdef UMI_IBKR_HAS_HISTORY_MODEL
                            CHECK(historyFirst == 70U && historyCount == 20U);
#endif
                            if (strcmp(argv[1], "history-range-clamp") == 0) {
                                gtk_spin_button_set_value(GTK_SPIN_BUTTON(offset), 511.0);
                                CHECK(Contains(details, "Showing bars 1") && historyReads == 1U);
#ifdef UMI_IBKR_HAS_HISTORY_MODEL
                                CHECK(historyFirst == 0U && historyCount == 1U);
#endif
                            }
                        } else if (strcmp(argv[1], "history-stale") == 0) {
                            history.stale = true;
                            CHECK(UmiBrokerWorkspaceGtkRefresh(workspace) == UMI_STATUS_OK);
                            CHECK(Contains(state, "STALE") && !Contains(details, "12.100000001"));
                        } else if (strcmp(argv[1], "history-reconnect") == 0) {
                            gtk_window_destroy(inspector);
                            CHECK(Contains(state, "STALE") && !Contains(details, "12.100000001"));
                            historyAvailable = FALSE;
                            CHECK(UmiBrokerWorkspaceGtkOpenConnection(workspace) == UMI_STATUS_OK);
                            CHECK(!Contains(state, "12345") && gtk_widget_get_sensitive(start));
                        } else if (strcmp(argv[1], "history-query-identity") == 0) {
                            gtk_editable_set_text(GTK_EDITABLE(contract), "999");
                            gtk_editable_set_text(GTK_EDITABLE(exchange), "OTHER");
                            CHECK(UmiBrokerWorkspaceGtkRefresh(workspace) == UMI_STATUS_OK);
                            CHECK(Contains(state, "12345") && Contains(state, "SYNTHETIC") && !Contains(state, "OTHER"));
                        } else if (strcmp(argv[1], "history-utf8") == 0) CHECK(Contains(state, "Invalid \xef\xbf\xbd"));
                        else if (strcmp(argv[1], "history-render-unavailable") == 0) {
#ifdef UMI_IBKR_HAS_HISTORY_MODEL
                            CHECK(Contains(details, "Chart unavailable"));
#endif
                        } else if (strcmp(argv[1], "history-request") != 0 &&
                            strcmp(argv[1], "history-volume-unavailable") != 0) return 2;
                    }
                }
            }
        }
    } else
    if (strncmp(argv[1], "trades-", 7U) == 0) {
        GtkWidget *start = Find(root, "broker-workspace-trades-read");
        GtkWidget *abandon = Find(root, "broker-workspace-trades-abandon");
        GtkWidget *account = Find(root, "broker-workspace-trades-accounts");
        GtkWidget *table = Find(root, "broker-workspace-trades-table");
        GtkWidget *state = Find(root, "broker-workspace-trades-status");
        GtkWidget *notice = Find(root, "broker-workspace-trades-notice");
        CHECK(start != NULL && gtk_widget_get_sensitive(start));
        CHECK(!gtk_widget_get_sensitive(abandon));
        CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(account))) == 2U);
        gtk_drop_down_set_selected(GTK_DROP_DOWN(account), 1U);
        if (strcmp(argv[1], "trades-retained") == 0) {
            UmiBrokerWorkspaceGtkDestroy(workspace); workspace = NULL;
            g_signal_emit_by_name(start, "clicked"); g_signal_emit_by_name(abandon, "clicked");
            CHECK(tradeReads == 0U && tradeAbandons == 0U);
        } else if (strcmp(argv[1], "trades-external-request") == 0) {
            tradesAvailable = TRUE; trades.requestId = 99U; strcpy(trades.account, "TEST-A");
            g_signal_emit_by_name(start, "clicked");
            CHECK(tradeReads == 0U && !gtk_widget_get_sensitive(start));
            CHECK(Contains(state, "TEST-A"));
        } else if (strcmp(argv[1], "trades-rejected") == 0) {
            tradeReadResult = UMI_STATUS_NOT_IMPLEMENTED;
            g_signal_emit_by_name(start, "clicked");
            CHECK(tradeReads == 0U && gtk_widget_get_sensitive(start));
            CHECK(Contains(notice, umi_status_text(tradeReadResult)));
        } else {
            g_signal_emit_by_name(start, "clicked");
            CHECK(tradeReads == 1U && !strcmp(trades.account, "TEST-B"));
            CHECK(!gtk_widget_get_sensitive(start) && !gtk_widget_get_sensitive(account));
            CHECK(gtk_widget_get_sensitive(abandon));
            g_signal_emit_by_name(start, "clicked"); CHECK(tradeReads == 1U);
            if (strcmp(argv[1], "trades-abandon") == 0) {
                g_signal_emit_by_name(abandon, "clicked");
                CHECK(tradeAbandons == 1U && Contains(state, "STALE"));
                CHECK(gtk_widget_get_sensitive(start) && !gtk_widget_get_sensitive(abandon));
            } else if (strcmp(argv[1], "trades-changed-request") == 0) {
                ++trades.requestId;
                g_signal_emit_by_name(abandon, "clicked");
                CHECK(tradeAbandons == 0U && Contains(notice, "capture changed"));
            } else {
                TradeRows();
                if (strcmp(argv[1], "trades-empty") == 0) trades.count = 0U;
                if (strcmp(argv[1], "trades-partial") == 0) trades.complete = false;
                if (strcmp(argv[1], "trades-stale") == 0) trades.stale = true;
                if (strcmp(argv[1], "trades-utf8") == 0)
                    memcpy(trades.rows[0].symbol, "EXAMPLE \xff", sizeof("EXAMPLE \xff"));
                if (strcmp(argv[1], "trades-reentrant") == 0)
                    g_signal_connect(state, "notify::label", G_CALLBACK(CloseDuringRender), &workspace);
                UmiStatus refreshed = UmiBrokerWorkspaceGtkRefresh(workspace);
                if (strcmp(argv[1], "trades-reentrant") == 0) {
                    CHECK(workspace == NULL && refreshed == UMI_STATUS_CANCELLED);
                    g_signal_handlers_disconnect_by_data(state, &workspace);
                } else {
                    CHECK(refreshed == UMI_STATUS_OK);
                    if (strcmp(argv[1], "trades-empty") == 0)
                        CHECK(Contains(table, "Completed capture contains no recent executions"));
                    else {
                        CHECK(Contains(table, "SYNTHETIC-FILL.01") && Contains(table, "TEST-B"));
                        CHECK(Contains(table, "9007199254740993") && Contains(table, "0.000000001"));
                        CHECK(Contains(table, "BrokerZone") && Contains(table, "Not received"));
                        if (strcmp(argv[1], "trades-utf8") == 0) {
                            CHECK(Contains(table, "EXAMPLE \xef\xbf\xbd"));
                            CHECK((unsigned char)trades.rows[0].symbol[8] == 0xffU);
                        }
                    }
                    if (strcmp(argv[1], "trades-partial") == 0) {
                        CHECK(Contains(state, "Receiving") && !gtk_widget_get_sensitive(start));
                    } else if (strcmp(argv[1], "trades-stale") == 0) CHECK(Contains(state, "STALE"));
                    else if (strcmp(argv[1], "trades-reconnect") == 0) {
                        gtk_window_destroy(inspector);
                        CHECK(Contains(state, "STALE") && !gtk_widget_get_sensitive(start));
                        tradesAvailable = FALSE;
                        CHECK(UmiBrokerWorkspaceGtkOpenConnection(workspace) == UMI_STATUS_OK);
                        CHECK(!Contains(table, "SYNTHETIC-FILL.01") && gtk_widget_get_sensitive(start));
                    } else if (strcmp(argv[1], "trades-account-change") == 0) {
                        gtk_drop_down_set_selected(GTK_DROP_DOWN(account), 0U);
                        CHECK(UmiBrokerWorkspaceGtkRefresh(workspace) == UMI_STATUS_OK);
                        CHECK(Contains(state, "TEST-B") && Contains(table, "TEST-B"));
                        g_signal_emit_by_name(start, "clicked");
                        CHECK(tradeReads == 2U && !strcmp(trades.account, "TEST-A"));
                        CHECK(!Contains(table, "SYNTHETIC-FILL.01"));
                    } else if (strcmp(argv[1], "trades-late-fees") == 0 ||
                        strcmp(argv[1], "trades-conflicting-fees") == 0) {
                        UmiIbkrExecutionFee *fee = &trades.fees[0];
                        fee->received = fee->exact = true;
                        strcpy(fee->firstReportedAmount, "-0.000000001"); strcpy(fee->firstCurrency, "USD");
                        CHECK(UmiBrokerWorkspaceGtkRefresh(workspace) == UMI_STATUS_OK);
                        CHECK(Contains(table, "-0.000000001 USD") && !Contains(table, "Not received"));
                        if (strcmp(argv[1], "trades-conflicting-fees") == 0) {
                            fee->conflicting = true; strcpy(fee->latestReportedAmount, "2.500");
                            strcpy(fee->latestCurrency, "EUR");
                            CHECK(UmiBrokerWorkspaceGtkRefresh(workspace) == UMI_STATUS_OK);
                            CHECK(Contains(table, "First -0.000000001 USD; latest 2.500 EUR"));
                            CHECK(Contains(table, "Conflicting reports"));
                        }
                    } else if (strcmp(argv[1], "trades-select") != 0 &&
                        strcmp(argv[1], "trades-rows") != 0 && strcmp(argv[1], "trades-empty") != 0 &&
                        strcmp(argv[1], "trades-utf8") != 0) return 2;
                }
            }
        }
    } else
    if (strncmp(argv[1], "orders-", 7U) == 0) {
        GtkWidget *orderRead = Find(root, "broker-workspace-orders-read");
        GtkWidget *all = Find(root, "broker-workspace-orders-all");
        GtkWidget *table = Find(root, "broker-workspace-orders-table");
        GtkWidget *state = Find(root, "broker-workspace-orders-status");
        GtkWidget *notice = Find(root, "broker-workspace-orders-notice");
        CHECK(orderRead != NULL && gtk_widget_get_sensitive(orderRead));
        if (strcmp(argv[1], "orders-retained") == 0) {
            UmiBrokerWorkspaceGtkDestroy(workspace); workspace = NULL;
            g_signal_emit_by_name(orderRead, "clicked"); CHECK(orderReads == 0U);
        } else if (strcmp(argv[1], "orders-external-request") == 0) {
            orderAvailable = TRUE; orderReport.capture.requested = true;
            orderReport.capture.scope = UMI_IBKR_ORDERS_ALL_CLIENTS;
            g_signal_emit_by_name(orderRead, "clicked");
            CHECK(orderReads == 0U && !gtk_widget_get_sensitive(orderRead));
            CHECK(Contains(state, "All broker-visible"));
            CHECK(gtk_check_button_get_active(GTK_CHECK_BUTTON(all)));
        } else if (strcmp(argv[1], "orders-rejected") == 0) {
            orderReadResult = UMI_STATUS_CAPACITY_EXCEEDED;
            g_signal_emit_by_name(orderRead, "clicked");
            CHECK(orderReads == 0U && gtk_widget_get_sensitive(orderRead));
            CHECK(Contains(notice, umi_status_text(UMI_STATUS_CAPACITY_EXCEEDED)));
        } else {
            gboolean allClients = strcmp(argv[1], "orders-all") == 0;
            gtk_check_button_set_active(GTK_CHECK_BUTTON(all), allClients);
            g_signal_emit_by_name(orderRead, "clicked");
            CHECK(orderReads == 1U && !gtk_widget_get_sensitive(orderRead));
            CHECK(!gtk_widget_get_sensitive(all));
            CHECK(orderReport.capture.scope == (allClients ? UMI_IBKR_ORDERS_ALL_CLIENTS : UMI_IBKR_ORDERS_THIS_CLIENT));
            CHECK(Contains(state, "Receiving"));
            g_signal_emit_by_name(orderRead, "clicked"); CHECK(orderReads == 1U);
            OrderRows();
            if (strcmp(argv[1], "orders-empty") == 0) orderReport.capture.count = 0U;
            if (strcmp(argv[1], "orders-manual") == 0) {
                orderReport.rows[0].key.clientId = orderReport.rows[0].key.orderId = 0;
                orderReport.rows[1] = orderReport.rows[0];
                orderReport.rows[1].permanentId = 801U;
                orderReport.capture.count = 2U;
            }
            if (strcmp(argv[1], "orders-stale") == 0) orderReport.capture.stale = true;
            if (strcmp(argv[1], "orders-utf8") == 0)
                memcpy(orderReport.rows[0].open.localSymbol, "Synthetic \xff", sizeof("Synthetic \xff"));
            if (strcmp(argv[1], "orders-reentrant") == 0)
                g_signal_connect(state, "notify::label", G_CALLBACK(CloseDuringRender), &workspace);
            UmiStatus refreshed = UmiBrokerWorkspaceGtkRefresh(workspace);
            if (strcmp(argv[1], "orders-reentrant") == 0) {
                CHECK(workspace == NULL && refreshed == UMI_STATUS_CANCELLED);
                g_signal_handlers_disconnect_by_data(state, &workspace);
            } else {
                CHECK(refreshed == UMI_STATUS_OK);
                if (strcmp(argv[1], "orders-empty") == 0)
                    CHECK(Contains(table, "Completed capture contains no orders"));
                else {
                    CHECK(Contains(table, "TEST-OTHER"));
                    if (strcmp(argv[1], "orders-manual") == 0)
                        CHECK(Contains(table, "0 / 0") && Contains(table, "800") && Contains(table, "801"));
                    else CHECK(Contains(table, "99 / 12"));
                    CHECK(Contains(table, "9007199254740993.123456789") && Contains(table, "0.000000001"));
                    CHECK(Contains(table, "Not received") && Contains(table, "STALE / partial"));
                    if (strcmp(argv[1], "orders-utf8") == 0)
                        CHECK(Contains(table, "Synthetic \xef\xbf\xbd"));
                }
                if (strcmp(argv[1], "orders-stale") == 0) CHECK(Contains(state, "STALE"));
                if (strcmp(argv[1], "orders-reconnect") == 0) {
                    gtk_window_destroy(inspector);
                    CHECK(Contains(state, "STALE") && !gtk_widget_get_sensitive(orderRead));
                    orderAvailable = TRUE; memset(&orderReport, 0, sizeof(orderReport));
                    CHECK(UmiBrokerWorkspaceGtkOpenConnection(workspace) == UMI_STATUS_OK);
                    CHECK(!Contains(table, "TEST-OTHER") && gtk_widget_get_sensitive(orderRead));
                }
            }
        }
    } else
    if (strncmp(argv[1], "search-", 7U) == 0) {
        GtkWidget *start = Find(root, "broker-workspace-search-start");
        GtkWidget *abandon = Find(root, "broker-workspace-search-abandon");
        GtkWidget *apply = Find(root, "broker-workspace-search-apply");
        GtkWidget *pattern = Find(root, "broker-workspace-search-pattern");
        GtkWidget *results = Find(root, "broker-workspace-search-results");
        GtkWidget *searchStatus = Find(root, "broker-workspace-search-status");
        GtkWidget *notice = Find(root, "broker-workspace-search-notice");
        GtkWidget *identity = Find(root, "broker-workspace-contract");
        GtkWidget *exchange = Find(root, "broker-workspace-exchange");
        CHECK(start != NULL && gtk_widget_get_sensitive(start));
        CHECK(!gtk_widget_get_sensitive(apply));
        gtk_editable_set_text(GTK_EDITABLE(pattern), "EXAMPLE");
        gtk_editable_set_text(GTK_EDITABLE(identity), "999");
        gtk_editable_set_text(GTK_EDITABLE(exchange), "OLD-ROUTE");
        if (strcmp(argv[1], "search-retained") == 0) {
            UmiBrokerWorkspaceGtkDestroy(workspace); workspace = NULL;
            g_signal_emit_by_name(start, "clicked");
            g_signal_emit_by_name(abandon, "clicked"); g_signal_emit_by_name(apply, "clicked");
            CHECK(searches == 0U && abandons == 0U);
        } else {
            g_signal_emit_by_name(start, "clicked");
            CHECK(searches == 1U && strcmp(search.pattern, "EXAMPLE") == 0);
            CHECK(!gtk_widget_get_sensitive(start) && gtk_widget_get_sensitive(abandon));
            CHECK(!gtk_widget_get_sensitive(apply));
            if (strcmp(argv[1], "search-abandon") == 0) {
                g_signal_emit_by_name(abandon, "clicked");
                CHECK(abandons == 1U && Contains(searchStatus, "STALE"));
                CHECK(gtk_widget_get_sensitive(start) && !gtk_widget_get_sensitive(apply));
            } else {
                SearchResults();
                if (strcmp(argv[1], "search-empty") == 0) search.count = 0U;
                if (strcmp(argv[1], "search-utf8") == 0)
                    memcpy(search.rows[1].description, "Example \xff", sizeof("Example \xff"));
                if (strcmp(argv[1], "search-reentrant") == 0)
                    g_signal_connect(results, "notify::model", G_CALLBACK(CloseDuringRender), &workspace);
                UmiStatus refreshed = UmiBrokerWorkspaceGtkRefresh(workspace);
                if (strcmp(argv[1], "search-reentrant") == 0) {
                    CHECK(refreshed == UMI_STATUS_CANCELLED && workspace == NULL);
                    g_signal_handlers_disconnect_by_data(results, &workspace);
                } else {
                    CHECK(refreshed == UMI_STATUS_OK);
                    CHECK(Contains(searchStatus, "Complete"));
                    CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(results)) == GTK_INVALID_LIST_POSITION);
                    CHECK(!gtk_widget_get_sensitive(apply));
                    if (strcmp(argv[1], "search-empty") == 0) {
                        CHECK(Contains(searchStatus, "0 candidates"));
                    } else {
                        gtk_drop_down_set_selected(GTK_DROP_DOWN(results), 1U);
                        CHECK(gtk_widget_get_sensitive(apply));
                        if (strcmp(argv[1], "search-select") == 0) {
                            g_signal_emit_by_name(apply, "clicked");
                            CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(identity)), "456") == 0);
                            CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(exchange)), "") == 0);
                            CHECK(Contains(notice, "broker-supported exchange"));
                        } else if (strcmp(argv[1], "search-stale") == 0) {
                            search.stale = search.failed = true;
                            g_signal_emit_by_name(apply, "clicked");
                            CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(identity)), "999") == 0);
                            CHECK(!gtk_widget_get_sensitive(apply) && Contains(searchStatus, "STALE"));
                        } else if (strcmp(argv[1], "search-replaced") == 0) {
                            ++search.requestId; search.rows[1].contractId = 789U;
                            g_signal_emit_by_name(apply, "clicked");
                            CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(identity)), "999") == 0);
                            CHECK(Contains(notice, "no longer current"));
                            CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(results)) == GTK_INVALID_LIST_POSITION);
                        } else if (strcmp(argv[1], "search-active-quote") == 0) {
                            quoteAvailable = TRUE; quote.subscribed = true; quote.requestId = 36000U;
                            CHECK(UmiBrokerWorkspaceGtkRefresh(workspace) == UMI_STATUS_OK);
                            CHECK(!gtk_widget_get_sensitive(apply));
                            g_signal_emit_by_name(apply, "clicked");
                            CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(identity)), "999") == 0);
                        } else if (strcmp(argv[1], "search-reconnect") == 0) {
                            gtk_window_destroy(inspector);
                            CHECK(Contains(searchStatus, "STALE") && !gtk_widget_get_sensitive(apply));
                            searchAvailable = FALSE;
                            CHECK(UmiBrokerWorkspaceGtkOpenConnection(workspace) == UMI_STATUS_OK);
                            CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(results)) == GTK_INVALID_LIST_POSITION);
                            CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(results))) == 0U);
                            CHECK(!gtk_widget_get_sensitive(apply));
                        } else if (strcmp(argv[1], "search-utf8") == 0) {
                            GtkStringObject *item = GTK_STRING_OBJECT(gtk_drop_down_get_selected_item(GTK_DROP_DOWN(results)));
                            CHECK(item != NULL && strstr(gtk_string_object_get_string(item), "Example \xef\xbf\xbd") != NULL);
                            CHECK((unsigned char)search.rows[1].description[8] == 0xffU);
                        } else if (strcmp(argv[1], "search-rejected") == 0) {
                            searchResult = UMI_STATUS_CAPACITY_EXCEEDED;
                            g_signal_emit_by_name(start, "clicked");
                            CHECK(searches == 1U && search.requestId == 37001U);
                            CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(results)) == 1U);
                            CHECK(Contains(notice, umi_status_text(searchResult)));
                        } else return 2;
                    }
                }
            }
        }
        CHECK(subscriptions == 0U && cancellations == 0U);
    } else if (strncmp(argv[1], "quote-", 6U) == 0) {
        GtkWidget *start = Find(root, "broker-workspace-subscribe");
        GtkWidget *stop = Find(root, "broker-workspace-stop-quotes");
        GtkWidget *identity = Find(root, "broker-workspace-contract");
        GtkWidget *exchange = Find(root, "broker-workspace-exchange");
        GtkWidget *quoteStatus = Find(root, "broker-workspace-quote-status");
        GtkWidget *notice = Find(root, "broker-workspace-quote-notice");
        GtkWidget *dataType = Find(root, "broker-workspace-data-type");
        CHECK(GTK_IS_DROP_DOWN(dataType));
        CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(dataType)) == 0U);
        if (strcmp(argv[1], "quote-mode-frozen") == 0)
            gtk_drop_down_set_selected(GTK_DROP_DOWN(dataType), 1U);
        else if (strcmp(argv[1], "quote-mode-delayed") == 0 || strcmp(argv[1], "quote-mode-change") == 0)
            gtk_drop_down_set_selected(GTK_DROP_DOWN(dataType), 2U);
        else if (strcmp(argv[1], "quote-mode-delayed-frozen") == 0)
            gtk_drop_down_set_selected(GTK_DROP_DOWN(dataType), 3U);
        CHECK(start != NULL && stop != NULL && gtk_widget_get_sensitive(start));
        CHECK(!gtk_widget_get_sensitive(stop) && Contains(root, "Not received"));
        gtk_editable_set_text(GTK_EDITABLE(identity), "265598");
        gtk_editable_set_text(GTK_EDITABLE(exchange), "SMART");
        if (strcmp(argv[1], "quote-invalid") == 0) {
            gtk_editable_set_text(GTK_EDITABLE(identity), "bad-id");
            g_signal_emit_by_name(start, "clicked");
            CHECK(subscriptions == 0U && Contains(notice, "positive contract ID"));
        } else if (strcmp(argv[1], "quote-mode-invalid") == 0) {
            gtk_drop_down_set_selected(GTK_DROP_DOWN(dataType), GTK_INVALID_LIST_POSITION);
            g_signal_emit_by_name(start, "clicked");
            CHECK(subscriptions == 0U && !quoteAvailable && gtk_widget_get_sensitive(start));
            CHECK(Contains(notice, "data mode"));
        } else {
            guint selectedMode = gtk_drop_down_get_selected(GTK_DROP_DOWN(dataType));
            g_signal_emit_by_name(start, "clicked");
            CHECK(subscriptions == 1U && quote.contract.contractId == 265598U);
            CHECK(strcmp(quote.contract.exchange, "SMART") == 0);
            CHECK(!gtk_widget_get_sensitive(start) && gtk_widget_get_sensitive(stop));
            CHECK(!gtk_widget_get_sensitive(identity));
            CHECK(!gtk_widget_get_sensitive(dataType));
            CHECK(quote.requestedDataType == (UmiIbkrMarketDataType)(selectedMode + 1U));
            g_signal_emit_by_name(start, "clicked");
            CHECK(subscriptions == 1U);
            quote.dataType = UMI_IBKR_DATA_DELAYED;
            quote.bid.received = true;
            quote.bid.dataType = UMI_IBKR_DATA_DELAYED;
            (void)snprintf(quote.bid.text, sizeof(quote.bid.text), "9007199254740993.123456789");
            quote.ask.received = true; quote.ask.unavailable = true; quote.ask.stale = true;
            quote.ask.dataType = UMI_IBKR_DATA_DELAYED;
            CHECK(UmiBrokerWorkspaceGtkRefresh(workspace) == UMI_STATUS_OK);
            CHECK(Contains(Find(root, "broker-workspace-quote-bid"), "9007199254740993.123456789"));
            CHECK(Contains(Find(root, "broker-workspace-quote-ask"), "Unavailable"));
            CHECK(Contains(Find(root, "broker-workspace-quote-last"), "Not received"));
            CHECK(Contains(quoteStatus, UmiIbkrMarketDataTypeName(UMI_IBKR_DATA_DELAYED)));
            if (strcmp(argv[1], "quote-selection") == 0) {
                g_signal_emit_by_name(stop, "clicked");
                CHECK(cancellations == 1U && !quote.subscribed);
                CHECK(gtk_widget_get_sensitive(start) && !gtk_widget_get_sensitive(stop));
                CHECK(Contains(quoteStatus, "STALE") && Contains(root, "9007199254740993.123456789"));
            } else if (strcmp(argv[1], "quote-age") == 0) {
                /* No account observation changes: quote ageing must still render. */
                quote.bid.stale = true;
                CHECK(UmiBrokerWorkspaceGtkRefresh(workspace) == UMI_STATUS_OK);
                GtkWidget *bid = Find(root, "broker-workspace-quote-bid");
                GtkWidget *state = gtk_widget_get_next_sibling(bid);
                CHECK(GTK_IS_LABEL(state) && Contains(state, "STALE"));
            } else if (strcmp(argv[1], "quote-cancel-failure") == 0) {
                cancelResult = UMI_STATUS_CAPACITY_EXCEEDED;
                g_signal_emit_by_name(stop, "clicked");
                CHECK(cancellations == 1U && quote.subscribed);
                CHECK(gtk_widget_get_sensitive(stop) && !gtk_widget_get_sensitive(start));
                CHECK(Contains(notice, umi_status_text(cancelResult)));
            } else if (strcmp(argv[1], "quote-reconnect") == 0) {
                gtk_window_destroy(inspector);
                CHECK(Contains(quoteStatus, "STALE") && Contains(root, "9007199254740993.123456789"));
                quoteAvailable = FALSE;
                memset(&observed, 0, sizeof(observed)); observed.readOnly = true;
                CHECK(UmiBrokerWorkspaceGtkOpenConnection(workspace) == UMI_STATUS_OK);
                CHECK(!Contains(root, "9007199254740993.123456789"));
                CHECK(Contains(quoteStatus, "No quote requested") && !gtk_widget_get_sensitive(start));
            } else if (strcmp(argv[1], "quote-reentrant") == 0) {
                g_signal_connect(quoteStatus, "notify::label", G_CALLBACK(CloseDuringRender), &workspace);
                quote.dataType = UMI_IBKR_DATA_FROZEN;
                CHECK(UmiBrokerWorkspaceGtkRefresh(workspace) == UMI_STATUS_CANCELLED);
                CHECK(workspace == NULL);
                g_signal_handlers_disconnect_by_data(quoteStatus, &workspace);
            } else if (strcmp(argv[1], "quote-retained") == 0) {
                UmiBrokerWorkspaceGtkDestroy(workspace); workspace = NULL;
                g_signal_emit_by_name(start, "clicked"); g_signal_emit_by_name(stop, "clicked");
                CHECK(subscriptions == 1U && cancellations == 0U);
            } else if (strncmp(argv[1], "quote-mode-", 11U) == 0) {
                /* Requested and actual modes can differ with account entitlements. */
                char expected[96];
                (void)snprintf(expected, sizeof(expected), "Requested: %s", UmiIbkrMarketDataTypeName(quote.requestedDataType));
                CHECK(Contains(quoteStatus, expected));
                (void)snprintf(expected, sizeof(expected), "Received: %s", UmiIbkrMarketDataTypeName(quote.dataType));
                CHECK(Contains(quoteStatus, expected));
                if (strcmp(argv[1], "quote-mode-change") == 0) {
                    g_signal_emit_by_name(stop, "clicked");
                    CHECK(gtk_widget_get_sensitive(dataType));
                    gtk_drop_down_set_selected(GTK_DROP_DOWN(dataType), 1U);
                    g_signal_emit_by_name(start, "clicked");
                    CHECK(subscriptions == 2U && cancellations == 1U);
                    CHECK(quote.requestedDataType == UMI_IBKR_DATA_FROZEN);
                    CHECK(quote.dataType == UMI_IBKR_DATA_UNKNOWN && !quote.bid.received);
                    CHECK(Contains(Find(root, "broker-workspace-quote-bid"), "Not received"));
                }
            } else if (strcmp(argv[1], "quote-values") != 0) return 2;
        }
    } else if (strcmp(argv[1], "selection") == 0) {
        gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(root, "broker-workspace-accounts")), 1U);
        g_signal_emit_by_name(read, "clicked");
        CHECK(reads == 1U && strcmp(requested, "TEST-B") == 0);
        CHECK(!gtk_widget_get_sensitive(read));
        CHECK(Contains(root, "Account: TEST-B"));
    } else if (strcmp(argv[1], "utf8") == 0) {
        memcpy(observed.message, "Provider \xff text", sizeof("Provider \xff text"));
        memcpy(observed.accounts[0], "TEST-\xff", sizeof("TEST-\xff"));
        observed.valueCount = 1U;
        memcpy(observed.values[0].tag, "Value\xff", sizeof("Value\xff"));
        UmiIbkrConnectionSnapshot before = observed;
        CHECK(UmiBrokerWorkspaceGtkRefresh(workspace) == UMI_STATUS_OK);
        CHECK(Contains(status, "Provider \xef\xbf\xbd text"));
        CHECK(Contains(root, "Value\xef\xbf\xbd"));
        GtkDropDown *accounts = GTK_DROP_DOWN(Find(root, "broker-workspace-accounts"));
        GtkStringObject *selected = GTK_STRING_OBJECT(gtk_drop_down_get_selected_item(accounts));
        CHECK(selected != NULL && strcmp(gtk_string_object_get_string(selected), "TEST-\xef\xbf\xbd") == 0);
        CHECK(memcmp(&observed, &before, sizeof(observed)) == 0);
    } else if (strcmp(argv[1], "guidance") == 0) {
        observed.state = UMI_IBKR_FAILED;
        observed.providerCode = 326;
        CHECK(UmiBrokerWorkspaceGtkRefresh(workspace) == UMI_STATUS_OK);
        CHECK(Contains(Find(root, "broker-workspace-guidance"), "unused client ID"));
        CHECK(!gtk_widget_get_sensitive(read));
        observed.state = UMI_IBKR_READY; observed.providerCode = 0;
        CHECK(UmiBrokerWorkspaceGtkRefresh(workspace) == UMI_STATUS_OK);
        CHECK(Contains(Find(root, "broker-workspace-guidance"), "Select and read"));
        CHECK(gtk_widget_get_sensitive(read));
    } else if (strcmp(argv[1], "reentrant") == 0) {
        g_signal_connect(status, "notify::label", G_CALLBACK(CloseDuringRender), &workspace);
        observed.state = UMI_IBKR_FAILED;
        CHECK(UmiBrokerWorkspaceGtkRefresh(workspace) == UMI_STATUS_CANCELLED);
        CHECK(workspace == NULL);
        g_signal_handlers_disconnect_by_data(status, &workspace);
    } else {
        observed.requestIssued = true;
        observed.valueCount = 1U;
        observed.positionCount = 1U;
        (void)snprintf(observed.selectedAccount, 64U, "TEST-A");
        (void)snprintf(observed.values[0].tag, 64U, "NetLiquidation");
        (void)snprintf(observed.values[0].value, UMI_IBKR_TEXT_CAPACITY, "9007199254740993.123456789");
        (void)snprintf(observed.positions[0].localSymbol, 96U, "SYNTHETIC-FUTURE");
        (void)snprintf(observed.positions[0].quantity, 96U, "-0.000000001");
        CHECK(UmiBrokerWorkspaceGtkRefresh(workspace) == UMI_STATUS_OK);
        CHECK(Contains(root, "9007199254740993.123456789") && Contains(root, "-0.000000001"));
        CHECK(Contains(root, "Account values: not complete"));
        if (strcmp(argv[1], "completion") == 0) {
            observed.summaryComplete = true;
            CHECK(UmiBrokerWorkspaceGtkRefresh(workspace) == UMI_STATUS_OK);
            CHECK(Contains(root, "Account values: complete") && Contains(root, "Positions: not complete"));
            observed.positionsComplete = true;
            CHECK(UmiBrokerWorkspaceGtkRefresh(workspace) == UMI_STATUS_OK);
            CHECK(Contains(root, "Positions: complete"));
        } else if (strcmp(argv[1], "reconnect") == 0) {
            gtk_window_destroy(inspector);
            CHECK(Contains(root, "STALE") && Contains(root, "9007199254740993.123456789"));
            memset(&observed, 0, sizeof(observed));
            observed.readOnly = true;
            CHECK(UmiBrokerWorkspaceGtkOpenConnection(workspace) == UMI_STATUS_OK);
            CHECK(!Contains(root, "9007199254740993.123456789") && !Contains(root, "SYNTHETIC-FUTURE"));
            CHECK(!gtk_widget_get_sensitive(read));
        } else if (strcmp(argv[1], "decimal") != 0) return 2;
    }
    UmiBrokerWorkspaceGtkDestroy(workspace);
    gtk_window_destroy(parent);
    g_object_unref(root);
    return 0;
}
