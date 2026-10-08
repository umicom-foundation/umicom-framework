/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_connection.c
 * PURPOSE:
 *   Paper/Live connection controls. This window never offers order execution.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Paper/Live connection controls. This window never offers order execution.
 *---------------------------------------------------------------------------*/
#include "window_removal_private.h"
#include "umicom/broker_connectivity/connection_gtk4.h"
#include "umicom/broker_connectivity/connection.h"
#include "umicom/broker_connectivity/quotes.h"
#include "umicom/broker_connectivity/contract_details.h"
#include "umicom/broker_connectivity/market_rule.h"
#include "umicom/broker_connectivity/execution_observation.h"
#include "umicom/broker_connectivity/pnl.h"
#include "umicom/broker_connectivity/market_depth.h"
#include "umicom/broker_connectivity/symbol_search.h"
#include "umicom/broker_connectivity/order_recovery.h"
#include "umicom/broker_connectivity/completed_orders.h"
#include "umicom/broker_connectivity/completed_report.h"
#include "umicom/broker_connectivity/historical_chart.h"
#include "umicom/broker_connectivity/historical_report.h"
#include "umicom/broker_connectivity/realtime_chart.h"
#include "umicom/broker_connectivity/realtime_report.h"
#include "umicom/broker_connectivity/scanner.h"
#include "umicom/broker_connectivity/scanner_catalog.h"
#include "umicom/broker_connectivity/discovery_report.h"
#include "umicom/broker_connectivity/option_contract.h"
#include <stddef.h>
#ifdef UMI_IBKR_HAS_HISTORY_CHART
#include "umicom/chart/adapters/cairo_renderer.h"
#endif
#include "umicom/broker_connectivity/fill_policy.h"
#include "umicom/trading/fill_watch.h"
#include "umicom/broker_connectivity/observation_export.h"
#include "umicom/platform/output_file.h"
#ifdef UMI_IBKR_HAS_FILTERED_CHOICES
#include "umicom/ui/gtk4/filtered_choices.h"
#include "umicom/broker_connectivity/position_review.h"
#endif
#include "umicom/base/text.h"
#include <inttypes.h>
#include <limits.h>
#include <string.h>
#include "desktop_system_brand.inc"

#define UI_KEY "umicom-ibkr-connection-owner"
typedef struct HistoricalCanvas HistoricalCanvas;
typedef struct ConnectionUi {
    UmiIbkrConnection *connection;
    UmiIbkrConnectionSnapshot snapshot;
    GtkWidget *mode, *program, *port, *client, *ack;
    GtkWidget *connect, *disconnect, *read, *accounts, *status, *output;
    guint source;
    gboolean closed, populated;
    uint64_t lastPaint;
    gchar *lastOutput;
    GtkWidget *quoteContract, *quoteExchange, *quoteStart, *quoteStop, *quoteOutput, *quoteNotice;
    uint32_t quoteRequest;
    gboolean quoteSubscribed;
    GtkWidget *exportPath, *exportReport, *exportStatus;
    gboolean exportBusy;
    /* The connection window owns a local notification watch. It never owns an
     * order submission callback, and a new connection retires the old binding. */
    GtkWindow *window;
    GtkWidget *fillMode, *fillSide, *fillTif, *fillQuantity, *fillPrice, *fillBuffer;
    GtkWidget *fillAge, *fillSkew, *fillReview, *fillStart, *fillStop, *fillOutput;
    UmiFullQuantityWatch *fillWatch;
    UmiFullQuantityPolicy fillPolicy;
    UmiIbkrQuoteContract fillContract;
    uint32_t fillRequest;
    gboolean fillBusy, fillRetired;
    GtkWidget *contractRead, *contractAbandon, *contractOutput;
    uint32_t contractRequest;
    GtkWidget *ruleRead, *ruleOutput;
    uint32_t ruleId, ruleQuoteRequest, ruleContractRequest;
    UmiIbkrQuoteContract ruleContract;
    GtkWidget *executionRead,*executionAbandon,*executionOutput,*executionReviewOutput;
    GtkWidget *executionPermanent,*executionContract,*executionQuantity,*executionSide;
    uint32_t executionRequest;
    /* Each observation panel owns its request identity until stopped or replaced. */
    GtkWidget *pnlModel,*pnlContract,*pnlStart,*pnlStop,*pnlOutput;
    uint32_t pnlRequest;
    GtkWidget *symbolSearchPattern,*symbolSearchRead,*symbolSearchAbandon;
    GtkWidget *symbolSearchResults,*symbolSearchApply,*symbolSearchOutput;
    uint32_t symbolSearchRequest,symbolSearchDisplayed;
    GtkWidget *depthRows,*depthSmart,*depthStart,*depthStop,*depthOutput;
    uint32_t depthRequest;
    GtkWidget *ordersAll,*ordersRead,*ordersOutput;
    GtkWidget *completedApiOnly,*completedRead,*completedOutput;
    GtkWidget *completedExportPath,*completedExportReport,*completedExportStatus;
    gboolean completedExportBusy;
    GtkWidget *historySize,*historyKind,*historyDuration,*historyEnd,*historyRth;
    GtkWidget *historyRead,*historyCancel,*historyOutput,*historyDrawing;
    GtkWidget *historyVisible,*historyOffset;
    uint32_t historyRequest;
    GtkWidget *historyExportPath,*historyExportReport,*historyExportStatus;
    gboolean historyExportBusy;
    HistoricalCanvas *historyCanvas; /* Owned by the drawing widget. */
    /* The native panel presents one explicit subscription; Framework consumers
     * can use additional independent slots without adding another transport. */
    GtkWidget *streamKind,*streamRth,*streamStart,*streamStop,*streamOutput;
    GtkWidget *streamDrawing,*streamVisible,*streamOffset;
    GtkWidget *streamExportPath,*streamExportReport,*streamExportStatus;
    gboolean streamExportBusy;
    GtkWidget *scannerEntries[19], *scannerRows, *scannerFilters, *scannerExclude;
    GtkWidget *scannerResults, *scannerOutput;
    GtkWidget *catalogText, *catalogSearch, *catalogOutput;
    gboolean catalogLoaded, catalogReset;
    uint32_t scannerRequest, scannerDisplayedRequest;
    uint64_t scannerDisplayedGeneration;
    GtkWidget *optionSymbol, *optionType, *optionExchange, *optionResults, *optionExpiry, *optionStrike;
    GtkWidget *optionRight, *optionCurrency, *optionOutput;
    uint32_t optionRequest, optionDisplayedRequest, optionContractRequest;
    GtkWidget *discoveryExportPath[2], *discoveryExportReport[2], *discoveryExportStatus[2];
    gboolean discoveryExportBusy[2];
    uint32_t streamRequest;
    HistoricalCanvas *streamCanvas; /* Uses the same owned-scene drawing callback. */
#ifdef UMI_IBKR_HAS_FILTERED_CHOICES
    GtkWidget *positionFilter, *positionPicker, *positionDetail;
    UmiIbkrPositionReview *positionReview;
    bool positionBusy;
#endif
} ConnectionUi;

