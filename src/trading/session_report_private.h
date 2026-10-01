/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/session_report_private.h
 * PURPOSE: Keep the canonical capture boundary separate from immutable session analysis.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_TRADING_SESSION_REPORT_PRIVATE_H
#define UMICOM_TRADING_SESSION_REPORT_PRIVATE_H
#include "umicom/trading/session_report.h"
#define UMI_SESSION_ISSUE_CAPACITY (2U * UMI_TRADING_MAX_ORDERS + 2U * UMI_TRADING_MAX_POSITIONS + 1U)
typedef struct UmiTradingSessionSource {
    UmiFinancialId account;
    UmiTradingEnvironment environment;
    uint64_t revision;
    uint64_t ownerIdentity;
    size_t orderCount, executionCount, positionCount;
    UmiOrder orders[UMI_TRADING_MAX_ORDERS];
    UmiExecutionReport executions[UMI_TRADING_MAX_ORDERS];
    UmiPosition positions[UMI_TRADING_MAX_POSITIONS];
} UmiTradingSessionSource;
struct UmiTradingSessionReport {
    UmiTradingSessionSource source;
    UmiTradingSessionSummary summary;
    size_t orderRows[UMI_TRADING_MAX_ORDERS], executionRows[UMI_TRADING_MAX_ORDERS];
    size_t positionRows[UMI_TRADING_MAX_POSITIONS];
    size_t executionOrders[UMI_TRADING_MAX_ORDERS];
    UmiTradingSessionCurrency currencies[UMI_TRADING_MAX_POSITIONS];
    UmiTradingSessionIssue issues[UMI_SESSION_ISSUE_CAPACITY];
};
/* Only workspace.c reads the live owner. No selection-repair snapshot call. */
UmiStatus UmiTradingCopySessionSource(const UmiTradingWorkspace *workspace,
    UmiTradingSessionSource *outSource);
UmiStatus UmiTradingSessionSourceCurrent(const UmiTradingWorkspace *workspace,
    const UmiTradingSessionSource *source, bool *outCurrent);
/* Internal pure analysis boundary also permits damaged-evidence regression
 * fixtures. It is not an import or a way to replace the live trading book. */
UmiStatus UmiTradingBuildSessionReport(const UmiTradingSessionSource *source,
    const char *filter, UmiTradingSessionReport **outReport);
#endif
