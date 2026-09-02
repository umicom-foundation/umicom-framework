/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/extension.h
 *
 * PURPOSE:
 *   Define chart extension points for indicators, series renderers, overlays and tools.
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
#ifndef UMICOM_CHART_EXTENSION_H
#define UMICOM_CHART_EXTENSION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CHART_EXTENSION_CAPACITY 512U

/**
 * Represent the chart extension snapshot data shared with callers of this public contract.
 */
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

/**
 * Represent the chart extension registry data shared with callers of this public contract.
 */
typedef struct UmiChartExtensionRegistry UmiChartExtensionRegistry;

/**
 * Initialise chart extension registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_chart_extension_registry_create(UmiChartExtensionRegistry **out_registry);
/**
 * Release or reset state held by chart extension registry so the same storage can be
 * reused safely.
 */
void umi_chart_extension_registry_destroy(UmiChartExtensionRegistry *registry);
/**
 * Provide the chart extension registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_extension_registry_upsert(UmiChartExtensionRegistry *registry, const UmiChartExtensionSnapshot *item);
/**
 * Remove chart extension registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_chart_extension_registry_remove(UmiChartExtensionRegistry *registry, const char *id);
/**
 * Find chart extension registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_chart_extension_registry_find(const UmiChartExtensionRegistry *registry, const char *id, UmiChartExtensionSnapshot *out_item);
/**
 * Find chart extension registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_chart_extension_registry_at(const UmiChartExtensionRegistry *registry, size_t index, UmiChartExtensionSnapshot *out_item);
/**
 * Return the number of records represented by chart extension registry without changing
 * their state.
 */
size_t umi_chart_extension_registry_count(const UmiChartExtensionRegistry *registry);
/**
 * Provide the chart extension registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_chart_extension_registry_revision(const UmiChartExtensionRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
