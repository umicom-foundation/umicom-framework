/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/breakpoint_query.h
 *
 * PURPOSE:
 *   Filter and order debugger breakpoints for reusable workbench views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_BREAKPOINT_QUERY_H
#define UMICOM_DEBUG_BREAKPOINT_QUERY_H

#include "umicom/debug/breakpoint.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_BREAKPOINT_QUERY_API_VERSION 1U

/**
 * List the named debug breakpoint sort values accepted by this public contract.
 */
typedef enum UmiDebugBreakpointSort {
    UMI_DEBUG_BREAKPOINT_SORT_SOURCE_ASCENDING = 1,
    UMI_DEBUG_BREAKPOINT_SORT_SOURCE_DESCENDING = 2,
    UMI_DEBUG_BREAKPOINT_SORT_VERIFICATION = 3
} UmiDebugBreakpointSort;

/**
 * Represent the debug breakpoint filter data shared with callers of this public contract.
 */
typedef struct UmiDebugBreakpointFilter {
    uint32_t struct_size;
    uint32_t api_version;
    char session_id[128];
    char uri[1024];
    char text[512];
    UmiDebugBreakpointSort sort;
    int enabled;
    int verified;
} UmiDebugBreakpointFilter;

/**
 * Represent the debug breakpoint query snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugBreakpointQuerySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t result_count;
    size_t enabled_count;
    size_t verified_count;
    size_t conditional_count;
    size_t logpoint_count;
    uint64_t source_revision;
    uint64_t revision;
} UmiDebugBreakpointQuerySnapshot;

/**
 * Represent the debug breakpoint query data shared with callers of this public contract.
 */
typedef struct UmiDebugBreakpointQuery UmiDebugBreakpointQuery;

/**
 * Initialise debug breakpoint query from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_breakpoint_query_create(
    UmiDebugBreakpointQuery **out_query);
/**
 * Release or reset state held by debug breakpoint query so the same storage can be reused
 * safely.
 */
void umi_debug_breakpoint_query_destroy(UmiDebugBreakpointQuery *query);
/**
 * Initialise debug breakpoint filter from caller-provided values so later operations
 * receive a known state.
 */
void umi_debug_breakpoint_filter_init(UmiDebugBreakpointFilter *filter);
/**
 * Perform debug breakpoint query through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_debug_breakpoint_query_execute(
    UmiDebugBreakpointQuery *query,
    const UmiDebugBreakpointRegistry *registry,
    const UmiDebugBreakpointFilter *filter);
/**
 * Find debug breakpoint query while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_debug_breakpoint_query_at(
    const UmiDebugBreakpointQuery *query,
    size_t index,
    UmiDebugBreakpointSnapshot *out_breakpoint);
/**
 * Provide the debug breakpoint query snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_breakpoint_query_snapshot(
    const UmiDebugBreakpointQuery *query,
    UmiDebugBreakpointQuerySnapshot *out_snapshot);
/**
 * Return the number of records represented by debug breakpoint query without changing
 * their state.
 */
size_t umi_debug_breakpoint_query_count(const UmiDebugBreakpointQuery *query);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEBUG_BREAKPOINT_QUERY_H */
