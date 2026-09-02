/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/watch_query.h
 *
 * PURPOSE:
 *   Filter and order watch expressions while keeping evaluation state in the
 *   canonical Framework watch registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WATCH_QUERY_H
#define UMICOM_DEBUG_WATCH_QUERY_H

#include "umicom/debug/watch.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_WATCH_QUERY_API_VERSION 1U

/**
 * List the named debug watch sort values accepted by this public contract.
 */
typedef enum UmiDebugWatchSort {
    UMI_DEBUG_WATCH_SORT_REGISTRY_ORDER = 1,
    UMI_DEBUG_WATCH_SORT_EXPRESSION_ASCENDING = 2,
    UMI_DEBUG_WATCH_SORT_INVALID_FIRST = 3
} UmiDebugWatchSort;

/**
 * Represent the debug watch filter data shared with callers of this public contract.
 */
typedef struct UmiDebugWatchFilter {
    uint32_t struct_size;
    uint32_t api_version;
    char session_id[128];
    char text[512];
    UmiDebugWatchSort sort;
    int enabled;
    int valid;
} UmiDebugWatchFilter;

/**
 * Represent the debug watch query snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugWatchQuerySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t result_count;
    size_t enabled_count;
    size_t valid_count;
    size_t invalid_count;
    uint64_t source_revision;
    uint64_t revision;
} UmiDebugWatchQuerySnapshot;

/**
 * Represent the debug watch query data shared with callers of this public contract.
 */
typedef struct UmiDebugWatchQuery UmiDebugWatchQuery;

/**
 * Initialise debug watch query from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_debug_watch_query_create(UmiDebugWatchQuery **out_query);
/**
 * Release or reset state held by debug watch query so the same storage can be reused
 * safely.
 */
void umi_debug_watch_query_destroy(UmiDebugWatchQuery *query);
/**
 * Initialise debug watch filter from caller-provided values so later operations receive a
 * known state.
 */
void umi_debug_watch_filter_init(UmiDebugWatchFilter *filter);
/**
 * Perform debug watch query through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_debug_watch_query_execute(
    UmiDebugWatchQuery *query,
    const UmiDebugWatchRegistry *registry,
    const UmiDebugWatchFilter *filter);
/**
 * Find debug watch query while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_debug_watch_query_at(
    const UmiDebugWatchQuery *query,
    size_t index,
    UmiDebugWatchSnapshot *out_watch);
/**
 * Provide the debug watch query snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_watch_query_snapshot(
    const UmiDebugWatchQuery *query,
    UmiDebugWatchQuerySnapshot *out_snapshot);
/**
 * Return the number of records represented by debug watch query without changing their
 * state.
 */
size_t umi_debug_watch_query_count(const UmiDebugWatchQuery *query);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEBUG_WATCH_QUERY_H */
