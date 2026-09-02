/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/console_query.h
 *
 * PURPOSE:
 *   Query debugger console output by session, category, severity and text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_CONSOLE_QUERY_H
#define UMICOM_DEBUG_CONSOLE_QUERY_H

#include "umicom/debug/console_entry.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_CONSOLE_QUERY_API_VERSION 1U

/**
 * List the named debug console sort values accepted by this public contract.
 */
typedef enum UmiDebugConsoleSort {
    UMI_DEBUG_CONSOLE_SORT_OLDEST_FIRST = 1,
    UMI_DEBUG_CONSOLE_SORT_NEWEST_FIRST = 2,
    UMI_DEBUG_CONSOLE_SORT_SEVERITY_FIRST = 3
} UmiDebugConsoleSort;

/**
 * Represent the debug console filter data shared with callers of this public contract.
 */
typedef struct UmiDebugConsoleFilter {
    uint32_t struct_size;
    uint32_t api_version;
    char session_id[128];
    char category[64];
    char text[512];
    UmiDebugConsoleSort sort;
    int minimum_severity;
    size_t limit;
} UmiDebugConsoleFilter;

/**
 * Represent the debug console query snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugConsoleQuerySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t result_count;
    size_t total_match_count;
    size_t error_count;
    size_t warning_count;
    uint64_t source_revision;
    uint64_t revision;
    int truncated;
} UmiDebugConsoleQuerySnapshot;

/**
 * Represent the debug console query data shared with callers of this public contract.
 */
typedef struct UmiDebugConsoleQuery UmiDebugConsoleQuery;

/**
 * Initialise debug console query from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_debug_console_query_create(UmiDebugConsoleQuery **out_query);
/**
 * Release or reset state held by debug console query so the same storage can be reused
 * safely.
 */
void umi_debug_console_query_destroy(UmiDebugConsoleQuery *query);
/**
 * Initialise debug console filter from caller-provided values so later operations receive
 * a known state.
 */
void umi_debug_console_filter_init(UmiDebugConsoleFilter *filter);
/**
 * Perform debug console query through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_debug_console_query_execute(
    UmiDebugConsoleQuery *query,
    const UmiDebugConsoleEntryRegistry *registry,
    const UmiDebugConsoleFilter *filter);
/**
 * Find debug console query while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_debug_console_query_at(
    const UmiDebugConsoleQuery *query,
    size_t index,
    UmiDebugConsoleEntrySnapshot *out_entry);
/**
 * Provide the debug console query snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_console_query_snapshot(
    const UmiDebugConsoleQuery *query,
    UmiDebugConsoleQuerySnapshot *out_snapshot);
/**
 * Return the number of records represented by debug console query without changing their
 * state.
 */
size_t umi_debug_console_query_count(const UmiDebugConsoleQuery *query);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEBUG_CONSOLE_QUERY_H */
