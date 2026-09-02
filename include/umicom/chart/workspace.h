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

/*
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

/**
 * Represent the chart workspace data shared with callers of this public contract.
 */
typedef struct UmiChartWorkspace UmiChartWorkspace;

/**
 * Represent the chart workspace snapshot data shared with callers of this public contract.
 */
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

/**
 * Initialise chart workspace from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_chart_workspace_create(UmiChartWorkspace **out_service);
/**
 * Release or reset state held by chart workspace so the same storage can be reused safely.
 */
void umi_chart_workspace_destroy(UmiChartWorkspace *service);
/**
 * Provide the chart workspace snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_workspace_snapshot(const UmiChartWorkspace *service, UmiChartWorkspaceSnapshot *out_snapshot);
/**
 * Provide the chart workspace panes operation used by this module and its client
 * applications.
 */
UmiChartPaneRegistry *umi_chart_workspace_panes(UmiChartWorkspace *service);
/**
 * Provide the chart workspace scales operation used by this module and its client
 * applications.
 */
UmiChartScaleRegistry *umi_chart_workspace_scales(UmiChartWorkspace *service);
/**
 * Provide the chart workspace crosshairs operation used by this module and its client
 * applications.
 */
UmiChartCrosshairRegistry *umi_chart_workspace_crosshairs(UmiChartWorkspace *service);
/**
 * Provide the chart workspace markers operation used by this module and its client
 * applications.
 */
UmiChartMarkerRegistry *umi_chart_workspace_markers(UmiChartWorkspace *service);
/**
 * Provide the chart workspace annotations operation used by this module and its client
 * applications.
 */
UmiChartAnnotationRegistry *umi_chart_workspace_annotations(UmiChartWorkspace *service);
/**
 * Provide the chart workspace drawings operation used by this module and its client
 * applications.
 */
UmiChartDrawingRegistry *umi_chart_workspace_drawings(UmiChartWorkspace *service);
/**
 * Provide the chart workspace streams operation used by this module and its client
 * applications.
 */
UmiChartStreamRegistry *umi_chart_workspace_streams(UmiChartWorkspace *service);
/**
 * Provide the chart workspace extensions operation used by this module and its client
 * applications.
 */
UmiChartExtensionRegistry *umi_chart_workspace_extensions(UmiChartWorkspace *service);

#ifdef __cplusplus
}
#endif
#endif
