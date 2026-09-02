/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/stream.h
 *
 * PURPOSE:
 *   Define live streaming state and counters for high-frequency chart updates.
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
#ifndef UMICOM_CHART_STREAM_H
#define UMICOM_CHART_STREAM_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CHART_STREAM_CAPACITY 512U

/**
 * Represent the chart stream snapshot data shared with callers of this public contract.
 */
typedef struct UmiChartStreamSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char series_id[128];
    uint64_t updates;
    uint64_t dropped;
    int64_t last_time;
    double last_value;
    int connected;
    int paused;
    uint64_t revision;
} UmiChartStreamSnapshot;

/**
 * Represent the chart stream registry data shared with callers of this public contract.
 */
typedef struct UmiChartStreamRegistry UmiChartStreamRegistry;

/**
 * Initialise chart stream registry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_chart_stream_registry_create(UmiChartStreamRegistry **out_registry);
/**
 * Release or reset state held by chart stream registry so the same storage can be reused
 * safely.
 */
void umi_chart_stream_registry_destroy(UmiChartStreamRegistry *registry);
/**
 * Provide the chart stream registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_stream_registry_upsert(UmiChartStreamRegistry *registry, const UmiChartStreamSnapshot *item);
/**
 * Remove chart stream registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_chart_stream_registry_remove(UmiChartStreamRegistry *registry, const char *id);
/**
 * Find chart stream registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_chart_stream_registry_find(const UmiChartStreamRegistry *registry, const char *id, UmiChartStreamSnapshot *out_item);
/**
 * Find chart stream registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_chart_stream_registry_at(const UmiChartStreamRegistry *registry, size_t index, UmiChartStreamSnapshot *out_item);
/**
 * Provide the chart stream registry record operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_stream_registry_record(UmiChartStreamRegistry *registry,
                                             const char *id,
                                             int64_t time,
                                             double value,
                                             int dropped_update);
/**
 * Provide the chart stream registry set state operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_stream_registry_set_state(UmiChartStreamRegistry *registry,
                                              const char *id,
                                              int connected,
                                              int paused);
/**
 * Return the number of records represented by chart stream registry without changing their
 * state.
 */
size_t umi_chart_stream_registry_count(const UmiChartStreamRegistry *registry);
/**
 * Provide the chart stream registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_chart_stream_registry_revision(const UmiChartStreamRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
