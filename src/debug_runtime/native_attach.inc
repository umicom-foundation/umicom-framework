/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/native_attach.inc
 * PURPOSE: Use the existing debugger session owner for an explicitly selected running process.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/native_attach.h"
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <unistd.h>
#endif
UmiStatus UmiDebugRuntimePlatformAttachNative(UmiDebugRuntimePlatform *platform,
                                              const UmiDebugNativeAttachOptions *options,
                                              uint32_t timeout_ms)
{
    if (platform == NULL || options == NULL || timeout_ms == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (platform->active)
        return UMI_STATUS_BUSY;
#ifdef _WIN32
    uint64_t host = (uint64_t)GetCurrentProcessId();
#else
    uint64_t host = (uint64_t)getpid();
#endif
    /* Attaching the IDE to itself can stop the thread needed for the protocol.
     * The explicit PID still identifies an external process only at request time. */
    if (options->process_id == host)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugNativeAttachPlan *plan = malloc(sizeof *plan);
    if (plan == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiDebugNativeAttachPlanCreate(options, plan);
    if (status == UMI_STATUS_OK && platform->native_generation == UINT64_MAX)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK)
        status = umi_debug_adapter_profile_registry_upsert(
            umi_debug_service_adapter_profiles(platform->service), &plan->profile);
    if (status == UMI_STATUS_OK)
        status = umi_debug_launch_configuration_registry_upsert(
            umi_debug_service_launch_configuration(platform->service), &plan->configuration);
    if (status == UMI_STATUS_OK)
    {
        char session[128];
        (void)snprintf(session, sizeof session, "native.%llu",
                       (unsigned long long)++platform->native_generation);
        /* Inspection captures from the preceding process must never become
         * evidence about this attachment. Desired breakpoints remain reusable. */
        umi_debug_thread_registry_clear(umi_debug_service_thread(platform->service));
        umi_debug_stack_frame_registry_clear(umi_debug_service_stack_frame(platform->service));
        umi_debug_scope_registry_clear(umi_debug_service_scope(platform->service));
        umi_debug_variable_registry_clear(umi_debug_service_variable(platform->service));
        platform->paused = 0;
        platform->initialized = 0;
        platform->active_thread_id = 0U;
        platform->active_frame_id = 0U;
        status = DebugPlatformStartWithEnvironment(
            platform, plan->profile.id, session, plan->configuration.id, plan->arguments, 1,
            plan->configuration.working_directory, timeout_ms, plan->tool_directory, "");
    }
    free(plan);
    return status;
}
