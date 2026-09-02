/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_debug_breakpoint_query.c
 *
 * PURPOSE:
 *   Implement the test debug breakpoint query behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_debug_breakpoint_query.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/debug/breakpoint_query.h"

/*
 * Exercise breakpoint and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiDebugBreakpointSnapshot breakpoint(const char *id, const char *uri,
                                              uint32_t line, int verified)
{
    UmiDebugBreakpointSnapshot item = {0};
    (void)strcpy(item.id, id);
    (void)strcpy(item.session_id, "session-1");
    (void)strcpy(item.uri, uri);
    item.line = line;
    item.enabled = 1;
    item.verified = verified;
    return item;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDebugBreakpointRegistry *registry = NULL;
    UmiDebugBreakpointQuery *query = NULL;
    UmiDebugBreakpointFilter filter;
    UmiDebugBreakpointSnapshot first =
        breakpoint("b-1", "file:///src/z.c", 9U, 0);
    UmiDebugBreakpointSnapshot second =
        breakpoint("b-2", "file:///src/a.c", 3U, 1);
    UmiDebugBreakpointSnapshot result;
    UmiDebugBreakpointQuerySnapshot snapshot;
    (void)strcpy(second.condition, "index > 3");

    assert(umi_debug_breakpoint_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_debug_breakpoint_query_create(&query) == UMI_STATUS_OK);
    assert(umi_debug_breakpoint_registry_upsert(registry, &first) ==
           UMI_STATUS_OK);
    assert(umi_debug_breakpoint_registry_upsert(registry, &second) ==
           UMI_STATUS_OK);
    umi_debug_breakpoint_filter_init(&filter);
    (void)strcpy(filter.session_id, "session-1");
    assert(umi_debug_breakpoint_query_execute(query, registry, &filter) ==
           UMI_STATUS_OK);
    assert(umi_debug_breakpoint_query_at(query, 0U, &result) == UMI_STATUS_OK);
    assert(strcmp(result.id, "b-2") == 0);
    assert(umi_debug_breakpoint_query_snapshot(query, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.result_count == 2U);
    assert(snapshot.verified_count == 1U);
    assert(snapshot.conditional_count == 1U);
    filter.verified = 1;
    assert(umi_debug_breakpoint_query_execute(query, registry, &filter) ==
           UMI_STATUS_OK);
    assert(umi_debug_breakpoint_query_count(query) == 1U);
    umi_debug_breakpoint_query_destroy(query);
    umi_debug_breakpoint_registry_destroy(registry);
    return 0;
}