static ConnectionUi *Owner(GtkWindow *window)
{
    ConnectionUi *ui = g_object_get_data(G_OBJECT(window), UI_KEY);
    return ui != NULL && !ui->closed ? ui : NULL;
}
static GtkWidget *Text(const char *value)
{
    GtkWidget *label = gtk_label_new(value);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_widget_set_hexpand(label, TRUE);
    return label;
}
/* The report worker never borrows a live broker or UI state. */
#include "ibkr_observation_export.inc"
#ifdef UMI_IBKR_HAS_FILTERED_CHOICES
#include "ibkr_position_review.inc"
#endif

static void FillWatchRetire(ConnectionUi *ui)
{
    if (ui->fillWatch != NULL) ui->fillRetired = TRUE;
    UmiFullQuantityWatchDestroy(ui->fillWatch);
    ui->fillWatch = NULL;
    ui->fillRequest = 0U;
}
static void FillWatchPaint(ConnectionUi *ui, uint64_t now);
static void ContractDetailsPaint(ConnectionUi *ui, uint64_t now);
static void MarketRulePaint(ConnectionUi *ui, uint64_t now);
static void ExecutionsPaint(ConnectionUi *ui, uint64_t now);
static void PnlPaint(ConnectionUi *ui, uint64_t now);
static void SymbolSearchPaint(ConnectionUi *ui, uint64_t now);
static void DepthPaint(ConnectionUi *ui, uint64_t now);
static void OrdersPaint(ConnectionUi *ui, uint64_t now);
static void CompletedPaint(ConnectionUi *ui, uint64_t now);
static void HistoricalPaint(ConnectionUi *ui, uint64_t now);
static void HistoricalRetire(ConnectionUi *ui);
static void StreamingPaint(ConnectionUi *ui, uint64_t now);
static void ScannerPaint(ConnectionUi *ui, uint64_t now);
static void CatalogPaint(ConnectionUi *ui, uint64_t now);
static void OptionChainPaint(ConnectionUi *ui, uint64_t now);
static void StreamingRetire(ConnectionUi *ui);

