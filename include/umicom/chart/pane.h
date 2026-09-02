/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/pane.h
 *
 * PURPOSE:
 *   Define reusable chart panes so multiple price and indicator regions share one timeline.
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
#ifndef UMICOM_CHART_PANE_H
#define UMICOM_CHART_PANE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CHART_PANE_CAPACITY 128U

/**
 * Represent the chart pane snapshot data shared with callers of this public contract.
 */
typedef struct UmiChartPaneSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char title[256];
    double height_weight;
    int visible;
    int collapsed;
    int32_t order;
    uint64_t revision;
} UmiChartPaneSnapshot;

/**
 * Represent the chart pane registry data shared with callers of this public contract.
 */
typedef struct UmiChartPaneRegistry UmiChartPaneRegistry;

/**
 * Initialise chart pane registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_chart_pane_registry_create(UmiChartPaneRegistry **out_registry);
/**
 * Release or reset state held by chart pane registry so the same storage can be reused
 * safely.
 */
void umi_chart_pane_registry_destroy(UmiChartPaneRegistry *registry);
/**
 * Provide the chart pane registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_pane_registry_upsert(UmiChartPaneRegistry *registry, const UmiChartPaneSnapshot *item);
/**
 * Remove chart pane registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_chart_pane_registry_remove(UmiChartPaneRegistry *registry, const char *id);
/**
 * Find chart pane registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_chart_pane_registry_find(const UmiChartPaneRegistry *registry, const char *id, UmiChartPaneSnapshot *out_item);
/**
 * Find chart pane registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_chart_pane_registry_at(const UmiChartPaneRegistry *registry, size_t index, UmiChartPaneSnapshot *out_item);
/**
 * Return the number of records represented by chart pane registry without changing their
 * state.
 */
size_t umi_chart_pane_registry_count(const UmiChartPaneRegistry *registry);
/**
 * Provide the chart pane registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_chart_pane_registry_revision(const UmiChartPaneRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
