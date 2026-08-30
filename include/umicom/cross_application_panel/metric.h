/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/metric.h
 *
 * PURPOSE:
 *   Define cross-application panel metric state and bounded storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_METRIC_H
#define UMICOM_CROSS_APPLICATION_PANEL_METRIC_H
#include "umicom/cross_application_panel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPanelMetric {
    uint32_t structure_size;
    char panel_id[UMI_PANEL_TEXT_CAPACITY];
    uint64_t open_count;
    uint64_t close_count;
    uint64_t activation_count;
    uint64_t context_count;
    uint64_t failure_count;
    uint64_t last_active_ms;
    uint64_t revision;
} UmiPanelMetric;
typedef struct UmiPanelMetricStore { UmiPanelMetric items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelMetricStore;
void umi_panel_metric_init(UmiPanelMetric *record);
UmiStatus umi_panel_metric_validate(const UmiPanelMetric *record);
void umi_panel_metric_store_init(UmiPanelMetricStore *store);
UmiStatus umi_panel_metric_store_put(UmiPanelMetricStore *store,const UmiPanelMetric *record);
UmiStatus umi_panel_metric_store_remove(UmiPanelMetricStore *store,const char *identity);
UmiPanelMetric *umi_panel_metric_store_find(UmiPanelMetricStore *store,const char *identity);
const UmiPanelMetric *umi_panel_metric_store_find_const(const UmiPanelMetricStore *store,const char *identity);
UmiStatus umi_panel_metric_store_snapshot(const UmiPanelMetricStore *store,UmiPanelMetric *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
