/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/workspace.h
 *
 * PURPOSE:
 *   Define an owned reusable chart workspace combining panes, scales, crosshairs, markers, annotations, drawings, live streams and extension points.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract is toolkit-neutral and intended for reuse by Studio,
 * Trader, TMS and future Umicom applications without exposing GUI objects.
 */
#ifndef UMICOM_CHART_WORKSPACE_H
#define UMICOM_CHART_WORKSPACE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/chart/pane.h"
#include "umicom/chart/scale.h"
#include "umicom/chart/crosshair.h"
#include "umicom/chart/marker.h"
#include "umicom/chart/annotation.h"
#include "umicom/chart/drawing.h"
#include "umicom/chart/stream.h"
#include "umicom/chart/extension.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiChartWorkspace UmiChartWorkspace;

typedef struct UmiChartWorkspaceSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t panes_count;
    size_t scales_count;
    size_t crosshairs_count;
    size_t markers_count;
    size_t annotations_count;
    size_t drawings_count;
    size_t streams_count;
    size_t extensions_count;
    uint64_t revision;
} UmiChartWorkspaceSnapshot;

UmiStatus umi_chart_workspace_create(UmiChartWorkspace **out_service);
void umi_chart_workspace_destroy(UmiChartWorkspace *service);
UmiStatus umi_chart_workspace_snapshot(const UmiChartWorkspace *service, UmiChartWorkspaceSnapshot *out_snapshot);
UmiChartPaneRegistry *umi_chart_workspace_panes(UmiChartWorkspace *service);
UmiChartScaleRegistry *umi_chart_workspace_scales(UmiChartWorkspace *service);
UmiChartCrosshairRegistry *umi_chart_workspace_crosshairs(UmiChartWorkspace *service);
UmiChartMarkerRegistry *umi_chart_workspace_markers(UmiChartWorkspace *service);
UmiChartAnnotationRegistry *umi_chart_workspace_annotations(UmiChartWorkspace *service);
UmiChartDrawingRegistry *umi_chart_workspace_drawings(UmiChartWorkspace *service);
UmiChartStreamRegistry *umi_chart_workspace_streams(UmiChartWorkspace *service);
UmiChartExtensionRegistry *umi_chart_workspace_extensions(UmiChartWorkspace *service);

#ifdef __cplusplus
}
#endif
#endif
