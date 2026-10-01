/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/order_csv.h
 * PURPOSE: Export the current applied order query without changing the workspace.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TRADING_ORDER_CSV_H
#define UMICOM_TRADING_ORDER_CSV_H
#include "umicom/trading/workspace.h"
#include "umicom/base/csv_document.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Export the current applied order query without changing the workspace.
 * Captures all matching retained orders on the owning thread. One metadata
 * row is always emitted, including an empty query result. Rows carry the
 * order coordinator revision and both matching and total retained counts.
 * The revision matches order reviews, not the aggregate chart/tape UI token. This is
 * retained local evidence, not broker reconciliation or an execution command.
 * Output is independent of later workspace changes/destruction. The caller
 * destroys it. Failures set *outDocument to NULL; no partial export escapes. */
UmiStatus UmiTradingWorkspaceExportOrdersCsv(const UmiTradingWorkspace *workspace,
    UmiCsvDocument **outDocument);
#ifdef __cplusplus
}
#endif
#endif
