/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/session_report.h
 * PURPOSE: Capture and reconcile a retained local trading book without changing orders or selection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_TRADING_SESSION_REPORT_H
#define UMICOM_TRADING_SESSION_REPORT_H
#include "umicom/trading/workspace.h"
#include "umicom/base/csv_document.h"
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTradingSessionReport UmiTradingSessionReport;
typedef enum UmiTradingSessionArea {
    UMI_SESSION_ORDER = 1, UMI_SESSION_EXECUTION, UMI_SESSION_POSITION, UMI_SESSION_BOOK
} UmiTradingSessionArea;
/* Several discrepancies in one retained record share one issue row. */
typedef enum UmiTradingSessionIssueFlag {
    UMI_SESSION_QUANTITY = 1U, UMI_SESSION_PRICE = 2U, UMI_SESSION_STATE = 4U,
    UMI_SESSION_CONTRACT = 8U, UMI_SESSION_DUPLICATE = 16U,
    UMI_SESSION_MISSING = 32U, UMI_SESSION_ACCOUNT = 64U,
    UMI_SESSION_ENVIRONMENT = 128U, UMI_SESSION_PNL = 256U
} UmiTradingSessionIssueFlag;
typedef struct UmiTradingSessionIssue {
    UmiTradingSessionArea area;
    UmiFinancialId id;
    uint32_t flags;
} UmiTradingSessionIssue;
typedef struct UmiTradingSessionSummary {
    UmiFinancialId account;
    UmiTradingEnvironment environment;
    uint64_t revision;
    char instrumentFilter[UMI_TRADING_WORKSPACE_FILTER_CAPACITY];
    size_t retainedOrders, retainedExecutions, retainedPositions;
    size_t orders, executions, positions, currencies, issues;
    bool totalsAvailable;
} UmiTradingSessionSummary;
typedef struct UmiTradingSessionExecution {
    UmiExecutionReport fill;
    UmiOrderRequest order;
    bool hasOrder;
} UmiTradingSessionExecution;
typedef struct UmiTradingSessionCurrency {
    UmiCurrency currency;
    size_t positions;
    double realisedPnl;
} UmiTradingSessionCurrency;

/* Call on the workspace's owning thread. The report copies every retained
 * order/fill/position, replays fills in arrival order using the canonical
 * arithmetic, then applies an ASCII-case-insensitive instrument ID/symbol/
 * venue/currency substring filter. NULL filter means all. Order/watchlist
 * filters and selection are untouched. No broker state is requested.
 * All discrepancy rows cover the whole book, even under a narrow filter.
 * A discrepancy withholds currency totals but leaves source rows readable.
 * Reports outlive the workspace and never authorize or perform trading. */
UmiStatus UmiTradingSessionReportCapture(const UmiTradingWorkspace *workspace,
    const char *instrumentFilter, UmiTradingSessionReport **outReport);
void UmiTradingSessionReportDestroy(UmiTradingSessionReport *report);
UmiStatus UmiTradingSessionReportSummary(const UmiTradingSessionReport *report,
    UmiTradingSessionSummary *outSummary);
/* Rows are copies. Orders/positions use creation order; fills use arrival
 * order, which can differ from event-time sorting. Output is cleared on error. */
UmiStatus UmiTradingSessionReportOrderAt(const UmiTradingSessionReport *report,
    size_t index, UmiOrder *outOrder);
UmiStatus UmiTradingSessionReportExecutionAt(const UmiTradingSessionReport *report,
    size_t index, UmiTradingSessionExecution *outExecution);
UmiStatus UmiTradingSessionReportPositionAt(const UmiTradingSessionReport *report,
    size_t index, UmiPosition *outPosition);
UmiStatus UmiTradingSessionReportCurrencyAt(const UmiTradingSessionReport *report,
    size_t index, UmiTradingSessionCurrency *outCurrency);
UmiStatus UmiTradingSessionReportIssueAt(const UmiTradingSessionReport *report,
    size_t index, UmiTradingSessionIssue *outIssue);
/* Current means the original workspace identity and revision/account/environment. It says
 * nothing about external data completeness or broker reconciliation. */
UmiStatus UmiTradingSessionReportIsCurrent(const UmiTradingSessionReport *report,
    const UmiTradingWorkspace *workspace, bool *outCurrent);
/* Text has a size-query form: output NULL, capacity 0, required non-NULL.
 * Short buffers return CAPACITY_EXCEEDED with empty output and required bytes
 * including the terminator. CSV owns its bytes and uses exact round-trip
 * decimal formatting for the source doubles; totals never convert currencies. */
UmiStatus UmiTradingSessionReportDescribe(const UmiTradingSessionReport *report,
    char *output, size_t capacity, size_t *required);
UmiStatus UmiTradingSessionReportExportCsv(const UmiTradingSessionReport *report,
    UmiCsvDocument **outDocument);
const char *UmiTradingSessionAreaText(UmiTradingSessionArea area);
#ifdef __cplusplus
}
#endif
#endif
