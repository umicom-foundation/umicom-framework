/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/drawing.h
 *
 * PURPOSE:
 *   Define persistent drawing-tool geometry for trend lines, ranges and measurement tools.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This contract stores bounded snapshots by value. The registry owns those
 * copies; it does not take ownership of strings or external resources.
 * Coordinate cross-thread mutation at the product/service boundary.
 */
#ifndef UMICOM_CHART_DRAWING_H
#define UMICOM_CHART_DRAWING_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CHART_DRAWING_CAPACITY 4096U

typedef struct UmiChartDrawingSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char pane_id[128];
    char tool[64];
    int64_t time1;
    int64_t time2;
    double value1;
    double value2;
    char style[256];
    int selected;
    int locked;
    uint64_t revision;
} UmiChartDrawingSnapshot;

typedef struct UmiChartDrawingRegistry UmiChartDrawingRegistry;

UmiStatus umi_chart_drawing_registry_create(UmiChartDrawingRegistry **out_registry);
void umi_chart_drawing_registry_destroy(UmiChartDrawingRegistry *registry);
UmiStatus umi_chart_drawing_registry_upsert(UmiChartDrawingRegistry *registry, const UmiChartDrawingSnapshot *item);
UmiStatus umi_chart_drawing_registry_remove(UmiChartDrawingRegistry *registry, const char *id);
UmiStatus umi_chart_drawing_registry_find(const UmiChartDrawingRegistry *registry, const char *id, UmiChartDrawingSnapshot *out_item);
UmiStatus umi_chart_drawing_registry_at(const UmiChartDrawingRegistry *registry, size_t index, UmiChartDrawingSnapshot *out_item);
size_t umi_chart_drawing_registry_count(const UmiChartDrawingRegistry *registry);
uint64_t umi_chart_drawing_registry_revision(const UmiChartDrawingRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
