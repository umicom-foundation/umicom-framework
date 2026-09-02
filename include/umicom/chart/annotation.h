/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/annotation.h
 *
 * PURPOSE:
 *   Define user and system annotations without coupling chart data to drawing widgets.
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
#ifndef UMICOM_CHART_ANNOTATION_H
#define UMICOM_CHART_ANNOTATION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CHART_ANNOTATION_CAPACITY 4096U

/**
 * Represent the chart annotation snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiChartAnnotationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char pane_id[128];
    char kind[64];
    int64_t time1;
    int64_t time2;
    double value1;
    double value2;
    char text[512];
    int locked;
    int visible;
    uint64_t revision;
} UmiChartAnnotationSnapshot;

/**
 * Represent the chart annotation registry data shared with callers of this public
 * contract.
 */
typedef struct UmiChartAnnotationRegistry UmiChartAnnotationRegistry;

/**
 * Initialise chart annotation registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_chart_annotation_registry_create(UmiChartAnnotationRegistry **out_registry);
/**
 * Release or reset state held by chart annotation registry so the same storage can be
 * reused safely.
 */
void umi_chart_annotation_registry_destroy(UmiChartAnnotationRegistry *registry);
/**
 * Provide the chart annotation registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_chart_annotation_registry_upsert(UmiChartAnnotationRegistry *registry, const UmiChartAnnotationSnapshot *item);
/**
 * Remove chart annotation registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_chart_annotation_registry_remove(UmiChartAnnotationRegistry *registry, const char *id);
/**
 * Find chart annotation registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_chart_annotation_registry_find(const UmiChartAnnotationRegistry *registry, const char *id, UmiChartAnnotationSnapshot *out_item);
/**
 * Find chart annotation registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_chart_annotation_registry_at(const UmiChartAnnotationRegistry *registry, size_t index, UmiChartAnnotationSnapshot *out_item);
/**
 * Return the number of records represented by chart annotation registry without changing
 * their state.
 */
size_t umi_chart_annotation_registry_count(const UmiChartAnnotationRegistry *registry);
/**
 * Provide the chart annotation registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_chart_annotation_registry_revision(const UmiChartAnnotationRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
