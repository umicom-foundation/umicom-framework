/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/order_report_private.h
 * PURPOSE: Copy order evidence without invoking selection-reconciling workspace snapshots.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TRADING_ORDER_REPORT_PRIVATE_H
#define UMICOM_TRADING_ORDER_REPORT_PRIVATE_H
#include "umicom/trading/workspace.h"
typedef struct UmiTradingOrderReport {
    UmiTradingOrderQuery query;
    uint64_t revision;
    size_t retained, matching;
    UmiOrder orders[UMI_TRADING_MAX_ORDERS];
} UmiTradingOrderReport;
/* Private copy boundary: only workspace.c reads the canonical owner. The
 * revision is the order coordinator revision, as used by order reviews,
 * rather than the aggregate chart/tape revision in the general UI snapshot. */
UmiStatus UmiTradingCopyOrderReport(const UmiTradingWorkspace *workspace,
    UmiTradingOrderReport *outReport);
#endif
