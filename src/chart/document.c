/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/document.c
 * PURPOSE: Own drawing and view captures for previews, storage and atomic publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "document_private.h"
#include "umicom/chart/drawing_validation.h"
#include <stdlib.h>
#include <string.h>

static int ValidPane(const char *pane)
{
    if (pane == NULL || pane[0] == '\0') return 0;
    for (size_t i = 1U; i < 128U; ++i) if (pane[i] == '\0') return 1;
    return 0;
}
static int ValidNavigation(const UmiChartNavigation *navigation)
{
/* Chart captures now validate their shared fixed-interval setting so a saved view cannot restore unsupported aggregation. The previous implementation remains for engineering review. */
#if 0
    return navigation != NULL && navigation->visible_bars <= UMI_CHART_MAX_POINTS &&
        (navigation->pinned == 0 || navigation->pinned == 1);
#endif
    return navigation != NULL && navigation->visible_bars <= UMI_CHART_MAX_POINTS &&
        (navigation->pinned == 0 || navigation->pinned == 1) && UmiChartTimeframeValid(navigation->interval_ms);
}
UmiStatus UmiChartDocumentValidateOwned(const UmiChartDocument *document)
{
    if (document == NULL || !ValidPane(document->summary.pane_id) ||
        !ValidNavigation(&document->summary.navigation) || document->summary.drawing_count > UMI_CHART_DRAWING_CAPACITY)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < document->summary.drawing_count; ++i) {
        const UmiChartDrawingSnapshot *drawing = &document->drawings[i];
        UmiStatus status = UmiChartDrawingValidateGeometry(drawing);
        if (status != UMI_STATUS_OK) return status;
        if (strcmp(drawing->pane_id, document->summary.pane_id) != 0) return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(drawing->id, document->drawings[j].id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiChartDocumentCreate(const char *paneId, const UmiChartNavigation *navigation,
    const UmiChartDrawingSnapshot *drawings, size_t count, uint64_t sourceRevision,
    UmiChartDocument **outDocument)
{
    if (outDocument == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outDocument = NULL;
    if (!ValidPane(paneId) || !ValidNavigation(navigation) ||
        (drawings == NULL && count != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_CHART_DRAWING_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiChartDocument *document = calloc(1U, sizeof(*document));
    if (document == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(document->summary.pane_id, paneId, strlen(paneId) + 1U);
    document->summary.navigation = *navigation;
    document->summary.drawing_count = count;
    document->summary.source_revision = sourceRevision;
    if (count != 0U) memcpy(document->drawings, drawings, count * sizeof(*drawings));
    UmiStatus status = UmiChartDocumentValidateOwned(document);
    if (status != UMI_STATUS_OK) { free(document); return status; }
    *outDocument = document; return UMI_STATUS_OK;
}
/* Registry capture now supplies the drawings and observation token together.
 * The previous per-record enumeration is retained for engineering review; the
 * shared capture below keeps document ownership and pane filtering unchanged. */
#if 0
UmiStatus UmiChartDocumentCapture(const UmiChartDrawingRegistry *registry,
    const char *paneId, const UmiChartNavigation *navigation, UmiChartDocument **outDocument)
{
    if (outDocument == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outDocument = NULL;
    if (registry == NULL || !ValidPane(paneId) || !ValidNavigation(navigation)) return UMI_STATUS_INVALID_ARGUMENT;
    size_t count = umi_chart_drawing_registry_count(registry);
    if (count > UMI_CHART_DRAWING_CAPACITY) return UMI_STATUS_INVALID_STATE;
    UmiChartDocument *document = calloc(1U, sizeof(*document));
    if (document == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(document->summary.pane_id, paneId, strlen(paneId) + 1U);
    document->summary.navigation = *navigation;
    document->summary.source_revision = umi_chart_drawing_registry_revision(registry);
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; i < count; ++i) {
        UmiChartDrawingSnapshot drawing;
        status = umi_chart_drawing_registry_at(registry, i, &drawing);
        if (status != UMI_STATUS_OK) break;
        if (strcmp(drawing.pane_id, paneId) == 0) document->drawings[document->summary.drawing_count++] = drawing;
    }
    if (status == UMI_STATUS_OK) status = UmiChartDocumentValidateOwned(document);
    if (status != UMI_STATUS_OK) { free(document); return status; }
    *outDocument = document; return UMI_STATUS_OK;
}
#endif
UmiStatus UmiChartDocumentCapture(const UmiChartDrawingRegistry *registry,
    const char *paneId, const UmiChartNavigation *navigation, UmiChartDocument **outDocument)
{
    if (outDocument == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outDocument = NULL;
    if (registry == NULL || !ValidPane(paneId) || !ValidNavigation(navigation))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiChartDocument *document = calloc(1U, sizeof(*document));
    if (document == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    /* Obtain records and their observation token through the registry owner.
     * The document owns this copy, so filtering it cannot change live drawings
     * or a neighbouring chart. Additional capture consumers can use the same
     * public operation without reaching into the registry's private storage. */
    UmiSnapshotCapture capture;
    UmiStatus status = umi_chart_drawing_registry_capture(registry,
        document->drawings, UMI_CHART_DRAWING_CAPACITY, &capture);
    if (status != UMI_STATUS_OK) { free(document); return status; }
    memcpy(document->summary.pane_id, paneId, strlen(paneId) + 1U);
    document->summary.navigation = *navigation;
    document->summary.source_revision = capture.revision;
    for (size_t index = 0U; index < capture.count; ++index) {
        if (strcmp(document->drawings[index].pane_id, paneId) == 0) {
            size_t retained = document->summary.drawing_count++;
            document->drawings[retained] = document->drawings[index];
        }
    }
    status = UmiChartDocumentValidateOwned(document);
    if (status != UMI_STATUS_OK) { free(document); return status; }
    *outDocument = document;
    return UMI_STATUS_OK;
}

void UmiChartDocumentDestroy(UmiChartDocument *document) { free(document); }
UmiStatus UmiChartDocumentGetSummary(const UmiChartDocument *document, UmiChartDocumentSummary *outSummary)
{
    if (document == NULL || outSummary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outSummary = document->summary; return UMI_STATUS_OK;
}
UmiStatus UmiChartDocumentDrawingAt(const UmiChartDocument *document, size_t index, UmiChartDrawingSnapshot *outDrawing)
{
    if (document == NULL || outDrawing == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= document->summary.drawing_count) return UMI_STATUS_NOT_FOUND;
    *outDrawing = document->drawings[index]; return UMI_STATUS_OK;
}
UmiStatus UmiChartDocumentApplyDrawings(const UmiChartDocument *document,
    UmiChartDrawingRegistry *registry, uint64_t expectedRevision)
{
    if (document == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return UmiChartDrawingRegistryReplacePane(registry, document->summary.pane_id,
        document->drawings, document->summary.drawing_count, expectedRevision);
}