static GtkWidget *Brand(const unsigned char *bytes, size_t length, int height)
{
    GBytes *data = g_bytes_new_static(bytes, length);
    GError *error = NULL;
    GdkTexture *texture = gdk_texture_new_from_bytes(data, &error);
    g_bytes_unref(data);
    if (texture == NULL) { g_clear_error(&error); return Text("Umicom"); }
    GtkWidget *image = gtk_image_new_from_paintable(GDK_PAINTABLE(texture));
    gtk_image_set_pixel_size(GTK_IMAGE(image), height);
    g_object_unref(texture);
    return image;
}
static gboolean Connected(const ConnectionUi *ui)
{
    return ui->connection != NULL && ui->snapshot.state >= UMI_IBKR_CONNECTING &&
        ui->snapshot.state <= UMI_IBKR_READY;
}
static void Controls(ConnectionUi *ui)
{
    gboolean active = Connected(ui);
    GtkWidget *profile[] = {ui->mode, ui->program, ui->port, ui->client};
    /* A sensitivity observer may close the window. The former loop is kept
     * for review; the replacement stops before accessing another child. */
#if 0
    for (size_t i = 0; i < G_N_ELEMENTS(profile); ++i)
        gtk_widget_set_sensitive(profile[i], !active);
#endif
    for (size_t i = 0; i < G_N_ELEMENTS(profile); ++i)
    {
        if (ui->closed) return;
        gtk_widget_set_sensitive(profile[i], !active);
    }
    if (ui->closed) return;
    gboolean live = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->mode)) == 1U;
    gtk_widget_set_sensitive(ui->ack, !active && live);
    if (ui->closed) return;
    gtk_widget_set_sensitive(ui->connect, !active);
    if (ui->closed) return;
    gtk_widget_set_sensitive(ui->disconnect, active);
    if (ui->closed) return;
    gboolean canRead = active && ui->snapshot.state == UMI_IBKR_READY &&
        !ui->snapshot.requestIssued && ui->snapshot.accountCount != 0U;
    gtk_widget_set_sensitive(ui->read, canRead);
    if (ui->closed) return;
    gtk_widget_set_sensitive(ui->accounts, canRead);
    if (ui->closed) return;
    /* Quote inspection shares the existing connection but has its own request
     * lifetime. A failed quote remains cancellable without closing the account. */
    gboolean ready = active && ui->snapshot.state == UMI_IBKR_READY;
    gtk_widget_set_sensitive(ui->quoteStart, ready && !ui->quoteSubscribed);
    if (ui->closed) return;
    gtk_widget_set_sensitive(ui->quoteStop, ready && ui->quoteSubscribed);
    if (ui->closed) return;
    gtk_widget_set_sensitive(ui->quoteContract, !ui->quoteSubscribed);
    if (ui->closed) return;
    gtk_widget_set_sensitive(ui->quoteExchange, !ui->quoteSubscribed);
    if (ui->closed) return;
}
/* Show each field's own receipt age. Activity on another side of the market
 * does not refresh this field, and the data type is always printed beside it. */
static void QuoteValueText(GString *text, const char *name, const UmiIbkrQuoteValue *value, uint64_t now)
{
    if (!value->received) { g_string_append_printf(text, "%s: waiting\n", name); return; }
    g_string_append_printf(text, "%s: %s | %s | %s | age %" PRIu64 " ms\n", name,
        value->unavailable ? "unavailable" : value->text,
        UmiIbkrMarketDataTypeName(value->dataType), value->stale ? "STALE" : "recently received",
        now >= value->receivedAtMilliseconds ? now - value->receivedAtMilliseconds : 0U);
}
static void RenderQuote(ConnectionUi *ui, uint64_t now)
{
    if (ui->connection == NULL || ui->quoteRequest == 0U) return;
    UmiIbkrQuoteSnapshot quote;
    UmiStatus status = UmiIbkrQuoteCopy(ui->connection, ui->quoteRequest, now, 15000U, &quote);
    if (status != UMI_STATUS_OK) return;
    ui->quoteSubscribed = quote.subscribed;
    GString *text = g_string_new(NULL);
    g_string_append_printf(text, "Contract %" PRIu32 " on %s | %s\n%s\n",
        quote.contract.contractId, quote.contract.exchange,
        UmiIbkrMarketDataTypeName(quote.dataType), quote.message);
    QuoteValueText(text, "Bid", &quote.bid, now);
    QuoteValueText(text, "Bid size", &quote.bidSize, now);
    QuoteValueText(text, "Ask", &quote.ask, now);
    QuoteValueText(text, "Ask size", &quote.askSize, now);
    QuoteValueText(text, "Last", &quote.last, now);
    QuoteValueText(text, "Last size", &quote.lastSize, now);
    gtk_label_set_text(GTK_LABEL(ui->quoteOutput), text->str);
    g_string_free(text, TRUE);
}

