/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/advanced_debugging.c
 *
 * PURPOSE:
 *   Implement the reusable advanced-debugging composition root.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/advanced_debugging.h"

#include <stdlib.h>
#include <string.h>

struct UmiDebugAdvancedPlatform {
    UmiDebugService *service;
    UmiDebugAdapterRegistry *adapters;
    UmiDebugInspectionSession *inspection;
    uint64_t revision;
};

/*
 * Initialise debug advanced platform from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_advanced_platform_create(
    UmiDebugService *service,
    UmiDebugAdvancedPlatform **out_platform)
{
    UmiDebugAdvancedPlatform *platform;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || out_platform == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_platform = NULL;
    platform = (UmiDebugAdvancedPlatform *)calloc(1U, sizeof(*platform));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    platform->service = service;
    platform->revision = 1U;
    status = umi_debug_adapter_registry_create(0U, &platform->adapters);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_debug_inspection_session_create(
            service, platform->adapters, &platform->inspection);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_debug_advanced_platform_destroy(platform);
        return status;
    }
    *out_platform = platform;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by debug advanced platform so the same storage can be reused
 * safely.
 */
void umi_debug_advanced_platform_destroy(UmiDebugAdvancedPlatform *platform)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return;
    umi_debug_inspection_session_destroy(platform->inspection);
    umi_debug_adapter_registry_destroy(platform->adapters);
    (void)memset(platform, 0, sizeof(*platform));
    free(platform);
}

/*
 * Provide the debug advanced platform register adapter operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_advanced_platform_register_adapter(
    UmiDebugAdvancedPlatform *platform,
    const UmiDebugAdapterDescriptor *descriptor)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_debug_adapter_registry_register(platform->adapters, descriptor);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the debug advanced platform unregister adapter operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_advanced_platform_unregister_adapter(
    UmiDebugAdvancedPlatform *platform,
    const char *adapter_id)
{
    UmiDebugInspectionSessionSnapshot inspection;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || adapter_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_debug_inspection_session_snapshot(platform->inspection,
                                                   &inspection);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (inspection.bound && strcmp(inspection.adapter_id, adapter_id) == 0) {
        return UMI_STATUS_BUSY;
    }
    status = umi_debug_adapter_registry_unregister(platform->adapters,
                                                   adapter_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the debug advanced platform open session operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_advanced_platform_open_session(
    UmiDebugAdvancedPlatform *platform,
    const char *debug_session_id,
    const char *adapter_id,
    uint64_t advertised_capabilities)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_debug_inspection_session_bind(
        platform->inspection, debug_session_id, adapter_id,
        advertised_capabilities);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the debug advanced platform open best session operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_advanced_platform_open_best_session(
    UmiDebugAdvancedPlatform *platform,
    const char *debug_session_id,
    const char *debugger_kind,
    uint64_t required_capabilities,
    uint64_t advertised_capabilities,
    int allow_remote)
{
    UmiDebugAdapterDescriptor descriptor;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || debug_session_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_debug_adapter_registry_select(
        platform->adapters, debugger_kind, required_capabilities,
        allow_remote, &descriptor);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_debug_advanced_platform_open_session(
        platform, debug_session_id, descriptor.id,
        advertised_capabilities & descriptor.capabilities);
}

/*
 * Provide the debug advanced platform close session operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_advanced_platform_close_session(
    UmiDebugAdvancedPlatform *platform)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_debug_inspection_session_unbind(platform->inspection);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the debug advanced platform refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_advanced_platform_refresh(
    UmiDebugAdvancedPlatform *platform)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_debug_inspection_session_refresh_threads(
        platform->inspection);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) platform->revision += 1U;
    return status;
}

/*
 * Provide the debug advanced platform invoke operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_advanced_platform_invoke(
    UmiDebugAdvancedPlatform *platform,
    const UmiDebugAdapterRequest *request,
    const UmiDebugAdapterCancellation *cancellation,
    UmiDebugAdapterResponse *out_response)
{
    uint64_t revision_before;
    uint64_t revision_after;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    revision_before = umi_debug_inspection_session_revision(
        platform->inspection);
    status = umi_debug_inspection_session_invoke(
        platform->inspection, request, cancellation, out_response);
    revision_after = umi_debug_inspection_session_revision(
        platform->inspection);
    /* Apply this branch only when its contract condition is satisfied. */
    if (revision_after != revision_before) platform->revision += 1U;
    return status;
}

/*
 * Provide the debug advanced platform command enabled operation used by this module and
 * its client applications.
 */
int umi_debug_advanced_platform_command_enabled(
    const UmiDebugAdvancedPlatform *platform,
    UmiDebugCommandKind command_kind)
{
    return platform != NULL &&
           umi_debug_inspection_session_command_enabled(
               platform->inspection, command_kind);
}

/*
 * Provide the debug advanced platform adapters operation used by this module and its
 * client applications.
 */
UmiDebugAdapterRegistry *umi_debug_advanced_platform_adapters(
    UmiDebugAdvancedPlatform *platform)
{
    return platform != NULL ? platform->adapters : NULL;
}

/*
 * Provide the debug advanced platform inspection operation used by this module and its
 * client applications.
 */
UmiDebugInspectionSession *umi_debug_advanced_platform_inspection(
    UmiDebugAdvancedPlatform *platform)
{
    return platform != NULL ? platform->inspection : NULL;
}

/*
 * Provide the debug advanced platform snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_advanced_platform_snapshot(
    const UmiDebugAdvancedPlatform *platform,
    UmiDebugAdvancedPlatformSnapshot *out_snapshot)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DEBUG_ADVANCED_PLATFORM_API_VERSION;
    status = umi_debug_adapter_registry_snapshot(platform->adapters,
                                                 &out_snapshot->adapters);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_debug_inspection_session_snapshot(platform->inspection,
                                                   &out_snapshot->inspection);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->revision = platform->revision;
    out_snapshot->session_open = out_snapshot->inspection.bound;
    out_snapshot->adapter_available =
        out_snapshot->adapters.enabled_count != 0U;
    out_snapshot->ready = out_snapshot->session_open &&
                          out_snapshot->adapter_available &&
                          out_snapshot->inspection.state !=
                              UMI_DEBUG_INSPECTION_FAILED;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug advanced platform revision operation used by this module and its
 * client applications.
 */
uint64_t umi_debug_advanced_platform_revision(
    const UmiDebugAdvancedPlatform *platform)
{
    return platform != NULL ? platform->revision : 0U;
}
