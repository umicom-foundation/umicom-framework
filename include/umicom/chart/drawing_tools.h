/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/drawing_tools.h
 * PURPOSE: Define reusable chart drawing tools and clipped geometry without GUI ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CHART_DRAWING_TOOLS_H
#define UMICOM_CHART_DRAWING_TOOLS_H
#include "umicom/chart/drawing_validation.h"
#include "umicom/chart/plot.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiChartDrawingKind {
    UMI_CHART_DRAWING_TREND=1, UMI_CHART_DRAWING_SUPPORT=2,
    UMI_CHART_DRAWING_RESISTANCE=3, UMI_CHART_DRAWING_RANGE=4,
    UMI_CHART_DRAWING_LIQUIDITY_ZONE=5, UMI_CHART_DRAWING_RAY=6
} UmiChartDrawingKind;
const char *UmiChartDrawingKindName(UmiChartDrawingKind kind);
UmiStatus UmiChartDrawingKindParse(const char *name,UmiChartDrawingKind *outKind);
/** Validate one of the supported tools. Unknown tools return UNAVAILABLE so
 * archives can retain future tools without silently interpreting their shape.
 * Trend/ray anchors need different times; boxes also need different prices.
 * All times are nonnegative milliseconds and prices must be finite. */
UmiStatus UmiChartDrawingToolValidate(const UmiChartDrawingSnapshot *drawing);
/** Create a value-owned drawing. Levels use first.value at both anchors.
 * Other tools retain the order of the two anchors; rays extend from first
 * through second, including a leftward ray. Output is unchanged on failure. */
UmiStatus UmiChartDrawingInitialize(const char *id,const char *pane,
    UmiChartDrawingKind kind,UmiChartPoint first,UmiChartPoint second,
    UmiChartDrawingSnapshot *outDrawing);
typedef struct UmiChartDrawingGeometry {
    UmiChartDrawingKind kind;
    int visible;
    UmiChartRenderPoint first,last;
    UmiChartRenderRectangle rectangle;
} UmiChartDrawingGeometry;
/** Clip in data coordinates before projection. Offscreen drawings return OK
 * with visible=0. Nothing is extrapolated into market history. A box containing
 * a single-timestamp viewport spans its width. Invalid input leaves output
 * unchanged. Geometry is a copy and does not retain either input. */
/* Hidden supported tools also return OK with visible=0 after validation. */
UmiStatus UmiChartDrawingProject(const UmiChartDrawingSnapshot *drawing,
    const UmiChartPlotViewport *viewport,UmiChartDrawingGeometry *outGeometry);
/** Append up to two commands, using the supplied theme. Geometry, style and
 * remaining capacity are checked before publication. A liquidity zone is a
 * translucent user annotation, not evidence of actual market liquidity. */
UmiStatus UmiChartDrawingRender(UmiChartRenderScene *scene,
    const UmiChartDrawingSnapshot *drawing,const UmiChartPlotViewport *viewport,
    const UmiChartPlotStyle *style);
#ifdef __cplusplus
}
#endif
#endif
