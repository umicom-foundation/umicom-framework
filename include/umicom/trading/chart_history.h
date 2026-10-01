/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/chart_history.h
 * PURPOSE: Expose drawing history through the selected-instrument workspace owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TRADING_CHART_HISTORY_H
#define UMICOM_TRADING_CHART_HISTORY_H
#include "umicom/trading/workspace.h"
#include "umicom/chart/drawing_history.h"
#ifdef __cplusplus
extern "C" {
#endif
/** One chronological history across instruments, limited to drawings. The next
 * undo/redo requires that step's instrument to be selected. Navigation, order
 * drafts, executions and broker state are never restored by drawing history.
 * The copied history revision binds an explicit action to what was displayed. */
UmiStatus UmiTradingWorkspaceDrawingHistory(const UmiTradingWorkspace *workspace, UmiChartDrawingHistorySnapshot *out);
UmiStatus UmiTradingWorkspaceUndoDrawing(UmiTradingWorkspace *workspace, const char *instrument, uint64_t expectedRevision);
UmiStatus UmiTradingWorkspaceRedoDrawing(UmiTradingWorkspace *workspace, const char *instrument, uint64_t expectedRevision);
/** Successful saved-chart restore clears drawing history; saving does not.
 * History is session memory and is not persisted with chart documents. */
#ifdef __cplusplus
}
#endif
#endif
