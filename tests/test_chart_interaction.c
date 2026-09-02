/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_chart_interaction.c
 *
 * PURPOSE:
 *   Exercise reusable chart panes, annotations and live-stream state used by
 *   Trader and future financial or telemetry applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <string.h>

#include "umicom/chart/annotation.h"
#include "umicom/chart/pane.h"
#include "umicom/chart/stream.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiChartPaneRegistry *panes = NULL;
    UmiChartPaneSnapshot pane = {0};
    UmiChartAnnotationRegistry *annotations = NULL;
    UmiChartAnnotationSnapshot annotation = {0};
    UmiChartStreamRegistry *streams = NULL;
    UmiChartStreamSnapshot stream = {0};

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_chart_pane_registry_create(&panes) != UMI_STATUS_OK) return 1;
    (void)strcpy(pane.id, "price");
    pane.visible = 1;
    pane.height_weight = 1.0;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_chart_pane_registry_upsert(panes, &pane) != UMI_STATUS_OK) return 2;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_chart_annotation_registry_create(&annotations) != UMI_STATUS_OK) return 3;
    (void)strcpy(annotation.id, "line");
    (void)strcpy(annotation.pane_id, "price");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_chart_annotation_registry_upsert(annotations, &annotation) != UMI_STATUS_OK) return 4;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_chart_stream_registry_create(&streams) != UMI_STATUS_OK) return 5;
    (void)strcpy(stream.id, "live");
    (void)strcpy(stream.series_id, "ES");
    stream.connected = 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_chart_stream_registry_upsert(streams, &stream) != UMI_STATUS_OK) return 6;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_chart_stream_registry_record(streams, "live", 10, 5000.25, 0) != UMI_STATUS_OK) return 7;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_chart_stream_registry_record(streams, "live", 11, 5000.50, 1) != UMI_STATUS_OK) return 8;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_chart_stream_registry_set_state(streams, "live", 1, 1) != UMI_STATUS_OK) return 9;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_chart_stream_registry_find(streams, "live", &stream) != UMI_STATUS_OK ||
        stream.updates != 1U || stream.dropped != 1U || stream.last_time != 10 ||
        stream.last_value != 5000.25 || !stream.paused) return 10;

    umi_chart_stream_registry_destroy(streams);
    umi_chart_annotation_registry_destroy(annotations);
    umi_chart_pane_registry_destroy(panes);
    return 0;
}
