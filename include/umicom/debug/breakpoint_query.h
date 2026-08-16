/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/breakpoint_query.h
 *
 * PURPOSE:
 *   Filter and order debugger breakpoints for reusable workbench views.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_BREAKPOINT_QUERY_H
#define UMICOM_DEBUG_BREAKPOINT_QUERY_H

#include "umicom/debug/breakpoint.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_BREAKPOINT_QUERY_API_VERSION 1U

typedef enum UmiDebugBreakpointSort {
    UMI_DEBUG_BREAKPOINT_SORT_SOURCE_ASCENDING = 1,
    UMI_DEBUG_BREAKPOINT_SORT_SOURCE_DESCENDING = 2,
    UMI_DEBUG_BREAKPOINT_SORT_VERIFICATION = 3
} UmiDebugBreakpointSort;

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

typedef struct UmiDebugBreakpointQuery UmiDebugBreakpointQuery;

UmiStatus umi_debug_breakpoint_query_create(
    UmiDebugBreakpointQuery **out_query);
void umi_debug_breakpoint_query_destroy(UmiDebugBreakpointQuery *query);
void umi_debug_breakpoint_filter_init(UmiDebugBreakpointFilter *filter);
UmiStatus umi_debug_breakpoint_query_execute(
    UmiDebugBreakpointQuery *query,
    const UmiDebugBreakpointRegistry *registry,
    const UmiDebugBreakpointFilter *filter);
UmiStatus umi_debug_breakpoint_query_at(
    const UmiDebugBreakpointQuery *query,
    size_t index,
    UmiDebugBreakpointSnapshot *out_breakpoint);
UmiStatus umi_debug_breakpoint_query_snapshot(
    const UmiDebugBreakpointQuery *query,
    UmiDebugBreakpointQuerySnapshot *out_snapshot);
size_t umi_debug_breakpoint_query_count(const UmiDebugBreakpointQuery *query);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEBUG_BREAKPOINT_QUERY_H */
