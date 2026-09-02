/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/drawing.h
 *
 * PURPOSE:
 *   Define persistent drawing-tool geometry for trend lines, ranges and measurement tools.
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
#ifndef UMICOM_CHART_DRAWING_H
#define UMICOM_CHART_DRAWING_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CHART_DRAWING_CAPACITY 4096U

/**
 * Represent the chart drawing snapshot data shared with callers of this public contract.
 */
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

/**
 * Represent the chart drawing registry data shared with callers of this public contract.
 */
typedef struct UmiChartDrawingRegistry UmiChartDrawingRegistry;

/**
 * Initialise chart drawing registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_chart_drawing_registry_create(UmiChartDrawingRegistry **out_registry);
/**
 * Release or reset state held by chart drawing registry so the same storage can be reused
 * safely.
 */
void umi_chart_drawing_registry_destroy(UmiChartDrawingRegistry *registry);
/**
 * Provide the chart drawing registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_drawing_registry_upsert(UmiChartDrawingRegistry *registry, const UmiChartDrawingSnapshot *item);
/**
 * Remove chart drawing registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_chart_drawing_registry_remove(UmiChartDrawingRegistry *registry, const char *id);
/**
 * Find chart drawing registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_chart_drawing_registry_find(const UmiChartDrawingRegistry *registry, const char *id, UmiChartDrawingSnapshot *out_item);
/**
 * Find chart drawing registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_chart_drawing_registry_at(const UmiChartDrawingRegistry *registry, size_t index, UmiChartDrawingSnapshot *out_item);
/**
 * Return the number of records represented by chart drawing registry without changing
 * their state.
 */
size_t umi_chart_drawing_registry_count(const UmiChartDrawingRegistry *registry);
/**
 * Provide the chart drawing registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_chart_drawing_registry_revision(const UmiChartDrawingRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