static void Render(ConnectionUi *ui, uint64_t now)
{
    if (ui->connection == NULL) { Controls(ui); return; }
    (void)UmiIbkrConnectionCopy(ui->connection, &ui->snapshot);
    if (!ui->populated && ui->snapshot.state == UMI_IBKR_READY) {
        GtkStringList *items = gtk_string_list_new(NULL);
        for (size_t i = 0; i < ui->snapshot.accountCount; ++i)
            gtk_string_list_append(items, ui->snapshot.accounts[i]);
        gtk_drop_down_set_model(GTK_DROP_DOWN(ui->accounts), G_LIST_MODEL(items));
        g_object_unref(items);
        ui->populated = TRUE;
    }
    if (ui->closed) return;
    RenderQuote(ui, now);
    if (ui->closed) return;
    GString *text = g_string_new(NULL);
    const UmiIbkrConnectionSnapshot *s = &ui->snapshot;
    g_string_append_printf(text, "Requested mode: %s\nServer mode attested: NO\nOrders: not available\nProtocol version: %d\n\n",
        s->requestedEnvironment == UMI_TRADING_LIVE ? "LIVE" : "PAPER", s->protocolVersion);
    g_string_append_printf(text, "Authorised accounts: %zu\n", s->accountCount);
    for (size_t i = 0; i < s->accountCount; ++i)
        g_string_append_printf(text, "  %s\n", s->accounts[i]);
    if (s->requestIssued) {
        g_string_append_printf(text, "\nSelected account: %s\nSummary: %s; Positions: %s\n",
            s->selectedAccount, s->summaryComplete ? "complete" : "INCOMPLETE",
            s->positionsComplete ? "complete" : "INCOMPLETE");
        if (s->summaryComplete && now >= s->summaryAtMilliseconds)
            g_string_append_printf(text, "Summary received at monotonic time %" PRIu64 " ms.\n", s->summaryAtMilliseconds);
        if (s->positionsComplete && now >= s->positionsAtMilliseconds)
            g_string_append_printf(text, "Positions received at monotonic time %" PRIu64 " ms.\n", s->positionsAtMilliseconds);
        g_string_append(text, "These are separate captured responses, not an atomic or continuously refreshed portfolio.\n\n");
        for (size_t i = 0; i < s->valueCount; ++i) {
            const UmiIbkrAccountValue *v = &s->values[i];
            g_string_append_printf(text, "%s: %s %s\n", v->tag, v->value[0] ? v->value : "(unset)", v->currency);
        }
        g_string_append_printf(text, "\nPositions received: %zu\n", s->positionCount);
        for (size_t i = 0; i < s->positionCount; ++i) {
            const UmiIbkrPositionObservation *p = &s->positions[i];
            g_string_append_printf(text, "%s | %s | %s | %s | %s\n  quantity %s; average cost %s %s\n  expiry %s; strike %s; right %s; multiplier %s\n",
                p->contractId, p->symbol, p->securityType, p->localSymbol, p->exchange,
                p->quantity, p->averageCost, p->currency, p->expiry, p->strike, p->right, p->multiplier);
        }
    }
    if (s->stale)
        g_string_append(text, "\nSTALE: the connection is not ready or has ended. Retained rows are not current.\n");
    /* Preserve selection/copying when only the age label changes. */
    if (ui->lastOutput == NULL || strcmp(ui->lastOutput, text->str) != 0) {
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ui->output)), text->str, -1);
        g_free(ui->lastOutput);
        ui->lastOutput = g_string_free(text, FALSE);
    } else {
        g_string_free(text, TRUE);
    }
    if (ui->closed) return;
    GString *status = g_string_new(UmiIbkrConnectionStateName(s->state));
    g_string_append_printf(status, "\n%s", s->message);
    if (s->summaryComplete && now >= s->summaryAtMilliseconds)
        g_string_append_printf(status, "\nSummary age: %" PRIu64 " ms.", now-s->summaryAtMilliseconds);
    if (s->positionsComplete && now >= s->positionsAtMilliseconds)
        g_string_append_printf(status, " Positions age: %" PRIu64 " ms.", now-s->positionsAtMilliseconds);
    gtk_label_set_text(GTK_LABEL(ui->status), status->str);
    g_string_free(status, TRUE);
    if (ui->closed) return;
    Controls(ui);
    ContractDetailsPaint(ui, now);
    if (ui->closed) return;
    MarketRulePaint(ui, now);
    if (ui->closed) return;
    ExecutionsPaint(ui, now);
    if (ui->closed) return;
    PnlPaint(ui, now);
    if (ui->closed) return;
    SymbolSearchPaint(ui, now);
    if (ui->closed) return;
    DepthPaint(ui, now);
    if (ui->closed) return;
    OrdersPaint(ui, now);
    if (ui->closed) return;
    CompletedPaint(ui, now);
    if (ui->closed) return;
    HistoricalPaint(ui, now);
    if (ui->closed) return;
    StreamingPaint(ui, now);
    if (ui->closed) return;
    CatalogPaint(ui, now);
    if (ui->closed) return;
    ScannerPaint(ui, now);
    if (ui->closed) return;
    OptionChainPaint(ui, now);
    if (ui->closed) return;
    FillWatchPaint(ui, now);
}
/* Native label callbacks can remove a retained window during polling. The
 * replacement below checks logical lifetime after publication; the former
 * timer callback is retained for engineering review. */
#if 0
static gboolean Poll(gpointer data)
{
    ConnectionUi *ui = Owner(GTK_WINDOW(data));
    if (ui == NULL) return G_SOURCE_REMOVE;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiStatus status = UmiIbkrConnectionPump(ui->connection, now);
    (void)UmiIbkrConnectionCopy(ui->connection, &ui->snapshot);
    if (status != UMI_STATUS_OK && Connected(ui))
        UmiIbkrConnectionClose(ui->connection);
    if (status != UMI_STATUS_OK || now-ui->lastPaint >= 250U) {
        Render(ui, now);
        ui->lastPaint = now;
    }
    if (status != UMI_STATUS_OK || !Connected(ui)) {
        ui->source = 0U;
        return G_SOURCE_REMOVE;
    }
    return G_SOURCE_CONTINUE;
}
#endif
/* Poll owns a temporary window reference while GTK publishes labels. A
 * notification may close the native window; reacquire the logical owner before
 * scheduling or touching state again. This also protects future child panels. */
