/*-----------------------------------------------------------------------------
 * Umicom Framework advanced breakpoint registry tests.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/debug/advanced_breakpoint.h"

static UmiDebugAdvancedBreakpoint make_breakpoint(
    const char *id,
    UmiDebugAdvancedBreakpointKind kind)
{
    UmiDebugAdvancedBreakpoint item;
    (void)memset(&item, 0, sizeof(item));
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_DEBUG_ADVANCED_BREAKPOINT_API_VERSION;
    (void)snprintf(item.id, sizeof(item.id), "%s", id);
    (void)snprintf(item.session_id, sizeof(item.session_id), "%s", "session-1");
    item.kind = kind;
    item.enabled = 1;
    item.stop_on_hit = 1;
    switch (kind) {
        case UMI_DEBUG_ADVANCED_BREAKPOINT_FUNCTION:
            (void)snprintf(item.name, sizeof(item.name), "%s", "calculate_risk");
            break;
        case UMI_DEBUG_ADVANCED_BREAKPOINT_DATA:
            (void)snprintf(item.name, sizeof(item.name), "%s", "position");
            (void)snprintf(item.reference, sizeof(item.reference), "%s", "data-17");
            item.access = UMI_DEBUG_DATA_BREAKPOINT_ACCESS_WRITE;
            break;
        case UMI_DEBUG_ADVANCED_BREAKPOINT_INSTRUCTION:
            (void)snprintf(item.reference, sizeof(item.reference), "%s", "0x1000");
            item.address = 0x1000U;
            break;
        case UMI_DEBUG_ADVANCED_BREAKPOINT_EXCEPTION:
            (void)snprintf(item.reference, sizeof(item.reference), "%s", "all-cpp");
            break;
        default:
            break;
    }
    return item;
}

int main(void)
{
    UmiDebugAdvancedBreakpointRegistry *registry = NULL;
    UmiDebugAdvancedBreakpointSnapshot snapshot;
    UmiDebugAdvancedBreakpointFilter filter;
    UmiDebugAdvancedBreakpoint result[4];
    UmiDebugAdvancedBreakpoint item;
    size_t result_count = 0U;
    size_t removed_count = 0U;

    assert(umi_debug_advanced_breakpoint_registry_create(2U, &registry) ==
           UMI_STATUS_OK);
    item = make_breakpoint("function-1",
                           UMI_DEBUG_ADVANCED_BREAKPOINT_FUNCTION);
    assert(umi_debug_advanced_breakpoint_registry_upsert(registry, &item) ==
           UMI_STATUS_OK);
    assert(umi_debug_advanced_breakpoint_registry_find(
               registry, "function-1", &item) == UMI_STATUS_OK);
    {
        const uint64_t original_sequence = item.sequence;
        (void)snprintf(item.condition, sizeof(item.condition), "%s", "value > 0");
        item.sequence = 0U;
        assert(umi_debug_advanced_breakpoint_registry_upsert(
                   registry, &item) == UMI_STATUS_OK);
        assert(umi_debug_advanced_breakpoint_registry_find(
                   registry, "function-1", &item) == UMI_STATUS_OK);
        assert(item.sequence == original_sequence);
    }
    item = make_breakpoint("data-1", UMI_DEBUG_ADVANCED_BREAKPOINT_DATA);
    item.verified = 1;
    assert(umi_debug_advanced_breakpoint_registry_upsert(registry, &item) ==
           UMI_STATUS_OK);
    item = make_breakpoint("instruction-1",
                           UMI_DEBUG_ADVANCED_BREAKPOINT_INSTRUCTION);
    assert(umi_debug_advanced_breakpoint_registry_upsert(registry, &item) ==
           UMI_STATUS_OK);
    item = make_breakpoint("exception-1",
                           UMI_DEBUG_ADVANCED_BREAKPOINT_EXCEPTION);
    assert(umi_debug_advanced_breakpoint_registry_upsert(registry, &item) ==
           UMI_STATUS_OK);

    assert(umi_debug_advanced_breakpoint_registry_snapshot(registry, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.breakpoint_count == 4U);
    assert(snapshot.function_count == 1U);
    assert(snapshot.data_count == 1U);
    assert(snapshot.instruction_count == 1U);
    assert(snapshot.exception_count == 1U);
    assert(snapshot.enabled_count == 4U);
    assert(snapshot.verified_count == 1U);

    (void)memset(&filter, 0, sizeof(filter));
    filter.struct_size = (uint32_t)sizeof(filter);
    filter.api_version = UMI_DEBUG_ADVANCED_BREAKPOINT_API_VERSION;
    filter.kind = UMI_DEBUG_ADVANCED_BREAKPOINT_DATA;
    filter.verified_only = 1;
    assert(umi_debug_advanced_breakpoint_registry_query(
               registry, &filter, result, 4U, &result_count) == UMI_STATUS_OK);
    assert(result_count == 1U);
    assert(strcmp(result[0].id, "data-1") == 0);

    assert(umi_debug_advanced_breakpoint_registry_set_enabled(
               registry, "data-1", 0) == UMI_STATUS_OK);
    assert(umi_debug_advanced_breakpoint_registry_find(
               registry, "data-1", &item) == UMI_STATUS_OK);
    assert(!item.enabled);

    item = make_breakpoint("bad-data", UMI_DEBUG_ADVANCED_BREAKPOINT_DATA);
    item.access = (UmiDebugDataBreakpointAccess)0;
    assert(umi_debug_advanced_breakpoint_registry_upsert(registry, &item) ==
           UMI_STATUS_INVALID_ARGUMENT);

    assert(umi_debug_advanced_breakpoint_registry_remove_session(
               registry, "session-1", &removed_count) == UMI_STATUS_OK);
    assert(removed_count == 4U);
    assert(umi_debug_advanced_breakpoint_registry_count(registry) == 0U);

    item = make_breakpoint("sequence-invalid",
                           UMI_DEBUG_ADVANCED_BREAKPOINT_FUNCTION);
    item.sequence = UINT64_MAX;
    assert(umi_debug_advanced_breakpoint_registry_upsert(registry, &item) ==
           UMI_STATUS_INVALID_ARGUMENT);
    item = make_breakpoint("sequence-limit",
                           UMI_DEBUG_ADVANCED_BREAKPOINT_FUNCTION);
    item.sequence = UINT64_MAX - 1U;
    assert(umi_debug_advanced_breakpoint_registry_upsert(registry, &item) ==
           UMI_STATUS_OK);
    item = make_breakpoint("sequence-overflow",
                           UMI_DEBUG_ADVANCED_BREAKPOINT_FUNCTION);
    assert(umi_debug_advanced_breakpoint_registry_upsert(registry, &item) ==
           UMI_STATUS_CAPACITY_EXCEEDED);
    assert(umi_debug_advanced_breakpoint_registry_count(registry) == 1U);
    umi_debug_advanced_breakpoint_registry_destroy(registry);
    return 0;
}
