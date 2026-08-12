/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/workspace.c
 *
 * PURPOSE:
 *   Implement an owned reusable chart workspace combining panes, scales, crosshairs, markers, annotations, drawings, live streams and extension points.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation owns only its documented child models. Product-specific
 * business data and toolkit objects remain outside this reusable Framework layer.
 */
#include "umicom/chart/workspace.h"

#include <stdlib.h>
#include <string.h>

struct UmiChartWorkspace {
    UmiChartPaneRegistry *panes;
    UmiChartScaleRegistry *scales;
    UmiChartCrosshairRegistry *crosshairs;
    UmiChartMarkerRegistry *markers;
    UmiChartAnnotationRegistry *annotations;
    UmiChartDrawingRegistry *drawings;
    UmiChartStreamRegistry *streams;
    UmiChartExtensionRegistry *extensions;
    uint64_t revision;
};

UmiStatus umi_chart_workspace_create(UmiChartWorkspace **out_service)
{
    UmiChartWorkspace *service;
    UmiStatus status = UMI_STATUS_OK;
    if (out_service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_service = NULL;
    service = (UmiChartWorkspace *)calloc(1U, sizeof(*service));
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    service->revision = 1U;
    if (status == UMI_STATUS_OK) status = umi_chart_pane_registry_create(&service->panes);
    if (status == UMI_STATUS_OK) status = umi_chart_scale_registry_create(&service->scales);
    if (status == UMI_STATUS_OK) status = umi_chart_crosshair_registry_create(&service->crosshairs);
    if (status == UMI_STATUS_OK) status = umi_chart_marker_registry_create(&service->markers);
    if (status == UMI_STATUS_OK) status = umi_chart_annotation_registry_create(&service->annotations);
    if (status == UMI_STATUS_OK) status = umi_chart_drawing_registry_create(&service->drawings);
    if (status == UMI_STATUS_OK) status = umi_chart_stream_registry_create(&service->streams);
    if (status == UMI_STATUS_OK) status = umi_chart_extension_registry_create(&service->extensions);
    if (status != UMI_STATUS_OK) {
        umi_chart_workspace_destroy(service);
        return status;
    }
    *out_service = service;
    return UMI_STATUS_OK;
}

void umi_chart_workspace_destroy(UmiChartWorkspace *service)
{
    if (service == NULL) return;
    umi_chart_extension_registry_destroy(service->extensions);
    umi_chart_stream_registry_destroy(service->streams);
    umi_chart_drawing_registry_destroy(service->drawings);
    umi_chart_annotation_registry_destroy(service->annotations);
    umi_chart_marker_registry_destroy(service->markers);
    umi_chart_crosshair_registry_destroy(service->crosshairs);
    umi_chart_scale_registry_destroy(service->scales);
    umi_chart_pane_registry_destroy(service->panes);
    free(service);
}

UmiStatus umi_chart_workspace_snapshot(const UmiChartWorkspace *service, UmiChartWorkspaceSnapshot *out_snapshot)
{
    if (service == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = 1U;
    out_snapshot->panes_count = umi_chart_pane_registry_count(service->panes);
    out_snapshot->scales_count = umi_chart_scale_registry_count(service->scales);
    out_snapshot->crosshairs_count = umi_chart_crosshair_registry_count(service->crosshairs);
    out_snapshot->markers_count = umi_chart_marker_registry_count(service->markers);
    out_snapshot->annotations_count = umi_chart_annotation_registry_count(service->annotations);
    out_snapshot->drawings_count = umi_chart_drawing_registry_count(service->drawings);
    out_snapshot->streams_count = umi_chart_stream_registry_count(service->streams);
    out_snapshot->extensions_count = umi_chart_extension_registry_count(service->extensions);
    out_snapshot->revision = service->revision;
    return UMI_STATUS_OK;
}

UmiChartPaneRegistry *umi_chart_workspace_panes(UmiChartWorkspace *service)
{
    return service != NULL ? service->panes : NULL;
}

UmiChartScaleRegistry *umi_chart_workspace_scales(UmiChartWorkspace *service)
{
    return service != NULL ? service->scales : NULL;
}

UmiChartCrosshairRegistry *umi_chart_workspace_crosshairs(UmiChartWorkspace *service)
{
    return service != NULL ? service->crosshairs : NULL;
}

UmiChartMarkerRegistry *umi_chart_workspace_markers(UmiChartWorkspace *service)
{
    return service != NULL ? service->markers : NULL;
}

UmiChartAnnotationRegistry *umi_chart_workspace_annotations(UmiChartWorkspace *service)
{
    return service != NULL ? service->annotations : NULL;
}

UmiChartDrawingRegistry *umi_chart_workspace_drawings(UmiChartWorkspace *service)
{
    return service != NULL ? service->drawings : NULL;
}

UmiChartStreamRegistry *umi_chart_workspace_streams(UmiChartWorkspace *service)
{
    return service != NULL ? service->streams : NULL;
}

UmiChartExtensionRegistry *umi_chart_workspace_extensions(UmiChartWorkspace *service)
{
    return service != NULL ? service->extensions : NULL;
}
