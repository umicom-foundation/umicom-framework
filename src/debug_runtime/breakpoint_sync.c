/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/breakpoint_sync.c
 * PURPOSE: Check negotiated breakpoint semantics and publish complete verification results.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/breakpoint_sync.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

UmiStatus UmiDebugRuntimeBreakpointSetValidate(const UmiDebugRuntimeCapabilities *capabilities,
    const UmiDebugBreakpointSnapshot *items, size_t count)
{
    if ((count != 0U && items == NULL) || count > UMI_DEBUG_RUNTIME_MAX_BREAKPOINTS)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < count; ++i) {
        const UmiDebugBreakpointSnapshot *item = &items[i];
        if (memchr(item->condition, '\0', sizeof item->condition) == NULL ||
            memchr(item->log_message, '\0', sizeof item->log_message) == NULL)
            return UMI_STATUS_INVALID_ARGUMENT;
        if ((item->enabled != 0 && item->enabled != 1) ||
            (item->enabled && (item->line == 0U || item->line > INT32_MAX || item->column > INT32_MAX)))
            return UMI_STATUS_INVALID_ARGUMENT;
        if (item->enabled && capabilities != NULL &&
            ((item->condition[0] != '\0' && !capabilities->supports_conditional_breakpoints) ||
             (item->log_message[0] != '\0' && !capabilities->supports_log_points))) return UMI_STATUS_UNAVAILABLE;
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiDebugRuntimeApplyBreakpointReply(UmiDebugBreakpointRegistry *registry,
    uint64_t expectedRevision, const UmiDebugBreakpointSnapshot *requested, size_t count,
    const UmiDebugRuntimeBreakpointList *reply)
{
    if (registry == NULL || reply == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiDebugRuntimeBreakpointSetValidate(NULL, requested, count);
    if (status != UMI_STATUS_OK) return status;
    if (umi_debug_breakpoint_registry_revision(registry) != expectedRevision) return UMI_STATUS_BUSY;
    if (reply->count != count) return UMI_STATUS_PARSE_ERROR;
    if (count == 0U) return UMI_STATUS_OK;
    UmiDebugBreakpointSnapshot *updates = calloc(count, sizeof *updates);
    if (updates == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    for (size_t i = 0U; i < count; ++i) {
        UmiDebugBreakpointSnapshot current;
        status = umi_debug_breakpoint_snapshot_validate(&requested[i], NULL);
        if (status != UMI_STATUS_OK) break;
        if (requested[i].uri[0] == '\0' || (i != 0U && strcmp(requested[0].uri, requested[i].uri) != 0)) {
            status = UMI_STATUS_INVALID_ARGUMENT; break;
        }
        status = umi_debug_breakpoint_registry_find(registry, requested[i].id, &current);
        if (status != UMI_STATUS_OK) break;
        if (!requested[i].enabled || current.revision != requested[i].revision ||
            strcmp(current.uri, requested[i].uri) != 0 || current.line != requested[i].line ||
            current.column != requested[i].column || current.enabled != requested[i].enabled ||
            strcmp(current.condition, requested[i].condition) != 0 ||
            strcmp(current.log_message, requested[i].log_message) != 0 ||
            strcmp(current.session_id, requested[i].session_id) != 0) { status = UMI_STATUS_BUSY; break; }
        const UmiDebugRuntimeBreakpoint *result = &reply->items[i];
        if ((result->verified != 0 && result->verified != 1) || result->line > INT32_MAX || result->column > INT32_MAX) {
            status = UMI_STATUS_PARSE_ERROR; break;
        }
        updates[i] = current; updates[i].verified = result->verified;
        if (result->line != 0U) updates[i].line = result->line;
        if (result->column != 0U) updates[i].column = result->column;
    }
    if (status == UMI_STATUS_OK)
        status = umi_debug_breakpoint_registry_upsert_many(registry, updates, count, NULL);
    free(updates); return status;
}
