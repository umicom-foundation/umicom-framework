/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/marker.h
 *
 * PURPOSE:
 *   Define event, signal and trade markers on time-series charts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This contract stores bounded snapshots by value. The registry owns those
 * copies; it does not take ownership of strings or external resources.
 * Coordinate cross-thread mutation at the product/service boundary.
 */
#ifndef UMICOM_CHART_MARKER_H
#define UMICOM_CHART_MARKER_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CHART_MARKER_CAPACITY 4096U

typedef struct UmiChartMarkerSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char series_id[128];
    int64_t time;
    double value;
    char text[256];
    char shape[64];
    char position[64];
    int32_t order;
    uint64_t revision;
} UmiChartMarkerSnapshot;

typedef struct UmiChartMarkerRegistry UmiChartMarkerRegistry;

UmiStatus umi_chart_marker_registry_create(UmiChartMarkerRegistry **out_registry);
void umi_chart_marker_registry_destroy(UmiChartMarkerRegistry *registry);
UmiStatus umi_chart_marker_registry_upsert(UmiChartMarkerRegistry *registry, const UmiChartMarkerSnapshot *item);
UmiStatus umi_chart_marker_registry_remove(UmiChartMarkerRegistry *registry, const char *id);
UmiStatus umi_chart_marker_registry_find(const UmiChartMarkerRegistry *registry, const char *id, UmiChartMarkerSnapshot *out_item);
UmiStatus umi_chart_marker_registry_at(const UmiChartMarkerRegistry *registry, size_t index, UmiChartMarkerSnapshot *out_item);
size_t umi_chart_marker_registry_count(const UmiChartMarkerRegistry *registry);
uint64_t umi_chart_marker_registry_revision(const UmiChartMarkerRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
