/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/console_query.h
 *
 * PURPOSE:
 *   Query debugger console output by session, category, severity and text.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_CONSOLE_QUERY_H
#define UMICOM_DEBUG_CONSOLE_QUERY_H

#include "umicom/debug/console_entry.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_CONSOLE_QUERY_API_VERSION 1U

typedef enum UmiDebugConsoleSort {
    UMI_DEBUG_CONSOLE_SORT_OLDEST_FIRST = 1,
    UMI_DEBUG_CONSOLE_SORT_NEWEST_FIRST = 2,
    UMI_DEBUG_CONSOLE_SORT_SEVERITY_FIRST = 3
} UmiDebugConsoleSort;

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

typedef struct UmiDebugConsoleQuery UmiDebugConsoleQuery;

UmiStatus umi_debug_console_query_create(UmiDebugConsoleQuery **out_query);
void umi_debug_console_query_destroy(UmiDebugConsoleQuery *query);
void umi_debug_console_filter_init(UmiDebugConsoleFilter *filter);
UmiStatus umi_debug_console_query_execute(
    UmiDebugConsoleQuery *query,
    const UmiDebugConsoleEntryRegistry *registry,
    const UmiDebugConsoleFilter *filter);
UmiStatus umi_debug_console_query_at(
    const UmiDebugConsoleQuery *query,
    size_t index,
    UmiDebugConsoleEntrySnapshot *out_entry);
UmiStatus umi_debug_console_query_snapshot(
    const UmiDebugConsoleQuery *query,
    UmiDebugConsoleQuerySnapshot *out_snapshot);
size_t umi_debug_console_query_count(const UmiDebugConsoleQuery *query);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEBUG_CONSOLE_QUERY_H */
