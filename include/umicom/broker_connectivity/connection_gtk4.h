/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/connection_gtk4.h
 *
 * PURPOSE:
 *   Present read-only broker controls and copy observations for application workspaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IBKR_CONNECTION_GTK4_H
#define UMICOM_IBKR_CONNECTION_GTK4_H
#include <gtk/gtk.h>
#include "umicom/broker_connectivity/connection.h"
#include "umicom/broker_connectivity/quotes.h"
#include "umicom/broker_connectivity/symbol_search.h"
#include "umicom/broker_connectivity/contract_details.h"
#include "umicom/broker_connectivity/option_contract.h"
#include "umicom/broker_connectivity/time_sales_report.h"
#include "umicom/broker_connectivity/order_recovery.h"
#include "umicom/broker_connectivity/completed_orders.h"
#include "umicom/broker_connectivity/execution_observation.h"
#include "umicom/broker_connectivity/historical_chart.h"
#include "umicom/broker_connectivity/realtime_chart.h"
#include "umicom/broker_connectivity/market_depth.h"
#include "umicom/broker_connectivity/pnl.h"
#include "umicom/broker_connectivity/scanner.h"
#ifdef __cplusplus
extern "C" {
#endif
/* GTK-main-context only. The toplevel is initially unpresented and owns its
 * connection. gtk_window_destroy closes the socket immediately, even if a
 * caller retains a reference to the destroyed window or one of its children. */
/* A non-null parent must still be registered as a native toplevel. All fields
 * are prepared before application observers run. Command/copy APIs refuse
 * access during construction; no connection is opened by a constructor.
 * A host may immediately close the child or parent during association:
 * construction then returns NULL, even if that observer retains the object. */
GtkWindow *UmiIbkrGtkCreate(GtkWindow *parent);
/* Copy observations on the GTK main thread without transferring socket ownership
 * or permitting orders. An unconnected inspector returns IDLE. Invalid or
 * retired windows leave the destination unchanged. Requested mode is intent. */
UmiStatus UmiIbkrGtkCopySnapshot(GtkWindow *window,
    UmiIbkrConnectionSnapshot *outSnapshot);
/**
 * @brief Create an unconnected broker inspector with Paper or Live preselected.
 * @param parent Optional owner window.
 * @param live Zero requests Paper; one requests Live. Other values return NULL.
 * @return A GTK-owned, unpresented window, or NULL for an invalid mode.
 * Selection is a request, not proof of broker authentication. Live consent
 * remains unchecked and the user must explicitly connect after logging into TWS.
 */
GtkWindow *UmiIbkrGtkCreateForEnvironment(GtkWindow *parent, int live);


/* The original one-shot contract is retained for review. Native callers now
 * share the explicit refresh service, with fresh request IDs for each capture. */
#if 0
/* Request one overview and positions snapshot through the existing inspector
 * session. The account must come from that session's managed account list.
 * No socket is opened and no order is sent. Reconnect to read another report. */
#endif
/* Request an overview and positions through the existing inspector session.
 * The account must come from its managed account list. A second capture is
 * admitted only after both responses finish; BUSY preserves the pending one.
 * No socket is opened or order sent. Failed/stale sessions need reconnect. */
UmiStatus UmiIbkrGtkReadAccount(GtkWindow *window, const char *account);

/* One selected quote stream is shared by the inspector and its workspace.
 * These GTK-main-thread calls never create a connection, submit an order or
 * authenticate. Subscribe requires READY; cancel preserves the stream if its
 * request cannot be queued. Copy recalculates age without pumping transport.
 * Missing observations return NOT_FOUND; invalid/retired input never changes
 * the output. Native retirement during publication returns CANCELLED. */
UmiStatus UmiIbkrGtkQuoteSubscribe(GtkWindow *window,
    const UmiIbkrQuoteContract *contract);
/* The selected mode changes only on an explicit new quote subscription. */
UmiStatus UmiIbkrGtkQuoteSubscribeWithDataType(GtkWindow *window,
    const UmiIbkrQuoteContract *contract, UmiIbkrMarketDataType requestedDataType);
UmiStatus UmiIbkrGtkQuoteCancel(GtkWindow *window);
UmiStatus UmiIbkrGtkCopyQuote(GtkWindow *window, uint64_t maximumAgeMilliseconds,
    UmiIbkrQuoteSnapshot *outQuote);

/* Search shares the inspector request namespace and core pacing. A candidate
 * is descriptive, not authority to subscribe or trade. Copy never pumps the
 * socket. Abandon retires a local lookup, not market subscriptions or orders.
 * Invalid and retired calls preserve output. All calls are GTK-main-thread only. */
UmiStatus UmiIbkrGtkSearchInstruments(GtkWindow *window, const char *pattern);
UmiStatus UmiIbkrGtkCopyInstrumentSearch(GtkWindow *window,
    UmiIbkrSymbolSearchSnapshot *outSearch);
UmiStatus UmiIbkrGtkAbandonInstrumentSearch(GtkWindow *window);

/* Exact instrument descriptions share the inspector request namespace. They do
 * not require a quote subscription. Copy uses bounded age without I/O and leaves
 * output unchanged on failure. Abandon affects only the expected local lookup.
 * These operations are GTK-main-thread only and never submit orders. */
UmiStatus UmiIbkrGtkReadContractDetails(GtkWindow *window, const UmiIbkrQuoteContract *contract);
UmiStatus UmiIbkrGtkCopyContractDetails(GtkWindow *window, uint64_t maximumAgeMilliseconds,
    UmiIbkrContractDetailsSnapshot *out);
UmiStatus UmiIbkrGtkAbandonContractDetails(GtkWindow *window, uint32_t expectedRequest);

/* Option definitions are candidates, not proof that each expiry/strike pairing
 * is valid. Explicit resolution writes the shared contract-description capture;
 * it does not subscribe to quotes or submit orders. Copies do no I/O and retain
 * original item indices. GTK-main-thread only, with unchanged output on failure. */
UmiStatus UmiIbkrGtkReadOptionChains(GtkWindow *, const UmiIbkrOptionChainQuery *);
UmiStatus UmiIbkrGtkCopyOptionChains(GtkWindow *, uint64_t maximumAgeMilliseconds, UmiIbkrOptionChainSnapshot *);
UmiStatus UmiIbkrGtkCopyOptionChain(GtkWindow *, uint32_t expectedRequest, size_t index, UmiIbkrOptionChain *);
UmiStatus UmiIbkrGtkAbandonOptionChains(GtkWindow *, uint32_t expectedRequest);
UmiStatus UmiIbkrGtkResolveOption(GtkWindow *, const UmiIbkrOptionSelection *, uint64_t maximumAgeMilliseconds);


/* One explicit native tick stream shares the inspector connection and pacing.
 * Report rows retain receipt order, exact provider text and independent ages.
 * Stop requires the observed request ID; errors preserve the stream or output.
 * These GTK-main-thread calls never open a socket or submit orders themselves. */
UmiStatus UmiIbkrGtkStartTimeSales(GtkWindow *, const UmiIbkrTimeSalesQuery *);
UmiStatus UmiIbkrGtkStopTimeSales(GtkWindow *, uint32_t expectedRequest);
UmiStatus UmiIbkrGtkCopyTimeSales(GtkWindow *, uint64_t maximumAgeMilliseconds, UmiIbkrTimeSalesReport *);

/* Open-order recovery shares the existing inspector session. It does not bind,
 * submit, change or cancel orders. One capture per connection; reconnect for
 * another. Copy uses no I/O and preserves output on invalid/retired calls.
 * A report can include several accounts: each row retains its account identity. */
UmiStatus UmiIbkrGtkRecoverOrders(GtkWindow *window, UmiIbkrOrderScope scope);
UmiStatus UmiIbkrGtkCopyOrderReport(GtkWindow *window, uint64_t maximumAgeMilliseconds,
    UmiIbkrOrderReport *outReport);

/* Completed orders use a separate one-shot capture in this same session.
 * API-only is all visible API clients, not the current account picker.
 * Copy performs no I/O; invalid/retired calls preserve the full output.
 * Call only on the GTK thread. Reconnect for a new completed-order capture. */
UmiStatus UmiIbkrGtkReadCompletedOrders(GtkWindow *window, bool apiOnly);
UmiStatus UmiIbkrGtkCopyCompletedOrders(GtkWindow *window, uint64_t maximumAgeMilliseconds,
    UmiIbkrCompletedOrderReport *out);

/* Recent execution reports share the inspector capture. Account must belong to
 * the connected session. Completion is an end marker, not a full-history claim;
 * commissions can arrive later. Copy performs no I/O and preserves output on
 * failure. Abandon requires the expected capture ID and never cancels orders.
 * All operations are GTK-main-thread only. */
UmiStatus UmiIbkrGtkReadExecutions(GtkWindow *window, const char *account);
UmiStatus UmiIbkrGtkCopyExecutions(GtkWindow *window, UmiIbkrExecutionSnapshot *out);
UmiStatus UmiIbkrGtkAbandonExecutions(GtkWindow *window, uint32_t expectedRequest);


/* Depth controls share the inspector's current subscription on the GTK thread.
 * Stop requires the observed request ID; failed copies leave output unchanged. */
UmiStatus UmiIbkrGtkSubscribeDepth(GtkWindow *window, const UmiIbkrDepthSelection *selection);
UmiStatus UmiIbkrGtkCancelDepth(GtkWindow *window, uint32_t expectedRequest);
UmiStatus UmiIbkrGtkCopyDepth(GtkWindow *window, uint64_t maximumAgeMilliseconds,
    UmiIbkrDepthSnapshot *out);

/* P&L shares the inspector's current stream and advertised-account validation.
 * No currency is inferred. Copies do no I/O and leave output unchanged on failure.
 * All calls are GTK-main-thread only; Stop requires the observed request ID. */
UmiStatus UmiIbkrGtkSubscribePnl(GtkWindow *window, const UmiIbkrPnlSelection *selection);
UmiStatus UmiIbkrGtkCancelPnl(GtkWindow *window, uint32_t expectedRequest);
UmiStatus UmiIbkrGtkCopyPnl(GtkWindow *window, uint64_t maximumAgeMilliseconds,
    UmiIbkrPnlSnapshot *out);

/* Scanner observations share the inspector subscription. Start refuses to hide
 * an existing subscription that still needs cancellation, including failed ones.
 * Copy performs no I/O. Rows require both the observed request and generation;
 * a replacement returns NOT_FOUND or BUSY without changing output. GTK thread only. */
UmiStatus UmiIbkrGtkStartScanner(GtkWindow *window, const UmiIbkrScannerQuery *query);
UmiStatus UmiIbkrGtkCancelScanner(GtkWindow *window, uint32_t expectedRequest);
UmiStatus UmiIbkrGtkCopyScanner(GtkWindow *window, uint64_t maximumAgeMilliseconds,
    UmiIbkrScannerSnapshot *out);
UmiStatus UmiIbkrGtkCopyScannerRow(GtkWindow *window, uint32_t expectedRequest,
    uint64_t generation, size_t index, UmiIbkrScannerRow *out);

/* Finite chart captures share the inspector request namespace and pacing.
 * No operation opens a connection or places orders. Copy and scene creation
 * perform no I/O. All calls run on the GTK main thread; output is unchanged on
 * failure. A retained raw bar is not proof of freshness: inspect its snapshot.
 * A successful scene owns its drawing commands and must be destroyed with
 * umi_chart_render_scene_destroy. A build without the chart model returns
 * NOT_IMPLEMENTED. Capture age is not the market age of the last bar. */
/* One explicit five-second stream shares the inspector's connection and pacing.
 * Stop uses the observed request ID so stale controls cannot cancel a replacement.
 * Copy and scene creation do not pump transport or start subscriptions. Outputs
 * remain unchanged on failure; returned scenes belong to the caller. */
UmiStatus UmiIbkrGtkStartRealtime(GtkWindow *window, const UmiIbkrRealtimeQuery *query);
UmiStatus UmiIbkrGtkStopRealtime(GtkWindow *window, uint32_t expectedRequest);
UmiStatus UmiIbkrGtkCopyRealtime(GtkWindow *window, uint64_t maximumAgeMilliseconds,
    UmiIbkrRealtimeSnapshot *out);
UmiStatus UmiIbkrGtkCopyRealtimeBar(GtkWindow *window, uint32_t expectedRequest,
    size_t index, UmiIbkrRealtimeBar *out);
UmiStatus UmiIbkrGtkCreateRealtimeScene(GtkWindow *window, uint32_t expectedRequest,
    size_t firstBar, size_t barCount, uint64_t maximumAgeMilliseconds, UmiChartRenderScene **out);

UmiStatus UmiIbkrGtkReadHistory(GtkWindow *window, const UmiIbkrHistoricalQuery *query);
UmiStatus UmiIbkrGtkCancelHistory(GtkWindow *window, uint32_t expectedRequest);
UmiStatus UmiIbkrGtkCopyHistory(GtkWindow *window, uint64_t maximumAgeMilliseconds,
    UmiIbkrHistoricalSnapshot *out);
UmiStatus UmiIbkrGtkCopyHistoryBar(GtkWindow *window, uint32_t expectedRequest,
    size_t index, UmiIbkrHistoricalBar *out);
UmiStatus UmiIbkrGtkCreateHistoryScene(GtkWindow *window, uint32_t expectedRequest,
    size_t firstBar, size_t barCount, uint64_t maximumAgeMilliseconds, UmiChartRenderScene **out);

/* Wrap an existing unparented widget without changing its original owner or
 * content. The button keeps only a weak parent reference. No connection opens
 * until the user explicitly chooses Connect in the separate window. */
GtkWidget *UmiIbkrGtkWrap(GtkWidget *child, GtkWindow *parent);
/**
 * @brief Create a floating broker button for an existing window's toolbar.
 * @param parent Live owner window; the launcher keeps only a weak reference.
 * @return An unparented button, or NULL for invalid input.
 * Clicking presents an unconnected inspector. A removed parent makes the button inert.
 */
GtkWidget *UmiIbkrGtkLauncherCreate(GtkWindow *parent);

#ifdef __cplusplus
}
#endif
#endif
