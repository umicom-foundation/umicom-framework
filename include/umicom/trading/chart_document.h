/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/chart_document.h
 * PURPOSE: Capture and restore one instrument without changing market data or order state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TRADING_CHART_DOCUMENT_H
#define UMICOM_TRADING_CHART_DOCUMENT_H
#include "umicom/trading/workspace.h"
#include "umicom/chart/document.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Owner-thread copied capture of a known instrument, selected or not. The
 * caller destroys the independent immutable result. No selection reconciliation
 * or market-data refresh occurs. Failure sets *outDocument=NULL. */
UmiStatus UmiTradingWorkspaceCaptureChart(const UmiTradingWorkspace *workspace,
    const char *instrumentId, UmiChartDocument **outDocument);
/* Validate Trader's currently supported trend/support/resistance geometry
 * without changing the model. A generic Framework document may support other
 * tools; Trader refuses them instead of silently dropping their drawings. */
UmiStatus UmiTradingChartDocumentValidate(const UmiChartDocument *document);
/* Explicitly replace this instrument's entire drawing set (including locked
 * drawings) and view. Both the registry revision and current view must still
 * match the caller's review. Failure changes neither. Other instruments, live
 * prices, order tickets and selection survive. Restored drawings receive fresh
 * local revisions so an old remove/edit action cannot act on restored content. */
UmiStatus UmiTradingWorkspaceRestoreChart(UmiTradingWorkspace *workspace,
    const UmiChartDocument *document, uint64_t expectedDrawingRevision,
    const UmiChartNavigation *expectedNavigation);
#ifdef __cplusplus
}
#endif
#endif
