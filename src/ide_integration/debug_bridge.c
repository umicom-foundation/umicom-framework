/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/debug_bridge.c
 *
 * PURPOSE:
 *   Implement active DAP frame cross-navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/debug_bridge.h"

#include <stdio.h>
#include <string.h>

/*
 * Initialise ide debug bridge from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ide_debug_bridge_init(
    UmiIdeDebugBridge *bridge,
    UmiDebugRuntimePlatform *runtime,
    UmiIdeDebugFrameLocationResolver resolver,
    void *user_data)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bridge == NULL || runtime == NULL || resolver == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    bridge->runtime = runtime;
    bridge->resolve_frame = resolver;
    bridge->resolver_user_data = user_data;
    return UMI_STATUS_OK;
}

/*
 * Provide the ide debug active frame target operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_debug_active_frame_target(
    UmiIdeDebugBridge *bridge,
    UmiIdeNavigationTarget *out_target)
{
    UmiDebugRuntimePlatformSnapshot snapshot;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bridge == NULL || bridge->runtime == NULL ||
        bridge->resolve_frame == NULL || out_target == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_debug_runtime_platform_snapshot(
        bridge->runtime,
        &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!snapshot.active || snapshot.active_frame_id == 0U) {
        return UMI_STATUS_NOT_FOUND;
    }

    (void)memset(out_target, 0, sizeof(*out_target));
    out_target->domain = UMI_IDE_DOMAIN_DEBUG;
    out_target->reason = UMI_IDE_NAVIGATION_DEBUG;

    (void)snprintf(
        out_target->subject_id,
        sizeof(out_target->subject_id),
        "frame.%llu",
        (unsigned long long)snapshot.active_frame_id);
    (void)snprintf(
        out_target->label,
        sizeof(out_target->label),
        "Debug frame %llu",
        (unsigned long long)snapshot.active_frame_id);

    umi_ide_location_init(&out_target->location);

    return bridge->resolve_frame(
        bridge->resolver_user_data,
        snapshot.active_frame_id,
        &out_target->location);
}
