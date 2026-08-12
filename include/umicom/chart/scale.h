/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/scale.h
 *
 * PURPOSE:
 *   Define independent price/value scales for multi-pane financial and scientific charts.
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
#ifndef UMICOM_CHART_SCALE_H
#define UMICOM_CHART_SCALE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CHART_SCALE_CAPACITY 256U

typedef struct UmiChartScaleSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char pane_id[128];
    char side[32];
    double minimum;
    double maximum;
    double margin_top;
    double margin_bottom;
    int auto_scale;
    int logarithmic;
    int inverted;
    uint64_t revision;
} UmiChartScaleSnapshot;

typedef struct UmiChartScaleRegistry UmiChartScaleRegistry;

UmiStatus umi_chart_scale_registry_create(UmiChartScaleRegistry **out_registry);
void umi_chart_scale_registry_destroy(UmiChartScaleRegistry *registry);
UmiStatus umi_chart_scale_registry_upsert(UmiChartScaleRegistry *registry, const UmiChartScaleSnapshot *item);
UmiStatus umi_chart_scale_registry_remove(UmiChartScaleRegistry *registry, const char *id);
UmiStatus umi_chart_scale_registry_find(const UmiChartScaleRegistry *registry, const char *id, UmiChartScaleSnapshot *out_item);
UmiStatus umi_chart_scale_registry_at(const UmiChartScaleRegistry *registry, size_t index, UmiChartScaleSnapshot *out_item);
size_t umi_chart_scale_registry_count(const UmiChartScaleRegistry *registry);
uint64_t umi_chart_scale_registry_revision(const UmiChartScaleRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
