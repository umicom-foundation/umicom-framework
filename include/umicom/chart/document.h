/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/document.h
 * PURPOSE: Capture an immutable chart view and its drawings independently of live market data.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CHART_DOCUMENT_H
#define UMICOM_CHART_DOCUMENT_H
#include "umicom/chart/drawing.h"
#include "umicom/chart/navigation.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiChartDocument UmiChartDocument;
typedef struct UmiChartDocumentSummary {
    char pane_id[128];
    UmiChartNavigation navigation;
    size_t drawing_count;
    uint64_t source_revision;
} UmiChartDocumentSummary;
/* Copy up to the full registry capacity. All drawings must belong to the
 * nonempty pane and have distinct IDs, finite coordinates and boolean flags.
 * The view's bar count is 0..UMI_CHART_MAX_POINTS; pinned is 0 or 1. Strings
 * must fit their fixed arrays. Geometry retains exact double values and signed
 * timestamps. No market data, orders or global indicator settings are copied.
 * A successful caller-owned document is immutable and survives owner changes.
 * Failure sets *outDocument=NULL. Zero drawings are a valid saved empty chart. */
UmiStatus UmiChartDocumentCreate(const char *paneId, const UmiChartNavigation *navigation,
    const UmiChartDrawingSnapshot *drawings, size_t count, uint64_t sourceRevision,
    UmiChartDocument **outDocument);
/* Read a registry only on its owning thread. No selection or geometry changes. */
UmiStatus UmiChartDocumentCapture(const UmiChartDrawingRegistry *registry,
    const char *paneId, const UmiChartNavigation *navigation, UmiChartDocument **outDocument);
void UmiChartDocumentDestroy(UmiChartDocument *document);
/* Failed copied reads leave output unchanged; zero-based order is retained. */
UmiStatus UmiChartDocumentGetSummary(const UmiChartDocument *document, UmiChartDocumentSummary *outSummary);
UmiStatus UmiChartDocumentDrawingAt(const UmiChartDocument *document, size_t index, UmiChartDrawingSnapshot *outDrawing);
/* Apply only this pane through the registry's revision-checked replacement.
 * Unrelated panes survive. The caller separately owns view publication. */
UmiStatus UmiChartDocumentApplyDrawings(const UmiChartDocument *document,
    UmiChartDrawingRegistry *registry, uint64_t expectedRevision);
#ifdef __cplusplus
}
#endif
#endif
