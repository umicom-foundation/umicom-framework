/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/debug_bridge.c
 *
 * PURPOSE:
 *   Implement active DAP frame cross-navigation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/debug_bridge.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_ide_debug_bridge_init(
    UmiIdeDebugBridge *bridge,
    UmiDebugRuntimePlatform *runtime,
    UmiIdeDebugFrameLocationResolver resolver,
    void *user_data)
{
    if (bridge == NULL || runtime == NULL || resolver == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    bridge->runtime = runtime;
    bridge->resolve_frame = resolver;
    bridge->resolver_user_data = user_data;
    return UMI_STATUS_OK;
}

UmiStatus umi_ide_debug_active_frame_target(
    UmiIdeDebugBridge *bridge,
    UmiIdeNavigationTarget *out_target)
{
    UmiDebugRuntimePlatformSnapshot snapshot;
    UmiStatus status;

    if (bridge == NULL || bridge->runtime == NULL ||
        bridge->resolve_frame == NULL || out_target == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_debug_runtime_platform_snapshot(
        bridge->runtime,
        &snapshot);
    if (status != UMI_STATUS_OK) return status;

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
