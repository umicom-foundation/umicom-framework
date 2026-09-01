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

typedef struct UmiChartPaneRegistry UmiChartPaneRegistry;

UmiStatus umi_chart_pane_registry_create(UmiChartPaneRegistry **out_registry);
void umi_chart_pane_registry_destroy(UmiChartPaneRegistry *registry);
UmiStatus umi_chart_pane_registry_upsert(UmiChartPaneRegistry *registry, const UmiChartPaneSnapshot *item);
UmiStatus umi_chart_pane_registry_remove(UmiChartPaneRegistry *registry, const char *id);
UmiStatus umi_chart_pane_registry_find(const UmiChartPaneRegistry *registry, const char *id, UmiChartPaneSnapshot *out_item);
UmiStatus umi_chart_pane_registry_at(const UmiChartPaneRegistry *registry, size_t index, UmiChartPaneSnapshot *out_item);
size_t umi_chart_pane_registry_count(const UmiChartPaneRegistry *registry);
uint64_t umi_chart_pane_registry_revision(const UmiChartPaneRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