static gboolean Poll(gpointer data)
{
    GtkWindow *window = GTK_WINDOW(data);
    g_object_ref(window);
    ConnectionUi *ui = Owner(window);
    if (ui == NULL) { g_object_unref(window); return G_SOURCE_REMOVE; }
    const uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiStatus status = UmiIbkrConnectionPump(ui->connection, now);
    (void)UmiIbkrConnectionCopy(ui->connection, &ui->snapshot);
    if (status != UMI_STATUS_OK && Connected(ui)) UmiIbkrConnectionClose(ui->connection);
    if (status != UMI_STATUS_OK || now-ui->lastPaint >= 250U)
    {
        Render(ui, now);
        ui = Owner(window);
        if (ui == NULL) { g_object_unref(window); return G_SOURCE_REMOVE; }
        ui->lastPaint = now;
    }
    const gboolean keep = status == UMI_STATUS_OK && Connected(ui);
    if (!keep) ui->source = 0U;
    g_object_unref(window);
    return keep ? G_SOURCE_CONTINUE : G_SOURCE_REMOVE;
}
static void ProfileChanged(GObject *object, GParamSpec *spec, gpointer data)
{
    (void)object; (void)spec;
    ConnectionUi *ui = Owner(GTK_WINDOW(data));
    if (ui == NULL || Connected(ui)) return;
    UmiTradingEnvironment mode = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->mode)) == 1U ? UMI_TRADING_LIVE : UMI_TRADING_PAPER;
    UmiIbkrProgram program = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->program)) == 1U ? UMI_IBKR_GATEWAY : UMI_IBKR_TWS;
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(ui->port), UmiIbkrDefaultPort(program, mode));
    gtk_check_button_set_active(GTK_CHECK_BUTTON(ui->ack), FALSE);
    Controls(ui);
}
static void ConnectClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = Owner(GTK_WINDOW(data));
    if (ui == NULL || Connected(ui)) return;
#ifdef UMI_IBKR_HAS_FILTERED_CHOICES
    if (PositionReviewBusy(ui)) return;
    /* A catalogue identity never crosses into a newly created connection. */
    PositionReviewRetire(ui);
#endif
    FillWatchRetire(ui);
    UmiIbkrConnectionOptions options = UmiIbkrConnectionOptionsDefault();
    options.environment = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->mode)) == 1U ? UMI_TRADING_LIVE : UMI_TRADING_PAPER;
    options.program = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->program)) == 1U ? UMI_IBKR_GATEWAY : UMI_IBKR_TWS;
    options.adapter.paperOnly = options.environment == UMI_TRADING_PAPER ? 1 : 0;
    options.adapter.port = (uint16_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ui->port));
    options.adapter.clientId = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ui->client));
    options.acknowledgeLive = gtk_check_button_get_active(GTK_CHECK_BUTTON(ui->ack)) != FALSE;
    UmiStatus status = UmiIbkrConnectionValidate(&options);
    if (status != UMI_STATUS_OK) {
        gtk_label_set_text(GTK_LABEL(ui->status), "Connection not opened. Check the profile and acknowledge Live access when selected.");
        return;
    }
    UmiIbkrConnectionDestroy(ui->connection);
    ui->connection = NULL; ui->populated = FALSE;
    HistoricalRetire(ui);
    StreamingRetire(ui);
    ui->catalogLoaded=FALSE; ui->catalogReset=TRUE;
    ui->scannerRequest=0U; ui->scannerDisplayedRequest=0U; ui->scannerDisplayedGeneration=0U;
    ui->optionRequest=0U; ui->optionDisplayedRequest=0U; ui->optionContractRequest=0U;
    /* Reconnecting creates a new request namespace. Discard the old inspector
     * selection before the new connection can allocate an identical number. */
    ui->quoteRequest = 0U; ui->quoteSubscribed = FALSE;
    ui->contractRequest = 0U;
    ui->executionRequest = 0U;
    ui->pnlRequest = 0U;
    ui->symbolSearchRequest = 0U;
    ui->depthRequest = 0U;
    ui->symbolSearchDisplayed = 0U;
    ui->ruleId = 0U;
    ui->ruleQuoteRequest = 0U;
    ui->ruleContractRequest = 0U;
    gtk_label_set_text(GTK_LABEL(ui->quoteOutput), "No quote requested on this connection.");
    gtk_label_set_text(GTK_LABEL(ui->quoteNotice), "");
    memset(&ui->snapshot, 0, sizeof ui->snapshot);
    gtk_drop_down_set_model(GTK_DROP_DOWN(ui->accounts), NULL);
    status = UmiIbkrConnectionCreate(&options, &ui->connection);
    if (status != UMI_STATUS_OK) {
        gtk_label_set_text(GTK_LABEL(ui->status), umi_status_text(status));
        Controls(ui); return;
    }
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    status = UmiIbkrConnectionOpen(ui->connection, now);
    Render(ui, now);
    if (ui->closed) return;
    if (status == UMI_STATUS_OK) {
        ui->lastPaint = now;
        ui->source = g_timeout_add(25U, Poll, data);
        if (ui->source == 0U) { UmiIbkrConnectionClose(ui->connection); Render(ui, now); }
    }
}
/* GTK collects a contract identity; Framework owns validation, framing and
 * subscription lifetime. A click cannot place an order or buy a snapshot. */
static void QuoteStartClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = Owner(GTK_WINDOW(data));
    if (ui == NULL || ui->connection == NULL || ui->quoteSubscribed) return;
#ifdef UMI_IBKR_HAS_FILTERED_CHOICES
    if (PositionReviewBusy(ui)) return;
