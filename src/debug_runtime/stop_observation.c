/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/stop_observation.c
 *
 * PURPOSE:
 *   Reuse the DAP event and stack owners for bounded stop observation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/stop_observation.h"
#include "deadline.h"

UmiStatus UmiDebugRuntimeObserveStop(UmiDebugRuntimePlatform *platform, uint32_t timeoutMs,
    UmiDebugStackFrameSnapshot *outFrame)
{
    if (platform == NULL || timeoutMs == 0U || outFrame == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    DebugDeadline deadline = DebugDeadlineStart(timeoutMs);
    UmiDebugRuntimePlatformSnapshot snapshot;
    UmiStatus status = umi_debug_runtime_platform_snapshot(platform, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (!snapshot.active) return UMI_STATUS_INVALID_STATE;

    /* Consume the same events as the IDE. Every request shares the original
     * deadline so asynchronous notifications cannot extend a CLI observation. */
    while (!snapshot.paused) {
        uint32_t remaining = DebugDeadlineRemaining(&deadline);
        if (remaining == 0U) return UMI_STATUS_TIMEOUT;
        int handled;
        status = umi_debug_runtime_platform_pump_event(platform, remaining, &handled);
        if (status != UMI_STATUS_OK) return status;
        status = umi_debug_runtime_platform_snapshot(platform, &snapshot);
        if (status != UMI_STATUS_OK) return status;
        if (!snapshot.active) return UMI_STATUS_NOT_FOUND;
    }
    uint32_t remaining = DebugDeadlineRemaining(&deadline);
    if (remaining == 0U) return UMI_STATUS_TIMEOUT;
    if (snapshot.active_thread_id == 0U) {
        status = umi_debug_runtime_platform_refresh_threads(platform, remaining);
        if (status != UMI_STATUS_OK) return status;
        status = umi_debug_runtime_platform_snapshot(platform, &snapshot);
        if (status != UMI_STATUS_OK) return status;
        remaining = DebugDeadlineRemaining(&deadline);
        if (remaining == 0U) return UMI_STATUS_TIMEOUT;
    }
    if (!snapshot.active || !snapshot.paused || snapshot.active_thread_id == 0U)
        return UMI_STATUS_NOT_FOUND;
    status = umi_debug_runtime_platform_refresh_stack(platform, snapshot.active_thread_id, remaining);
    if (status != UMI_STATUS_OK) return status;
    UmiDebugService *service = umi_debug_runtime_platform_service(platform);
    UmiDebugStackFrameSnapshot frame;
    status = umi_debug_stack_frame_registry_at(umi_debug_service_stack_frame(service), 0U, &frame);
    if (status != UMI_STATUS_OK) return status;
    if (DebugDeadlineExpired(&deadline)) return UMI_STATUS_TIMEOUT;
    *outFrame = frame;
    return UMI_STATUS_OK;
}
