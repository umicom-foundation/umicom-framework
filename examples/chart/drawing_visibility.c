/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/chart/drawing_visibility.c
 * PURPOSE: Teach revision-checked drawing visibility using an in-memory registry.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/drawing_tools.h"
#include "umicom/chart/drawing_visibility.h"
#include <stdio.h>

/* Hide without deleting, then show the same ID using its newly read revision. */
int main(void)
{
    UmiChartDrawingRegistry *registry = NULL;
    UmiStatus status = umi_chart_drawing_registry_create(&registry);
    UmiChartDrawingSnapshot drawing = {0};
    if (status == UMI_STATUS_OK)
        status = UmiChartDrawingInitialize("lesson.range", "NQ", UMI_CHART_DRAWING_RANGE,
            (UmiChartPoint){60000, 100}, (UmiChartPoint){120000, 110}, &drawing);
    if (status == UMI_STATUS_OK) status = umi_chart_drawing_registry_upsert(registry, &drawing);
    if (status == UMI_STATUS_OK) status = umi_chart_drawing_registry_find(registry, "lesson.range", &drawing);
    if (status == UMI_STATUS_OK)
        status = UmiChartDrawingSetHidden(registry, "NQ", drawing.id, drawing.revision, 1);
    if (status == UMI_STATUS_OK) status = umi_chart_drawing_registry_find(registry, "lesson.range", &drawing);
    if (status == UMI_STATUS_OK && drawing.visibility_flags != UMI_CHART_DRAWING_VISIBILITY_HIDDEN)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) puts("Hidden: the same drawing and its anchors are retained.");
    /* Mutations advance revisions. Read first; never reuse an old edit token. */
    if (status == UMI_STATUS_OK)
        status = UmiChartDrawingSetHidden(registry, "NQ", drawing.id, drawing.revision, 0);
    if (status == UMI_STATUS_OK) status = umi_chart_drawing_registry_find(registry, "lesson.range", &drawing);
    if (status == UMI_STATUS_OK && (drawing.visibility_flags != 0U || umi_chart_drawing_registry_count(registry) != 1U))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) puts("Shown: one drawing, with the original identity and geometry.");
    else fprintf(stderr, "Drawing visibility failed with status %d.\n", (int)status);
    umi_chart_drawing_registry_destroy(registry);
    return status == UMI_STATUS_OK ? 0 : 1;
}