#endif
    const char *identity = gtk_editable_get_text(GTK_EDITABLE(ui->quoteContract));
    const char *exchange = gtk_editable_get_text(GTK_EDITABLE(ui->quoteExchange));
    uint64_t number = 0U;
    gboolean valid = identity[0] != '\0';
    for (const char *cursor = identity; valid && *cursor != '\0'; ++cursor) {
        if (*cursor < '0' || *cursor > '9' || number > (uint64_t)INT_MAX / 10U) valid = FALSE;
        else { number = number * 10U + (uint64_t)(*cursor - '0'); if (number > INT_MAX) valid = FALSE; }
    }
    UmiIbkrQuoteContract contract = {0};
    if (!valid || number == 0U || strlen(exchange) >= sizeof(contract.exchange)) {
        gtk_label_set_text(GTK_LABEL(ui->quoteNotice), "Enter the positive contract ID and exchange from TWS Contract Description.");
        return;
    }
    contract.contractId = (uint32_t)number;
    strcpy(contract.exchange, exchange);
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiStatus status = UmiIbkrQuoteSubscribe(ui->connection, &contract, now, &ui->quoteRequest);
    gtk_label_set_text(GTK_LABEL(ui->quoteNotice), status == UMI_STATUS_OK
        ? "Subscribed data requested. The provider determines permissions and availability."
        : umi_status_text(status));
    Render(ui, now);
}
static void QuoteStopClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = Owner(GTK_WINDOW(data));
    if (ui == NULL || ui->connection == NULL || ui->quoteRequest == 0U) return;
    FillWatchRetire(ui);
    UmiStatus status = UmiIbkrQuoteCancel(ui->connection, ui->quoteRequest);
    gtk_label_set_text(GTK_LABEL(ui->quoteNotice), status == UMI_STATUS_OK
        ? "Quote stream cancelled. Choose another contract or disconnect."
        : umi_status_text(status));
    Render(ui, UmiIbkrMonotonicMilliseconds());
}

