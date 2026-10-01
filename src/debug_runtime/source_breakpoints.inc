/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/source_breakpoints.inc
 * PURPOSE: Synchronize one whole source set without stack-heavy buffers or partial verification.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/breakpoint_sync.h"

/* Included by the platform owner so transport/session authority stays private. */
UmiStatus umi_debug_runtime_platform_sync_breakpoints(UmiDebugRuntimePlatform *platform,
    const char *source_uri, uint32_t timeout_ms)
{
    if (platform == NULL || source_uri == NULL || source_uri[0] == '\0' || platform->adapter == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugBreakpointRegistry *registry = umi_debug_service_breakpoint(platform->service);
    UmiDebugBreakpointSnapshot *requested = calloc(UMI_DEBUG_RUNTIME_MAX_BREAKPOINTS, sizeof *requested);
    UmiDebugRuntimeBreakpointList *result = calloc(1U, sizeof *result);
    UmiDebugRuntimeEnvelope *response = calloc(1U, sizeof *response);
    UmiStatus status = UMI_STATUS_OK; size_t count = 0U; uint64_t sequence = 0U, generation = 0U;
    if (requested == NULL || result == NULL || response == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
    for (size_t i = 0U; i < umi_debug_breakpoint_registry_count(registry); ++i) {
        UmiDebugBreakpointSnapshot item;
        status = umi_debug_breakpoint_registry_at(registry, i, &item);
        if (status != UMI_STATUS_OK) goto done;
        if (strcmp(item.uri, source_uri) != 0 || !item.enabled) continue;
        if (count == UMI_DEBUG_RUNTIME_MAX_BREAKPOINTS) { status = UMI_STATUS_CAPACITY_EXCEEDED; goto done; }
        requested[count++] = item;
    }
    status = UmiDebugRuntimeBreakpointSetValidate(&platform->capabilities, requested, count);
    if (status != UMI_STATUS_OK) goto done;
    /* This request replaces the complete source set. Remove old confirmation
     * before I/O; timeout or malformed replies cannot assert adapter readiness. */
    bool hadConfirmation = false;
    for (size_t i = 0U; i < count; ++i) {
        if (requested[i].verified) hadConfirmation = true;
        requested[i].verified = 0;
    }
    if (hadConfirmation) {
        status = umi_debug_breakpoint_registry_upsert_many(registry, requested, count, NULL);
        if (status != UMI_STATUS_OK) goto done;
        for (size_t i = 0U; i < count; ++i) {
            status = umi_debug_breakpoint_registry_find(registry, requested[i].id, &requested[i]);
            if (status != UMI_STATUS_OK) goto done;
        }
    }
    generation = umi_debug_breakpoint_registry_revision(registry);
    status = umi_debug_runtime_request_set_breakpoints(platform->adapter, source_uri, requested, count, 0, &sequence);
    if (status == UMI_STATUS_OK) status = umi_debug_runtime_adapter_wait_response(platform->adapter, sequence, timeout_ms, response);
    if (status == UMI_STATUS_OK && !response->success) status = UMI_STATUS_UNAVAILABLE;
    if (status == UMI_STATUS_OK) status = umi_debug_runtime_decode_breakpoints(response->json, result);
    if (status == UMI_STATUS_OK) status = UmiDebugRuntimeApplyBreakpointReply(registry, generation, requested, count, result);
    if (status == UMI_STATUS_OK) platform->revision++;
done:
    free(response); free(result); free(requested); return status;
}
