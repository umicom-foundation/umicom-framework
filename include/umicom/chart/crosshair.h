/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/crosshair.h
 *
 * PURPOSE:
 *   Define synchronised crosshair state shared across chart panes and series.
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
#ifndef UMICOM_CHART_CROSSHAIR_H
#define UMICOM_CHART_CROSSHAIR_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CHART_CROSSHAIR_CAPACITY 64U

typedef struct UmiChartCrosshairSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char pane_id[128];
    int64_t time;
    double value;
    int visible;
    int magnet;
    int show_labels;
    uint64_t revision;
} UmiChartCrosshairSnapshot;

typedef struct UmiChartCrosshairRegistry UmiChartCrosshairRegistry;

UmiStatus umi_chart_crosshair_registry_create(UmiChartCrosshairRegistry **out_registry);
void umi_chart_crosshair_registry_destroy(UmiChartCrosshairRegistry *registry);
UmiStatus umi_chart_crosshair_registry_upsert(UmiChartCrosshairRegistry *registry, const UmiChartCrosshairSnapshot *item);
UmiStatus umi_chart_crosshair_registry_remove(UmiChartCrosshairRegistry *registry, const char *id);
UmiStatus umi_chart_crosshair_registry_find(const UmiChartCrosshairRegistry *registry, const char *id, UmiChartCrosshairSnapshot *out_item);
UmiStatus umi_chart_crosshair_registry_at(const UmiChartCrosshairRegistry *registry, size_t index, UmiChartCrosshairSnapshot *out_item);
size_t umi_chart_crosshair_registry_count(const UmiChartCrosshairRegistry *registry);
uint64_t umi_chart_crosshair_registry_revision(const UmiChartCrosshairRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
