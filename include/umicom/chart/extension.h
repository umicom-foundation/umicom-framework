/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/extension.h
 *
 * PURPOSE:
 *   Define chart extension points for indicators, series renderers, overlays and tools.
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
#ifndef UMICOM_CHART_EXTENSION_H
#define UMICOM_CHART_EXTENSION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CHART_EXTENSION_CAPACITY 512U

typedef struct UmiChartExtensionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char name[256];
    char kind[64];
    char provider_id[128];
    char entry_point[256];
    int enabled;
    int trusted;
    int32_t order;
    uint64_t revision;
} UmiChartExtensionSnapshot;

typedef struct UmiChartExtensionRegistry UmiChartExtensionRegistry;

UmiStatus umi_chart_extension_registry_create(UmiChartExtensionRegistry **out_registry);
void umi_chart_extension_registry_destroy(UmiChartExtensionRegistry *registry);
UmiStatus umi_chart_extension_registry_upsert(UmiChartExtensionRegistry *registry, const UmiChartExtensionSnapshot *item);
UmiStatus umi_chart_extension_registry_remove(UmiChartExtensionRegistry *registry, const char *id);
UmiStatus umi_chart_extension_registry_find(const UmiChartExtensionRegistry *registry, const char *id, UmiChartExtensionSnapshot *out_item);
UmiStatus umi_chart_extension_registry_at(const UmiChartExtensionRegistry *registry, size_t index, UmiChartExtensionSnapshot *out_item);
size_t umi_chart_extension_registry_count(const UmiChartExtensionRegistry *registry);
uint64_t umi_chart_extension_registry_revision(const UmiChartExtensionRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
