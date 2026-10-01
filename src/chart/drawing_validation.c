/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/drawing_validation.c
 * PURPOSE: Share archive geometry validation without changing legacy registry normalisation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/drawing_validation.h"
#include <math.h>
UmiStatus UmiChartDrawingValidateGeometry(const UmiChartDrawingSnapshot *drawing)
{
    UmiStatus status = umi_chart_drawing_snapshot_validate(drawing, NULL);
    if (status != UMI_STATUS_OK) return status;
    if (drawing->pane_id[0] == '\0' || drawing->tool[0] == '\0' ||
        !isfinite(drawing->value1) || !isfinite(drawing->value2) ||
        (drawing->selected != 0 && drawing->selected != 1) ||
        (drawing->locked != 0 && drawing->locked != 1)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Unknown flags cannot be silently lost by a saved-chart round trip. */
    if ((drawing->visibility_flags & ~UMI_CHART_DRAWING_VISIBILITY_HIDDEN) != 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
