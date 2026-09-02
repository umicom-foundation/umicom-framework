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

/*
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

/**
 * Represent the chart marker snapshot data shared with callers of this public contract.
 */
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

/**
 * Represent the chart marker registry data shared with callers of this public contract.
 */
typedef struct UmiChartMarkerRegistry UmiChartMarkerRegistry;

/**
 * Initialise chart marker registry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_chart_marker_registry_create(UmiChartMarkerRegistry **out_registry);
/**
 * Release or reset state held by chart marker registry so the same storage can be reused
 * safely.
 */
void umi_chart_marker_registry_destroy(UmiChartMarkerRegistry *registry);
/**
 * Provide the chart marker registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_marker_registry_upsert(UmiChartMarkerRegistry *registry, const UmiChartMarkerSnapshot *item);
/**
 * Remove chart marker registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_chart_marker_registry_remove(UmiChartMarkerRegistry *registry, const char *id);
/**
 * Find chart marker registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_chart_marker_registry_find(const UmiChartMarkerRegistry *registry, const char *id, UmiChartMarkerSnapshot *out_item);
/**
 * Find chart marker registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_chart_marker_registry_at(const UmiChartMarkerRegistry *registry, size_t index, UmiChartMarkerSnapshot *out_item);
/**
 * Return the number of records represented by chart marker registry without changing their
 * state.
 */
size_t umi_chart_marker_registry_count(const UmiChartMarkerRegistry *registry);
/**
 * Provide the chart marker registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_chart_marker_registry_revision(const UmiChartMarkerRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
