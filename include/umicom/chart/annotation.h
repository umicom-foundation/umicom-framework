/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/annotation.h
 *
 * PURPOSE:
 *   Define user and system annotations without coupling chart data to drawing widgets.
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
#ifndef UMICOM_CHART_ANNOTATION_H
#define UMICOM_CHART_ANNOTATION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CHART_ANNOTATION_CAPACITY 4096U

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

typedef struct UmiChartAnnotationRegistry UmiChartAnnotationRegistry;

UmiStatus umi_chart_annotation_registry_create(UmiChartAnnotationRegistry **out_registry);
void umi_chart_annotation_registry_destroy(UmiChartAnnotationRegistry *registry);
UmiStatus umi_chart_annotation_registry_upsert(UmiChartAnnotationRegistry *registry, const UmiChartAnnotationSnapshot *item);
UmiStatus umi_chart_annotation_registry_remove(UmiChartAnnotationRegistry *registry, const char *id);
UmiStatus umi_chart_annotation_registry_find(const UmiChartAnnotationRegistry *registry, const char *id, UmiChartAnnotationSnapshot *out_item);
UmiStatus umi_chart_annotation_registry_at(const UmiChartAnnotationRegistry *registry, size_t index, UmiChartAnnotationSnapshot *out_item);
size_t umi_chart_annotation_registry_count(const UmiChartAnnotationRegistry *registry);
uint64_t umi_chart_annotation_registry_revision(const UmiChartAnnotationRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
