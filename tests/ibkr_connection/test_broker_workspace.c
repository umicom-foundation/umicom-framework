/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_broker_workspace.c
 *
 * PURPOSE:
 *   Verify local broker workspace endpoint retention and native window ownership without connecting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/workspace_gtk4.h"
#include "umicom/broker_connectivity/connection_gtk4.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
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
static GtkWindow *Inspector(GtkWindow *parent)
{
    GListModel *windows = gtk_window_get_toplevels();
    for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i) {
        GtkWindow *window = g_list_model_get_item(windows, i);
        if (gtk_window_get_transient_for(window) == parent &&
            Find(GTK_WIDGET(window), "ibkr-mode") != NULL) return window;
        g_object_unref(window);
    }
    return NULL;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!gtk_init_check()) return 77;
    GtkWindow *parent = GTK_WINDOW(gtk_window_new());
    g_object_ref(parent);
    UmiBrokerAccessPlan plan;
    CHECK(UmiBrokerAccessPlanCreate(UMI_BROKER_ACCESS_LIVE, &plan) == UMI_STATUS_OK);
    CHECK(UmiBrokerAccessPlanSetProgram(&plan, UMI_IBKR_GATEWAY) == UMI_STATUS_OK);
    plan.port = 4111U; plan.clientId = 73;
    UmiBrokerWorkspaceGtk *workspace = NULL;
    CHECK(UmiBrokerWorkspaceGtkCreate(parent, &plan, &workspace) == UMI_STATUS_OK);
    GtkWidget *root = UmiBrokerWorkspaceGtkWidget(workspace);
    g_object_ref(root);
    gtk_window_set_child(parent, root);
    CHECK(Inspector(parent) == NULL);
    GtkWidget *open = Find(root, "broker-workspace-connect");
    CHECK(open != NULL && !gtk_widget_get_sensitive(Find(root, "broker-workspace-read")));
    if (strcmp(argv[1], "invalid") == 0) {
        UmiBrokerWorkspaceGtk *unchanged = workspace;
        CHECK(UmiBrokerAccessPlanCreate(UMI_BROKER_ACCESS_GUEST, &plan) == UMI_STATUS_OK);
        CHECK(UmiBrokerWorkspaceGtkCreate(parent, &plan, &unchanged) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(unchanged == workspace);
    } else {
        /* Mutating the original plan must not alter the controller's endpoint. */
        plan.port = 1U; plan.clientId = 2;
        CHECK(UmiBrokerWorkspaceGtkOpenConnection(workspace) == UMI_STATUS_OK);
        GtkWindow *first = Inspector(parent);
        CHECK(first != NULL);
        CHECK(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(Find(GTK_WIDGET(first), "ibkr-port"))) == 4111);
        CHECK(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(Find(GTK_WIDGET(first), "ibkr-client"))) == 73);
        UmiIbkrConnectionSnapshot *snapshot = g_new0(UmiIbkrConnectionSnapshot, 1);
        CHECK(UmiIbkrGtkCopySnapshot(first, snapshot) == UMI_STATUS_OK);
        CHECK(snapshot->state == UMI_IBKR_IDLE && snapshot->readOnly && !snapshot->requestIssued);
        CHECK(snapshot->requestedEnvironment == UMI_TRADING_LIVE && !snapshot->environmentAttested);
        CHECK(UmiIbkrGtkReadAccount(first, "SYNTHETIC") == UMI_STATUS_UNAVAILABLE);



        if (strcmp(argv[1], "tape-guards") == 0) {
            UmiIbkrTimeSalesReport *report = g_new0(UmiIbkrTimeSalesReport, 1);
            UmiIbkrTimeSalesReport *before = g_new0(UmiIbkrTimeSalesReport, 1);
            memset(report, 0x5a, sizeof(*report)); *before = *report;
            UmiIbkrTimeSalesQuery query = {0}; query.contract.contractId = 123U;
            strcpy(query.contract.exchange, "SMART"); query.kind = UMI_IBKR_TICKS_LAST;
            CHECK(UmiIbkrGtkCopyTimeSales(NULL, 15000U, report) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyTimeSales(first, 0U, report) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyTimeSales(first, 15000U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyTimeSales(first, 15000U, report) == UMI_STATUS_NOT_FOUND);
            CHECK(UmiIbkrGtkStartTimeSales(NULL, &query) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkStartTimeSales(first, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkStartTimeSales(first, &query) == UMI_STATUS_UNAVAILABLE);
            CHECK(UmiIbkrGtkStopTimeSales(first, 0U) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkStopTimeSales(first, 1U) == UMI_STATUS_NOT_FOUND);
            CHECK(!memcmp(report, before, sizeof(*report)));
            gtk_window_destroy(first);
            CHECK(UmiIbkrGtkCopyTimeSales(first, 15000U, report) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkStartTimeSales(first, &query) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkStopTimeSales(first, 1U) == UMI_STATUS_CANCELLED);
            CHECK(!memcmp(report, before, sizeof(*report)));
            g_free(before); g_free(report);
        } else
        if (strcmp(argv[1], "options-guards") == 0) {
            UmiIbkrOptionChainQuery query = {0};
            query.underlyingContractId = 123U;
            strcpy(query.underlyingSymbol, "EXAMPLE"); strcpy(query.underlyingSecurityType, "STK");
            UmiIbkrOptionSelection selection = {0};
            selection.chainRequest = 1U; selection.right = 'C';
            strcpy(selection.currency, "USD"); strcpy(selection.exchange, "SMART");
            UmiIbkrOptionChainSnapshot capture, unchanged;
            memset(&capture, 0x5a, sizeof(capture)); unchanged = capture;
            UmiIbkrOptionChain *item = g_new0(UmiIbkrOptionChain, 1);
            item->underlyingContractId = 999U;
            CHECK(UmiIbkrGtkReadOptionChains(NULL, &query) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkReadOptionChains(first, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkReadOptionChains(first, &query) == UMI_STATUS_UNAVAILABLE);
            CHECK(UmiIbkrGtkCopyOptionChains(NULL, 60000U, &capture) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyOptionChains(first, 0U, &capture) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyOptionChains(first, 60000U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyOptionChains(first, 60000U, &capture) == UMI_STATUS_NOT_FOUND);
            CHECK(UmiIbkrGtkCopyOptionChain(NULL, 1U, 0U, item) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyOptionChain(first, 0U, 0U, item) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyOptionChain(first, 1U, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyOptionChain(first, 1U, 0U, item) == UMI_STATUS_NOT_FOUND);
            CHECK(UmiIbkrGtkAbandonOptionChains(NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkAbandonOptionChains(first, 0U) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkAbandonOptionChains(first, 1U) == UMI_STATUS_NOT_FOUND);
            CHECK(UmiIbkrGtkResolveOption(NULL, &selection, 60000U) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkResolveOption(first, NULL, 60000U) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkResolveOption(first, &selection, 0U) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkResolveOption(first, &selection, 60000U) == UMI_STATUS_NOT_FOUND);
            CHECK(memcmp(&capture, &unchanged, sizeof(capture)) == 0 && item->underlyingContractId == 999U);
            gtk_window_destroy(first);
            CHECK(UmiIbkrGtkReadOptionChains(first, &query) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkCopyOptionChains(first, 60000U, &capture) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkCopyOptionChain(first, 1U, 0U, item) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkAbandonOptionChains(first, 1U) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkResolveOption(first, &selection, 60000U) == UMI_STATUS_CANCELLED);
            CHECK(memcmp(&capture, &unchanged, sizeof(capture)) == 0 && item->underlyingContractId == 999U);
            g_free(item);
        } else
        if (strcmp(argv[1], "details-guards") == 0) {
            UmiIbkrQuoteContract contract = {0};
            CHECK(UmiIbkrQuoteContractParse("123", "SMART", &contract) == UMI_STATUS_OK);
            UmiIbkrContractDetailsSnapshot *capture = g_new0(UmiIbkrContractDetailsSnapshot, 1);
            UmiIbkrContractDetailsSnapshot *unchanged = g_new0(UmiIbkrContractDetailsSnapshot, 1);
            memset(capture, 0x5a, sizeof(*capture)); *unchanged = *capture;
            CHECK(UmiIbkrGtkReadContractDetails(NULL, &contract) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkReadContractDetails(first, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkReadContractDetails(first, &contract) == UMI_STATUS_UNAVAILABLE);
            CHECK(UmiIbkrGtkCopyContractDetails(NULL, 60000U, capture) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyContractDetails(first, 0U, capture) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyContractDetails(first, 60000U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyContractDetails(first, 60000U, capture) == UMI_STATUS_NOT_FOUND);
            CHECK(UmiIbkrGtkAbandonContractDetails(NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkAbandonContractDetails(first, 0U) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkAbandonContractDetails(first, 1U) == UMI_STATUS_NOT_FOUND);
            CHECK(memcmp(capture, unchanged, sizeof(*capture)) == 0);
            gtk_window_destroy(first);
            CHECK(UmiIbkrGtkReadContractDetails(first, &contract) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkCopyContractDetails(first, 60000U, capture) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkAbandonContractDetails(first, 1U) == UMI_STATUS_CANCELLED);
            CHECK(memcmp(capture, unchanged, sizeof(*capture)) == 0);
            g_free(unchanged); g_free(capture);
        } else
        if (strcmp(argv[1], "completed-guards") == 0) {
            UmiIbkrCompletedOrderReport *capture = g_new0(UmiIbkrCompletedOrderReport, 1);
            UmiIbkrCompletedOrderReport *unchanged = g_new0(UmiIbkrCompletedOrderReport, 1);
            memset(capture, 0x5a, sizeof(*capture)); memcpy(unchanged, capture, sizeof(*capture));
            CHECK(UmiIbkrGtkReadCompletedOrders(NULL, true) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkReadCompletedOrders(first, true) == UMI_STATUS_UNAVAILABLE);
            CHECK(UmiIbkrGtkCopyCompletedOrders(NULL, 15000U, capture) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyCompletedOrders(first, 0U, capture) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyCompletedOrders(first, 15000U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyCompletedOrders(first, 15000U, capture) == UMI_STATUS_NOT_FOUND);
            CHECK(memcmp(capture, unchanged, sizeof(*capture)) == 0);
            gtk_window_destroy(first);
            CHECK(UmiIbkrGtkReadCompletedOrders(first, false) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkCopyCompletedOrders(first, 15000U, capture) == UMI_STATUS_CANCELLED);
            CHECK(memcmp(capture, unchanged, sizeof(*capture)) == 0);
            g_free(unchanged); g_free(capture);
        } else
        if (strcmp(argv[1], "scanner-guards") == 0) {
            UmiIbkrScannerQuery query = {0}; query.numberOfRows = 10U;
            strcpy(query.instrument, "STK"); strcpy(query.locationCode, "STK.US.MAJOR");
            strcpy(query.scanCode, "TOP_PERC_GAIN");
            UmiIbkrScannerSnapshot capture, unchanged;
            UmiIbkrScannerRow row, rowBefore;
            memset(&capture, 0x5a, sizeof(capture)); unchanged = capture;
            memset(&row, 0x5a, sizeof(row)); rowBefore = row;
            CHECK(UmiIbkrGtkStartScanner(NULL, &query) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkStartScanner(first, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkStartScanner(first, &query) == UMI_STATUS_UNAVAILABLE);
            CHECK(UmiIbkrGtkCopyScanner(NULL, 60000U, &capture) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyScanner(first, 0U, &capture) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyScanner(first, 60000U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyScanner(first, 60000U, &capture) == UMI_STATUS_NOT_FOUND);
            CHECK(UmiIbkrGtkCancelScanner(NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCancelScanner(first, 0U) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCancelScanner(first, 1U) == UMI_STATUS_NOT_FOUND);
            CHECK(UmiIbkrGtkCopyScannerRow(NULL, 1U, 1U, 0U, &row) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyScannerRow(first, 0U, 1U, 0U, &row) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyScannerRow(first, 1U, 0U, 0U, &row) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyScannerRow(first, 1U, 1U, UMI_IBKR_SCANNER_ROW_LIMIT, &row) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyScannerRow(first, 1U, 1U, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyScannerRow(first, 1U, 1U, 0U, &row) == UMI_STATUS_NOT_FOUND);
            CHECK(memcmp(&capture, &unchanged, sizeof(capture)) == 0);
            CHECK(memcmp(&row, &rowBefore, sizeof(row)) == 0);
            gtk_window_destroy(first);
            CHECK(UmiIbkrGtkStartScanner(first, &query) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkCancelScanner(first, 1U) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkCopyScanner(first, 60000U, &capture) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkCopyScannerRow(first, 1U, 1U, 0U, &row) == UMI_STATUS_CANCELLED);
            CHECK(memcmp(&capture, &unchanged, sizeof(capture)) == 0);
            CHECK(memcmp(&row, &rowBefore, sizeof(row)) == 0);
        } else
        if (strcmp(argv[1], "pnl-guards") == 0) {
            UmiIbkrPnlSelection selection = {0}; strcpy(selection.account, "SYNTHETIC");
            UmiIbkrPnlSnapshot capture, unchanged;
            memset(&capture, 0x5a, sizeof(capture)); unchanged = capture;
            CHECK(UmiIbkrGtkSubscribePnl(NULL, &selection) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkSubscribePnl(first, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkSubscribePnl(first, &selection) == UMI_STATUS_UNAVAILABLE);
            CHECK(UmiIbkrGtkCopyPnl(NULL, 1U, &capture) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyPnl(first, 0U, &capture) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyPnl(first, 1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyPnl(first, 1U, &capture) == UMI_STATUS_NOT_FOUND);
            CHECK(UmiIbkrGtkCancelPnl(NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCancelPnl(first, 0U) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCancelPnl(first, 1U) == UMI_STATUS_NOT_FOUND);
            CHECK(memcmp(&capture, &unchanged, sizeof(capture)) == 0);
            gtk_window_destroy(first);
            CHECK(UmiIbkrGtkSubscribePnl(first, &selection) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkCopyPnl(first, 1U, &capture) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkCancelPnl(first, 1U) == UMI_STATUS_CANCELLED);
            CHECK(memcmp(&capture, &unchanged, sizeof(capture)) == 0);
        } else
        if (strcmp(argv[1], "depth-guards") == 0) {
            UmiIbkrDepthSelection selection = {0};
            selection.contractId = 12345U; selection.rows = 5U;
            strcpy(selection.exchange, "SYNTHETIC");
            UmiIbkrDepthSnapshot *capture = g_new0(UmiIbkrDepthSnapshot, 1);
            UmiIbkrDepthSnapshot *unchanged = g_new0(UmiIbkrDepthSnapshot, 1);
            memset(capture, 0x5a, sizeof(*capture)); *unchanged = *capture;
            CHECK(UmiIbkrGtkSubscribeDepth(NULL, &selection) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkSubscribeDepth(first, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkSubscribeDepth(first, &selection) == UMI_STATUS_UNAVAILABLE);
            CHECK(UmiIbkrGtkCopyDepth(NULL, 1U, capture) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyDepth(first, 0U, capture) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyDepth(first, 1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyDepth(first, 1U, capture) == UMI_STATUS_NOT_FOUND);
            CHECK(UmiIbkrGtkCancelDepth(NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCancelDepth(first, 0U) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCancelDepth(first, 1U) == UMI_STATUS_NOT_FOUND);
            CHECK(memcmp(capture, unchanged, sizeof(*capture)) == 0);
            gtk_window_destroy(first);
            CHECK(UmiIbkrGtkSubscribeDepth(first, &selection) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkCopyDepth(first, 1U, capture) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkCancelDepth(first, 1U) == UMI_STATUS_CANCELLED);
            CHECK(memcmp(capture, unchanged, sizeof(*capture)) == 0);
            g_free(unchanged); g_free(capture);
        } else
        if (strcmp(argv[1], "history-guards") == 0) {
            UmiIbkrHistoricalQuery query = {0};
            query.contract.contractId = 12345U;
            strcpy(query.contract.exchange, "SYNTHETIC");
            query.durationSeconds = 3600U; query.barSeconds = 60U;
            query.dataKind = UMI_IBKR_HISTORY_TRADES;
            UmiIbkrHistoricalSnapshot capture, unchanged;
            UmiIbkrHistoricalBar bar, oldBar;
            UmiChartRenderScene *scene = NULL;
            memset(&capture, 0x5a, sizeof(capture)); unchanged = capture;
            memset(&bar, 0x5a, sizeof(bar)); oldBar = bar;
            CHECK(UmiIbkrGtkReadHistory(NULL, &query) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkReadHistory(first, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkReadHistory(first, &query) == UMI_STATUS_UNAVAILABLE);
            CHECK(UmiIbkrGtkCopyHistory(NULL, 1U, &capture) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyHistory(first, 0U, &capture) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyHistory(first, 1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyHistory(first, 1U, &capture) == UMI_STATUS_NOT_FOUND);
            CHECK(UmiIbkrGtkCopyHistoryBar(first, 0U, 0U, &bar) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyHistoryBar(first, 1U, 0U, &bar) == UMI_STATUS_NOT_FOUND);
            CHECK(UmiIbkrGtkCreateHistoryScene(first, 1U, 0U, 0U, 1U, &scene) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCreateHistoryScene(first, 1U, 0U, 1U, 0U, &scene) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCreateHistoryScene(first, 1U, 0U, 1U, 1U, &scene) == UMI_STATUS_NOT_FOUND);
            CHECK(UmiIbkrGtkCancelHistory(first, 0U) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCancelHistory(first, 1U) == UMI_STATUS_NOT_FOUND);
            CHECK(memcmp(&capture, &unchanged, sizeof(capture)) == 0);
            CHECK(memcmp(&bar, &oldBar, sizeof(bar)) == 0 && scene == NULL);
            gtk_window_destroy(first);
            CHECK(UmiIbkrGtkReadHistory(first, &query) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkCopyHistory(first, 1U, &capture) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkCopyHistoryBar(first, 1U, 0U, &bar) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkCreateHistoryScene(first, 1U, 0U, 1U, 1U, &scene) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkCancelHistory(first, 1U) == UMI_STATUS_CANCELLED);
            CHECK(memcmp(&capture, &unchanged, sizeof(capture)) == 0);
            CHECK(memcmp(&bar, &oldBar, sizeof(bar)) == 0 && scene == NULL);
        } else
        if (strcmp(argv[1], "execution-guards") == 0) {
            UmiIbkrExecutionSnapshot *capture = g_new0(UmiIbkrExecutionSnapshot, 1);
            UmiIbkrExecutionSnapshot *unchanged = g_new0(UmiIbkrExecutionSnapshot, 1);
            memset(capture, 0x5a, sizeof(*capture)); *unchanged = *capture;
            CHECK(UmiIbkrGtkCopyExecutions(NULL, capture) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyExecutions(first, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyExecutions(first, capture) == UMI_STATUS_NOT_FOUND);
            CHECK(UmiIbkrGtkReadExecutions(NULL, "TEST") == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkReadExecutions(first, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkReadExecutions(first, "TEST") == UMI_STATUS_UNAVAILABLE);
            CHECK(UmiIbkrGtkAbandonExecutions(NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkAbandonExecutions(first, 0U) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkAbandonExecutions(first, 1U) == UMI_STATUS_NOT_FOUND);
            CHECK(memcmp(capture, unchanged, sizeof(*capture)) == 0);
            gtk_window_destroy(first);
            CHECK(UmiIbkrGtkCopyExecutions(first, capture) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkReadExecutions(first, "TEST") == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkAbandonExecutions(first, 1U) == UMI_STATUS_CANCELLED);
            CHECK(memcmp(capture, unchanged, sizeof(*capture)) == 0);
            g_free(unchanged); g_free(capture);
        } else
        if (strcmp(argv[1], "order-guards") == 0) {
            UmiIbkrOrderReport *report = g_new0(UmiIbkrOrderReport, 1);
            UmiIbkrOrderReport *unchanged = g_new0(UmiIbkrOrderReport, 1);
            memset(report, 0x5a, sizeof(*report)); *unchanged = *report;
            CHECK(UmiIbkrGtkCopyOrderReport(NULL, 15000U, report) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyOrderReport(first, 0U, report) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyOrderReport(first, 15000U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyOrderReport(first, 15000U, report) == UMI_STATUS_NOT_FOUND);
            CHECK(UmiIbkrGtkRecoverOrders(NULL, UMI_IBKR_ORDERS_THIS_CLIENT) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkRecoverOrders(first, (UmiIbkrOrderScope)0) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkRecoverOrders(first, UMI_IBKR_ORDERS_ALL_CLIENTS) == UMI_STATUS_UNAVAILABLE);
            CHECK(memcmp(report, unchanged, sizeof(*report)) == 0);
            gtk_window_destroy(first);
            CHECK(UmiIbkrGtkCopyOrderReport(first, 15000U, report) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkRecoverOrders(first, UMI_IBKR_ORDERS_THIS_CLIENT) == UMI_STATUS_CANCELLED);
            CHECK(memcmp(report, unchanged, sizeof(*report)) == 0);
            g_free(unchanged); g_free(report);
        } else
        if (strcmp(argv[1], "search-guards") == 0) {
            UmiIbkrSymbolSearchSnapshot search, unchanged;
            memset(&search, 0x5a, sizeof(search));
            unchanged = search;
            char longPattern[129];
            memset(longPattern, 'A', sizeof(longPattern) - 1U);
            longPattern[sizeof(longPattern) - 1U] = '\0';
            CHECK(UmiIbkrGtkCopyInstrumentSearch(NULL, &search) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyInstrumentSearch(first, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyInstrumentSearch(first, &search) == UMI_STATUS_NOT_FOUND);
            CHECK(UmiIbkrGtkSearchInstruments(first, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkSearchInstruments(first, "") == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkSearchInstruments(first, longPattern) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkSearchInstruments(NULL, "EXAMPLE") == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkSearchInstruments(first, "EXAMPLE") == UMI_STATUS_UNAVAILABLE);
            CHECK(UmiIbkrGtkAbandonInstrumentSearch(NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkAbandonInstrumentSearch(first) == UMI_STATUS_UNAVAILABLE);
            CHECK(memcmp(&search, &unchanged, sizeof(search)) == 0);
            gtk_window_destroy(first);
            CHECK(UmiIbkrGtkCopyInstrumentSearch(first, &search) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkSearchInstruments(first, "EXAMPLE") == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkAbandonInstrumentSearch(first) == UMI_STATUS_CANCELLED);
            CHECK(memcmp(&search, &unchanged, sizeof(search)) == 0);
        } else if (strcmp(argv[1], "quote-mode-guards") == 0) {
            GtkWidget *type = Find(GTK_WIDGET(first), "ibkr-quote-data-type");
            CHECK(GTK_IS_DROP_DOWN(type));
            CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(type)) == 0U);
            UmiIbkrQuoteContract contract = {0};
            CHECK(UmiIbkrQuoteContractParse("1", "SMART", &contract) == UMI_STATUS_OK);
            CHECK(UmiIbkrGtkQuoteSubscribeWithDataType(first, &contract, UMI_IBKR_DATA_UNKNOWN) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkQuoteSubscribeWithDataType(first, &contract, (UmiIbkrMarketDataType)5) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkQuoteSubscribeWithDataType(NULL, &contract, UMI_IBKR_DATA_DELAYED) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkQuoteSubscribeWithDataType(first, &contract, UMI_IBKR_DATA_DELAYED) == UMI_STATUS_UNAVAILABLE);
            CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(type)) == 0U);
            gtk_window_destroy(first);
            CHECK(UmiIbkrGtkQuoteSubscribeWithDataType(first, &contract, UMI_IBKR_DATA_DELAYED) == UMI_STATUS_CANCELLED);
        } else if (strcmp(argv[1], "quote-guards") == 0) {
            UmiIbkrQuoteContract contract = {0};
            CHECK(UmiIbkrQuoteContractParse("1", "SMART", &contract) == UMI_STATUS_OK);
            UmiIbkrQuoteSnapshot quote;
            memset(&quote, 0x5a, sizeof(quote));
            UmiIbkrQuoteSnapshot unchanged = quote;
            CHECK(UmiIbkrGtkCopyQuote(NULL, 15000U, &quote) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyQuote(first, 0U, &quote) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyQuote(first, 15000U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkCopyQuote(first, 15000U, &quote) == UMI_STATUS_NOT_FOUND);
            CHECK(UmiIbkrGtkQuoteSubscribe(first, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiIbkrGtkQuoteSubscribe(first, &contract) == UMI_STATUS_UNAVAILABLE);
            CHECK(UmiIbkrGtkQuoteCancel(first) == UMI_STATUS_UNAVAILABLE);
            CHECK(UmiIbkrGtkQuoteCancel(NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(memcmp(&quote, &unchanged, sizeof(quote)) == 0);
            gtk_window_destroy(first);
            CHECK(UmiIbkrGtkCopyQuote(first, 15000U, &quote) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkQuoteSubscribe(first, &contract) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkQuoteCancel(first) == UMI_STATUS_CANCELLED);
            CHECK(memcmp(&quote, &unchanged, sizeof(quote)) == 0);
        } else if (strcmp(argv[1], "reuse") == 0) {
            g_signal_emit_by_name(open, "clicked");
            GtkWindow *again = Inspector(parent);
            CHECK(again == first);
            g_object_unref(again);
        } else if (strcmp(argv[1], "reopen") == 0) {
            gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(GTK_WIDGET(first), "ibkr-live-ack")), TRUE);
            gtk_window_destroy(first);
            CHECK(Inspector(parent) == NULL);
            snapshot->providerCode = 12345;
            CHECK(UmiIbkrGtkCopySnapshot(first, snapshot) == UMI_STATUS_CANCELLED);
            CHECK(snapshot->providerCode == 12345);
            CHECK(UmiBrokerWorkspaceGtkOpenConnection(workspace) == UMI_STATUS_OK);
            GtkWindow *again = Inspector(parent);
            CHECK(again != NULL && again != first);
            CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(Find(GTK_WIDGET(again), "ibkr-live-ack"))));
            g_object_unref(again);
        } else if (strcmp(argv[1], "parent-retired") == 0) {
            gtk_window_destroy(parent);
            CHECK(Inspector(parent) == NULL);
            CHECK(UmiBrokerWorkspaceGtkOpenConnection(workspace) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkCopySnapshot(first, snapshot) == UMI_STATUS_CANCELLED);
            g_signal_emit_by_name(open, "clicked");
            CHECK(Inspector(parent) == NULL);
        } else if (strcmp(argv[1], "retained-controls") != 0) return 2;
        g_free(snapshot);
        g_object_unref(first);
    }
    UmiBrokerWorkspaceGtkDestroy(workspace);
    /* The root deliberately remains alive after controller destruction. */
    g_signal_emit_by_name(open, "clicked");
    CHECK(Inspector(parent) == NULL);
    gtk_window_destroy(parent);
    g_object_unref(parent);
    g_object_unref(root);
    return 0;
}
