/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/scale.h
 *
 * PURPOSE:
 *   Define independent price/value scales for multi-pane financial and scientific charts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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

/**
 * Represent the chart scale snapshot data shared with callers of this public contract.
 */
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

/**
 * Represent the chart scale registry data shared with callers of this public contract.
 */
typedef struct UmiChartScaleRegistry UmiChartScaleRegistry;

/**
 * Initialise chart scale registry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_chart_scale_registry_create(UmiChartScaleRegistry **out_registry);
/**
 * Release or reset state held by chart scale registry so the same storage can be reused
 * safely.
 */
void umi_chart_scale_registry_destroy(UmiChartScaleRegistry *registry);
/**
 * Provide the chart scale registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_scale_registry_upsert(UmiChartScaleRegistry *registry, const UmiChartScaleSnapshot *item);
/**
 * Remove chart scale registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_chart_scale_registry_remove(UmiChartScaleRegistry *registry, const char *id);
/**
 * Find chart scale registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_chart_scale_registry_find(const UmiChartScaleRegistry *registry, const char *id, UmiChartScaleSnapshot *out_item);
/**
 * Find chart scale registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_chart_scale_registry_at(const UmiChartScaleRegistry *registry, size_t index, UmiChartScaleSnapshot *out_item);
/**
 * Return the number of records represented by chart scale registry without changing their
 * state.
 */
size_t umi_chart_scale_registry_count(const UmiChartScaleRegistry *registry);
/**
 * Provide the chart scale registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_chart_scale_registry_revision(const UmiChartScaleRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
