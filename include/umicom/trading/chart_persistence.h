/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/chart_persistence.h
 * PURPOSE: Coordinate explicit chart save, preview and revision-checked restore.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TRADING_CHART_PERSISTENCE_H
#define UMICOM_TRADING_CHART_PERSISTENCE_H
#include "umicom/trading/chart_document.h"
#include "umicom/chart/checkpoint.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTradingChartPersistence UmiTradingChartPersistence;
typedef struct UmiTradingChartPreview {
    uint64_t preview_id;
    UmiChartDocumentSummary saved;
    UmiChartDocumentSummary current;
    UmiChartCheckpointReport storage;
} UmiTradingChartPreview;
/* Owner-thread service; workspace outlives it. Construction performs no I/O. */
UmiStatus UmiTradingChartPersistenceCreate(UmiTradingWorkspace *workspace, UmiTradingChartPersistence **outService);
void UmiTradingChartPersistenceDestroy(UmiTradingChartPersistence *service);
/* Borrow server until unbound or destroyed. Scope is a nonempty identity up to
 * 127 bytes. Rebinding clears tokens and previews, never chart state. NULL
 * server unbinds; no database operation occurs in Bind. */
UmiStatus UmiTradingChartPersistenceBind(UmiTradingChartPersistence *service, UmiDataServer *server, const char *scope);
/* Read and validate the saved copy, remember its revision for a subsequent
 * explicit save, and capture the live drawing/view precondition for Restore.
 * Failure invalidates any old preview. NOT_FOUND with a fresh namespace permits
 * a first save. Recovery is read-only and does not invent a new storage token. */
UmiStatus UmiTradingChartPersistencePreview(UmiTradingChartPersistence *service,
    const char *instrumentId, UmiTradingChartPreview *outPreview);
/* Read copied preview geometry for a review UI. Never returns a mutable owner. */
UmiStatus UmiTradingChartPersistencePreviewDrawing(const UmiTradingChartPersistence *service,
    uint64_t previewId, size_t index, UmiChartDrawingSnapshot *outDrawing);
/* A first save probes for existing storage. If a saved chart already exists,
 * preview it first; Save never silently adopts another writer's revision.
 * Successful saves clear the preview. Conflicts leave live and saved state
 * unchanged and require another explicit Preview before overwriting. */
UmiStatus UmiTradingChartPersistenceSave(UmiTradingChartPersistence *service,
    const char *instrumentId, uint64_t savedAtMs, UmiChartCheckpointReport *outReport);
/* Explicit confirmation to replace all drawings, including locked drawings,
 * and navigation for the reviewed instrument. Reloads storage and compares its
 * digest and token before checking the live model. Stale inputs fail unchanged.
 * The preview ID prevents another panel from substituting its own review.
 * The caller must verify that its displayed instrument is still selected. */
UmiStatus UmiTradingChartPersistenceRestore(UmiTradingChartPersistence *service, const char *instrumentId, uint64_t previewId);
#ifdef __cplusplus
}
#endif
#endif