static void DisconnectClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = Owner(GTK_WINDOW(data));
    if (ui == NULL) return;
    if (ui->source != 0U) { g_source_remove(ui->source); ui->source = 0U; }
    UmiIbkrConnectionClose(ui->connection);
    Render(ui, UmiIbkrMonotonicMilliseconds());
}
static void ReadClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = Owner(GTK_WINDOW(data));
    if (ui == NULL || !Connected(ui)) return;
    guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->accounts));
    if ((size_t)index >= ui->snapshot.accountCount) return;
    UmiStatus status = UmiIbkrConnectionReadAccount(ui->connection, ui->snapshot.accounts[index], UmiIbkrMonotonicMilliseconds());
    Render(ui, UmiIbkrMonotonicMilliseconds());
    if (status != UMI_STATUS_OK) gtk_label_set_text(GTK_LABEL(ui->status), umi_status_text(status));
}
static void Destroyed(GtkWidget *widget, gpointer data)
{
    (void)data;
    ConnectionUi *ui = g_object_get_data(G_OBJECT(widget), UI_KEY);
    if (ui == NULL || ui->closed) return;
    ui->closed = TRUE;
    FillWatchRetire(ui);
#ifdef UMI_IBKR_HAS_FILTERED_CHOICES
    PositionReviewRetire(ui);
#endif
    if (ui->source != 0U) { g_source_remove(ui->source); ui->source = 0U; }
    UmiIbkrConnectionDestroy(ui->connection); ui->connection = NULL;
    /* Native removal can already have released the child widgets. Clear only
     * connection-owned identity here; never repaint controls during teardown. */
    ui->quoteRequest = 0U; ui->quoteSubscribed = FALSE;
}
static void FreeOwner(gpointer data)
{
    ConnectionUi *ui = data;
    if (ui->source != 0U) g_source_remove(ui->source);
    UmiIbkrConnectionDestroy(ui->connection);
    g_free(ui->lastOutput);
    FillWatchRetire(ui);
#ifdef UMI_IBKR_HAS_FILTERED_CHOICES
    PositionReviewRetire(ui);
#endif
    g_free(ui);
}
static void Field(GtkGrid *grid, const char *name, GtkWidget *widget, int row, const char *id)
{
    gtk_grid_attach(grid, Text(name), 0, row, 1, 1);
    gtk_grid_attach(grid, widget, 1, row, 1, 1);
    gtk_widget_set_hexpand(widget, TRUE);
    gtk_widget_set_name(widget, id);
}
/* The native adapter only collects fields and paints shared policy results. */
#include "ibkr_fill_policy.inc"
#include "ibkr_contract_details.inc"
#include "ibkr_market_rule.inc"
#include "ibkr_executions.inc"
#include "ibkr_pnl.inc"
#include "ibkr_symbol_search.inc"
#include "ibkr_depth.inc"
#include "ibkr_orders.inc"
#include "ibkr_completed_export.inc"
#include "ibkr_completed.inc"
#include "ibkr_historical_export.inc"
#include "ibkr_historical.inc"
#include "ibkr_streaming_export.inc"
#include "ibkr_streaming.inc"
#include "ibkr_discovery_export.inc"
#include "ibkr_scanner_catalog.inc"
#include "ibkr_scanner.inc"
#include "ibkr_option_chain.inc"
GtkWindow *UmiIbkrGtkCreate(GtkWindow *parent)
{
    GtkWindow *window = GTK_WINDOW(gtk_window_new());
    ConnectionUi *ui = g_new0(ConnectionUi, 1);
    ui->window = window;
    g_object_set_data_full(G_OBJECT(window), UI_KEY, ui, FreeOwner);
/* Close the connection at native window removal, including retained windows. The previous implementation remains for engineering review. */
#if 0
    g_signal_connect(window, "destroy", G_CALLBACK(Destroyed), NULL);
#endif
    UmiGtk4ObserveWindowRemoval(window, G_OBJECT(window), Destroyed, NULL);
    gtk_window_set_title(window, "Umicom Broker Connections — Read-only");
    gtk_window_set_default_size(window, 940, 740);
    gtk_window_set_icon_name(window, "org.umicom.trader");
    if (parent != NULL) {
        gtk_window_set_transient_for(window, parent);
        gtk_window_set_destroy_with_parent(window, TRUE);
        UmiGtk4ObserveWindowRemoval(parent, G_OBJECT(window), UmiGtk4CloseRemovedParentChild, window);
        GtkApplication *application = gtk_window_get_application(parent);
        if (application != NULL) gtk_window_set_application(window, application);
    }
    GtkWidget *scroll = gtk_scrolled_window_new();
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_start(root, 18); gtk_widget_set_margin_end(root, 18);
    gtk_widget_set_margin_top(root, 18); gtk_widget_set_margin_bottom(root, 18);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), root);
    gtk_window_set_child(window, scroll);
    GtkWidget *brand = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_box_append(GTK_BOX(brand), Brand(UMICOM_SYSTEM_ICON, sizeof UMICOM_SYSTEM_ICON, 40));
    gtk_box_append(GTK_BOX(brand), Brand(UMICOM_SYSTEM_LOGO, sizeof UMICOM_SYSTEM_LOGO, 48));
    gtk_box_append(GTK_BOX(root), brand);
    gtk_box_append(GTK_BOX(root), Text("Choose Paper or Live, then confirm the actual login in TWS / IB Gateway. This window cannot place, cancel or modify orders."));
    gtk_box_append(GTK_BOX(root), Text("Endpoint: 127.0.0.1 only. Keep Read-Only API enabled in TWS. A port number or account prefix does not verify the environment."));
    const char *modes[] = {"Paper", "Live", NULL};
    const char *programs[] = {"TWS", "IB Gateway", NULL};
    ui->mode = gtk_drop_down_new_from_strings(modes);
    ui->program = gtk_drop_down_new_from_strings(programs);
    ui->port = gtk_spin_button_new_with_range(1, 65535, 1);
    ui->client = gtk_spin_button_new_with_range(1, INT_MAX, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(ui->port), 7497);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(ui->client), 35);
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8); gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    Field(GTK_GRID(grid), "Requested environment", ui->mode, 0, "ibkr-mode");
    Field(GTK_GRID(grid), "Provider program", ui->program, 1, "ibkr-program");
    Field(GTK_GRID(grid), "Configured API port", ui->port, 2, "ibkr-port");
    Field(GTK_GRID(grid), "Unique client ID", ui->client, 3, "ibkr-client");
    gtk_box_append(GTK_BOX(root), grid);
    ui->ack = gtk_check_button_new_with_label("I intend to access my Live account read-only and have checked the provider login.");
    gtk_widget_set_name(ui->ack, "ibkr-live-ack");
    gtk_box_append(GTK_BOX(root), ui->ack);
    GtkWidget *buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    ui->connect = gtk_button_new_with_label("Connect read-only");
    ui->disconnect = gtk_button_new_with_label("Disconnect");
    gtk_widget_set_name(ui->connect, "ibkr-connect");
    gtk_widget_set_name(ui->disconnect, "ibkr-disconnect");
    gtk_box_append(GTK_BOX(buttons), ui->connect); gtk_box_append(GTK_BOX(buttons), ui->disconnect);
    gtk_box_append(GTK_BOX(root), buttons);
    ui->accounts = gtk_drop_down_new(NULL, NULL);
    ui->read = gtk_button_new_with_label("Read selected account once");
    gtk_widget_set_name(ui->accounts, "ibkr-accounts"); gtk_widget_set_name(ui->read, "ibkr-read");
    gtk_box_append(GTK_BOX(root), ui->accounts); gtk_box_append(GTK_BOX(root), ui->read);
    gtk_box_append(GTK_BOX(root), Text("To refresh or choose another account, disconnect and connect again. Only selected-account observations are retained; provider requests may cover all accounts authorised to this login."));
    gtk_box_append(GTK_BOX(root), Text("Subscribed market quote"));
    gtk_box_append(GTK_BOX(root), Text("In TWS, open Contract Description and copy the contract ID and exchange. This inspector streams one contract at a time through your API market-data permissions. It does not request paid regulatory snapshots. Values older than 15 seconds are marked stale; this does not prove the market price changed."));
    GtkWidget *quoteGrid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(quoteGrid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(quoteGrid), 16);
    ui->quoteContract = gtk_entry_new();
    ui->quoteExchange = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ui->quoteContract), "Contract ID from TWS");
    gtk_entry_set_placeholder_text(GTK_ENTRY(ui->quoteExchange), "Exchange from TWS");
    Field(GTK_GRID(quoteGrid), "Contract ID", ui->quoteContract, 0, "ibkr-quote-contract");
    Field(GTK_GRID(quoteGrid), "Exchange", ui->quoteExchange, 1, "ibkr-quote-exchange");
    gtk_box_append(GTK_BOX(root), quoteGrid);
    GtkWidget *quoteButtons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    ui->quoteStart = gtk_button_new_with_label("Subscribe to quote");
    ui->quoteStop = gtk_button_new_with_label("Cancel quote stream");
    gtk_widget_set_name(ui->quoteStart, "ibkr-quote-start");
    gtk_widget_set_name(ui->quoteStop, "ibkr-quote-stop");
    gtk_box_append(GTK_BOX(quoteButtons), ui->quoteStart);
    gtk_box_append(GTK_BOX(quoteButtons), ui->quoteStop);
    gtk_box_append(GTK_BOX(root), quoteButtons);
    ui->quoteNotice = Text("");
    ui->quoteOutput = Text("Connect read-only, then choose a contract to inspect its quote.");
    gtk_widget_set_name(ui->quoteOutput, "ibkr-quote-output");
    gtk_label_set_selectable(GTK_LABEL(ui->quoteOutput), TRUE);
    gtk_box_append(GTK_BOX(root), ui->quoteNotice);
    gtk_box_append(GTK_BOX(root), ui->quoteOutput);
    ui->status = Text("Not connected. Opening this window does not open a socket.");
    gtk_widget_set_name(ui->status, "ibkr-status");
    gtk_box_append(GTK_BOX(root), ui->status);
    ui->output = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(ui->output), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(ui->output), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(ui->output), GTK_WRAP_WORD_CHAR);
    GtkWidget *outputScroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(outputScroll), ui->output);
    gtk_widget_set_size_request(outputScroll, -1, 300);
    gtk_widget_set_vexpand(outputScroll, TRUE);
    gtk_box_append(GTK_BOX(root), outputScroll);
    g_signal_connect_object(ui->mode, "notify::selected", G_CALLBACK(ProfileChanged), window, 0);
    g_signal_connect_object(ui->program, "notify::selected", G_CALLBACK(ProfileChanged), window, 0);
    g_signal_connect_object(ui->connect, "clicked", G_CALLBACK(ConnectClicked), window, 0);
    g_signal_connect_object(ui->disconnect, "clicked", G_CALLBACK(DisconnectClicked), window, 0);
    g_signal_connect_object(ui->read, "clicked", G_CALLBACK(ReadClicked), window, 0);
    g_signal_connect_object(ui->quoteStart, "clicked", G_CALLBACK(QuoteStartClicked), window, 0);
    g_signal_connect_object(ui->quoteStop, "clicked", G_CALLBACK(QuoteStopClicked), window, 0);
    FillPolicyControls(window, ui, root);
    ContractDetailsControls(window, ui, root);
    MarketRuleControls(window, ui, root);
    ExecutionsControls(window, ui, root);
    PnlControls(window, ui, root);
    SymbolSearchControls(window, ui, root);
    DepthControls(window, ui, root);
    OrdersControls(window, ui, root);
    CompletedControls(window, ui, root);
    HistoricalControls(window, ui, root);
    StreamingControls(window, ui, root);
    CatalogControls(window, ui, root);
    ScannerControls(window, ui, root);
    OptionChainControls(window, ui, root);
    ObservationExportControls(window, ui, root);
