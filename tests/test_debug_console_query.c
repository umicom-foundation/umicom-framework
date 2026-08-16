/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_debug_console_query.c
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/debug/console_query.h"

static UmiDebugConsoleEntrySnapshot entry(const char *id, const char *text,
                                          uint64_t timestamp, int severity)
{
    UmiDebugConsoleEntrySnapshot item = {0};
    (void)strcpy(item.id, id);
    (void)strcpy(item.session_id, "session-1");
    (void)strcpy(item.category, "stderr");
    (void)strcpy(item.text, text);
    item.timestamp = timestamp;
    item.severity = severity;
    return item;
}

int main(void)
{
    UmiDebugConsoleEntryRegistry *registry = NULL;
    UmiDebugConsoleQuery *query = NULL;
    UmiDebugConsoleFilter filter;
    UmiDebugConsoleEntrySnapshot first = entry("c-1", "warning", 10U, 2);
    UmiDebugConsoleEntrySnapshot second = entry("c-2", "fatal error", 20U, 4);
    UmiDebugConsoleEntrySnapshot result;
    UmiDebugConsoleQuerySnapshot snapshot;

    assert(umi_debug_console_entry_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_debug_console_query_create(&query) == UMI_STATUS_OK);
    assert(umi_debug_console_entry_registry_upsert(registry, &first) ==
           UMI_STATUS_OK);
    assert(umi_debug_console_entry_registry_upsert(registry, &second) ==
           UMI_STATUS_OK);
    umi_debug_console_filter_init(&filter);
    filter.sort = UMI_DEBUG_CONSOLE_SORT_NEWEST_FIRST;
    filter.limit = 1U;
    assert(umi_debug_console_query_execute(query, registry, &filter) ==
           UMI_STATUS_OK);
    assert(umi_debug_console_query_at(query, 0U, &result) == UMI_STATUS_OK);
    assert(strcmp(result.id, "c-2") == 0);
    assert(umi_debug_console_query_snapshot(query, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.result_count == 1U);
    assert(snapshot.total_match_count == 2U);
    assert(snapshot.error_count == 1U);
    assert(snapshot.truncated);
    umi_debug_console_query_destroy(query);
    umi_debug_console_entry_registry_destroy(registry);
    return 0;
}
