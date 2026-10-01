/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/drawing_visibility.c
 * PURPOSE: Publish explicit visibility changes through the existing drawing registry.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/drawing_visibility.h"
#include "umicom/chart/drawing_validation.h"
#include <stdlib.h>
#include <string.h>

/* Fixed identity widths match the registry and checkpoint contracts. */
static int VisibilityIdentityValid(const char *text)
{
    if (text == NULL || text[0] == '\0') return 0;
    for (size_t i = 1U; i < 128U; ++i) if (text[i] == '\0') return 1;
    return 0;
}

/* Visibility is metadata on the canonical row; a second UI-only registry
 * would lose the state on chart restore and could drift from drawing IDs. */
UmiStatus UmiChartDrawingSetHidden(UmiChartDrawingRegistry *registry, const char *pane,
    const char *id, uint64_t expectedRevision, int hidden)
{
    if (registry == NULL || !VisibilityIdentityValid(pane) || !VisibilityIdentityValid(id) ||
        (hidden != 0 && hidden != 1)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiChartDrawingSnapshot drawing;
    UmiStatus status = umi_chart_drawing_registry_find(registry, id, &drawing);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(drawing.pane_id, pane) != 0) return UMI_STATUS_INVALID_STATE;
    if (drawing.revision != expectedRevision) return UMI_STATUS_BUSY;
    status = UmiChartDrawingValidateGeometry(&drawing);
    if (status != UMI_STATUS_OK) return status;
    uint64_t flags = hidden ? UMI_CHART_DRAWING_VISIBILITY_HIDDEN : 0U;
    if (drawing.visibility_flags == flags) return UMI_STATUS_OK;
    drawing.visibility_flags = flags;
    return umi_chart_drawing_registry_upsert(registry, &drawing);
}

/* Count and validate before allocating. The registry's existing batch owner
 * performs one publication, so a capacity failure cannot leave half a pane hidden. */
UmiStatus UmiChartDrawingSetPaneHidden(UmiChartDrawingRegistry *registry, const char *pane,
    uint64_t expectedRegistryRevision, int hidden, size_t *outChanged)
{
    if (registry == NULL || !VisibilityIdentityValid(pane) || (hidden != 0 && hidden != 1))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_chart_drawing_registry_revision(registry) != expectedRegistryRevision) return UMI_STATUS_BUSY;
    size_t count = umi_chart_drawing_registry_count(registry), changed = 0U;
    if (count > UMI_CHART_DRAWING_CAPACITY) return UMI_STATUS_INVALID_STATE;
    uint64_t flags = hidden ? UMI_CHART_DRAWING_VISIBILITY_HIDDEN : 0U;
    for (size_t i = 0U; i < count; ++i) {
        UmiChartDrawingSnapshot drawing;
        UmiStatus status = umi_chart_drawing_registry_at(registry, i, &drawing);
        if (status != UMI_STATUS_OK) return status;
        if (strcmp(drawing.pane_id, pane) != 0) continue;
        status = UmiChartDrawingValidateGeometry(&drawing);
        if (status != UMI_STATUS_OK) return status;
        if (drawing.visibility_flags != flags) ++changed;
    }
    if (changed == 0U) { if (outChanged != NULL) *outChanged = 0U; return UMI_STATUS_OK; }
    UmiChartDrawingSnapshot *batch = calloc(changed, sizeof *batch);
    if (batch == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    size_t used = 0U; UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; i < count && status == UMI_STATUS_OK; ++i) {
        UmiChartDrawingSnapshot drawing;
        status = umi_chart_drawing_registry_at(registry, i, &drawing);
        if (status == UMI_STATUS_OK && strcmp(drawing.pane_id, pane) == 0 && drawing.visibility_flags != flags) {
            drawing.visibility_flags = flags;
            if (used >= changed) { status = UMI_STATUS_INVALID_STATE; break; }
            batch[used++] = drawing;
        }
    }
    if (status == UMI_STATUS_OK && used != changed) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) status = umi_chart_drawing_registry_upsert_many(registry, batch, used, NULL);
    free(batch);
    if (status == UMI_STATUS_OK && outChanged != NULL) *outChanged = changed;
    return status;
}