#ifdef UMI_IBKR_HAS_FILTERED_CHOICES
    PositionReviewControls(window, ui, root);
#endif
    Controls(ui);
    return window;
}

typedef struct ParentLink { GWeakRef parent; gulong destroyed; } ParentLink;
static void ParentDestroyed(GtkWidget *widget, gpointer data)
{
    (void)widget;
    ParentLink *link = data;
    link->destroyed = 0U;
    g_weak_ref_set(&link->parent, NULL);
}
static void FreeLink(gpointer data)
{
    ParentLink *link = data;
    GObject *parent = g_weak_ref_get(&link->parent);
    if (parent != NULL) {
        if (link->destroyed != 0U) g_signal_handler_disconnect(parent, link->destroyed);
        g_object_unref(parent);
    }
    g_weak_ref_clear(&link->parent); g_free(link);
}
static void OpenClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ParentLink *link = data;
    GObject *parent = g_weak_ref_get(&link->parent);
    if (parent == NULL) return;
    GtkWindow *window = UmiIbkrGtkCreate(GTK_WINDOW(parent));
    gtk_window_present(window);
    g_object_unref(parent);
}
GtkWidget *UmiIbkrGtkWrap(GtkWidget *child, GtkWindow *parent)
{
    if (!GTK_IS_WIDGET(child) || !GTK_IS_WINDOW(parent) || gtk_widget_get_parent(child) != NULL) return child;
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *button = gtk_button_new_with_label("Open Paper / Live broker connections");
    ParentLink *link = g_new0(ParentLink, 1);
    g_weak_ref_init(&link->parent, parent);
/* A retained parent must lose launcher authority when removed from the native window list. The previous implementation remains for engineering review. */
#if 0
    link->destroyed = g_signal_connect(parent, "destroy", G_CALLBACK(ParentDestroyed), link);
#endif
    UmiGtk4ObserveWindowRemoval(parent, G_OBJECT(button), ParentDestroyed, link);
    g_object_set_data_full(G_OBJECT(button), "umicom-broker-parent", link, FreeLink);
    g_signal_connect(button, "clicked", G_CALLBACK(OpenClicked), link);
    gtk_widget_set_halign(button, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(box), button); gtk_box_append(GTK_BOX(box), child);
    gtk_widget_set_vexpand(child, TRUE);
    return box;
}
