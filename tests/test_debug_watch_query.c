/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_debug_watch_query.c
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/debug/watch_query.h"

int main(void)
{
    UmiDebugWatchRegistry *registry = NULL;
    UmiDebugWatchQuery *query = NULL;
    UmiDebugWatchFilter filter;
    UmiDebugWatchSnapshot valid = {0};
    UmiDebugWatchSnapshot invalid = {0};
    UmiDebugWatchSnapshot result;
    UmiDebugWatchQuerySnapshot snapshot;

    (void)strcpy(valid.id, "w-1");
    (void)strcpy(valid.session_id, "session-1");
    (void)strcpy(valid.expression, "customer_count");
    (void)strcpy(valid.value, "12");
    valid.enabled = 1;
    valid.valid = 1;
    invalid = valid;
    (void)strcpy(invalid.id, "w-2");
    (void)strcpy(invalid.expression, "missing_value");
    (void)strcpy(invalid.value, "not available");
    invalid.valid = 0;
    assert(umi_debug_watch_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_debug_watch_query_create(&query) == UMI_STATUS_OK);
    assert(umi_debug_watch_registry_upsert(registry, &valid) == UMI_STATUS_OK);
    assert(umi_debug_watch_registry_upsert(registry, &invalid) == UMI_STATUS_OK);
    umi_debug_watch_filter_init(&filter);
    filter.sort = UMI_DEBUG_WATCH_SORT_INVALID_FIRST;
    assert(umi_debug_watch_query_execute(query, registry, &filter) ==
           UMI_STATUS_OK);
    assert(umi_debug_watch_query_at(query, 0U, &result) == UMI_STATUS_OK);
    assert(strcmp(result.id, "w-2") == 0);
    assert(umi_debug_watch_query_snapshot(query, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.result_count == 2U);
    assert(snapshot.invalid_count == 1U);
    umi_debug_watch_query_destroy(query);
    umi_debug_watch_registry_destroy(registry);
    return 0;
}
