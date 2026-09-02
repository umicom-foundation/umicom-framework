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

/*
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

/**
 * Represent the chart crosshair snapshot data shared with callers of this public contract.
 */
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

/**
 * Represent the chart crosshair registry data shared with callers of this public contract.
 */
typedef struct UmiChartCrosshairRegistry UmiChartCrosshairRegistry;

/**
 * Initialise chart crosshair registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_chart_crosshair_registry_create(UmiChartCrosshairRegistry **out_registry);
/**
 * Release or reset state held by chart crosshair registry so the same storage can be
 * reused safely.
 */
void umi_chart_crosshair_registry_destroy(UmiChartCrosshairRegistry *registry);
/**
 * Provide the chart crosshair registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_crosshair_registry_upsert(UmiChartCrosshairRegistry *registry, const UmiChartCrosshairSnapshot *item);
/**
 * Remove chart crosshair registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_chart_crosshair_registry_remove(UmiChartCrosshairRegistry *registry, const char *id);
/**
 * Find chart crosshair registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_chart_crosshair_registry_find(const UmiChartCrosshairRegistry *registry, const char *id, UmiChartCrosshairSnapshot *out_item);
/**
 * Find chart crosshair registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_chart_crosshair_registry_at(const UmiChartCrosshairRegistry *registry, size_t index, UmiChartCrosshairSnapshot *out_item);
/**
 * Return the number of records represented by chart crosshair registry without changing
 * their state.
 */
size_t umi_chart_crosshair_registry_count(const UmiChartCrosshairRegistry *registry);
/**
 * Provide the chart crosshair registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_chart_crosshair_registry_revision(const UmiChartCrosshairRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
