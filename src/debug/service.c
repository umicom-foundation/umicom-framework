/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/service.c
 *
 * PURPOSE:
 *   Implement the reusable debugger service aggregating launch configurations, breakpoints, sessions, threads, stack frames, variables and events.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#include "umicom/debug/service.h"

#include <stdlib.h>
#include <string.h>

struct UmiDebugService {
    UmiDebugLaunchConfigurationRegistry *launch_configuration;
    UmiDebugBreakpointRegistry *breakpoint;
    UmiDebugSessionRegistry *session;
    UmiDebugThreadRegistry *thread;
    UmiDebugStackFrameRegistry *stack_frame;
    UmiDebugScopeRegistry *scope;
    UmiDebugVariableRegistry *variable;
    UmiDebugWatchRegistry *watch;
    UmiDebugConsoleEntryRegistry *console_entry;
    UmiDebugModuleRegistry *module;
    UmiDebugSourceRegistry *source;
    UmiDebugExceptionRegistry *exception;
    UmiDebugEventRegistry *event;
    UmiDebugAdapterProfileRegistry *adapter_profiles;
    UmiDebugTimeline *timeline;
    uint64_t revision;
};

/*
 * Initialise debug service from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_debug_service_create(UmiDebugService **out_owner)
{
    UmiDebugService *owner; UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_owner = NULL; owner = (UmiDebugService *)calloc(1U,sizeof(*owner));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (owner == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    owner->revision = 1U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_debug_launch_configuration_registry_create(&owner->launch_configuration);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_debug_breakpoint_registry_create(&owner->breakpoint);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_debug_session_registry_create(&owner->session);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_debug_thread_registry_create(&owner->thread);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_debug_stack_frame_registry_create(&owner->stack_frame);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_debug_scope_registry_create(&owner->scope);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_debug_variable_registry_create(&owner->variable);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_debug_watch_registry_create(&owner->watch);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_debug_console_entry_registry_create(&owner->console_entry);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_debug_module_registry_create(&owner->module);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_debug_source_registry_create(&owner->source);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_debug_exception_registry_create(&owner->exception);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_debug_event_registry_create(&owner->event);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_debug_adapter_profile_registry_create(&owner->adapter_profiles);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_debug_timeline_create(&owner->timeline);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) { umi_debug_service_destroy(owner); return status; }
    *out_owner = owner; return UMI_STATUS_OK;
}

/* Release or reset state held by debug service so the same storage can be reused safely. */
void umi_debug_service_destroy(UmiDebugService *owner)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (owner == NULL) return;
    umi_debug_timeline_destroy(owner->timeline);
    umi_debug_adapter_profile_registry_destroy(owner->adapter_profiles);
    umi_debug_event_registry_destroy(owner->event);
    umi_debug_exception_registry_destroy(owner->exception);
    umi_debug_source_registry_destroy(owner->source);
    umi_debug_module_registry_destroy(owner->module);
    umi_debug_console_entry_registry_destroy(owner->console_entry);
    umi_debug_watch_registry_destroy(owner->watch);
    umi_debug_variable_registry_destroy(owner->variable);
    umi_debug_scope_registry_destroy(owner->scope);
    umi_debug_stack_frame_registry_destroy(owner->stack_frame);
    umi_debug_thread_registry_destroy(owner->thread);
    umi_debug_session_registry_destroy(owner->session);
    umi_debug_breakpoint_registry_destroy(owner->breakpoint);
    umi_debug_launch_configuration_registry_destroy(owner->launch_configuration);
    free(owner);
}

/*
 * Provide the debug service snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_service_snapshot(const UmiDebugService *owner, UmiDebugServiceSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (owner == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_snapshot,0,sizeof(*out_snapshot));
    out_snapshot->struct_size=(uint32_t)sizeof(*out_snapshot); out_snapshot->api_version=1U;
    out_snapshot->revision=owner->revision;
    out_snapshot->launch_configuration_count = umi_debug_launch_configuration_registry_count(owner->launch_configuration);
    out_snapshot->breakpoint_count = umi_debug_breakpoint_registry_count(owner->breakpoint);
    out_snapshot->session_count = umi_debug_session_registry_count(owner->session);
    out_snapshot->thread_count = umi_debug_thread_registry_count(owner->thread);
    out_snapshot->stack_frame_count = umi_debug_stack_frame_registry_count(owner->stack_frame);
    out_snapshot->scope_count = umi_debug_scope_registry_count(owner->scope);
    out_snapshot->variable_count = umi_debug_variable_registry_count(owner->variable);
    out_snapshot->watch_count = umi_debug_watch_registry_count(owner->watch);
    out_snapshot->console_entry_count = umi_debug_console_entry_registry_count(owner->console_entry);
    out_snapshot->module_count = umi_debug_module_registry_count(owner->module);
    out_snapshot->source_count = umi_debug_source_registry_count(owner->source);
    out_snapshot->exception_count = umi_debug_exception_registry_count(owner->exception);
    out_snapshot->event_count = umi_debug_event_registry_count(owner->event);
    out_snapshot->adapter_profile_count = umi_debug_adapter_profile_registry_count(owner->adapter_profiles);
    out_snapshot->timeline_event_count = umi_debug_timeline_count(owner->timeline);
    out_snapshot->item_count = out_snapshot->launch_configuration_count + out_snapshot->breakpoint_count + out_snapshot->session_count + out_snapshot->thread_count + out_snapshot->stack_frame_count + out_snapshot->scope_count + out_snapshot->variable_count + out_snapshot->watch_count + out_snapshot->console_entry_count + out_snapshot->module_count + out_snapshot->source_count + out_snapshot->exception_count + out_snapshot->event_count + out_snapshot->adapter_profile_count + out_snapshot->timeline_event_count;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug service launch configuration operation used by this module and its
 * client applications.
 */
UmiDebugLaunchConfigurationRegistry *umi_debug_service_launch_configuration(UmiDebugService *owner) { return owner != NULL ? owner->launch_configuration : NULL; }
/*
 * Provide the debug service breakpoint operation used by this module and its client
 * applications.
 */
UmiDebugBreakpointRegistry *umi_debug_service_breakpoint(UmiDebugService *owner) { return owner != NULL ? owner->breakpoint : NULL; }
/*
 * Provide the debug service session operation used by this module and its client
 * applications.
 */
UmiDebugSessionRegistry *umi_debug_service_session(UmiDebugService *owner) { return owner != NULL ? owner->session : NULL; }
/*
 * Provide the debug service thread operation used by this module and its client
 * applications.
 */
UmiDebugThreadRegistry *umi_debug_service_thread(UmiDebugService *owner) { return owner != NULL ? owner->thread : NULL; }
/*
 * Provide the debug service stack frame operation used by this module and its client
 * applications.
 */
UmiDebugStackFrameRegistry *umi_debug_service_stack_frame(UmiDebugService *owner) { return owner != NULL ? owner->stack_frame : NULL; }
/*
 * Provide the debug service scope operation used by this module and its client
 * applications.
 */
UmiDebugScopeRegistry *umi_debug_service_scope(UmiDebugService *owner) { return owner != NULL ? owner->scope : NULL; }
/*
 * Provide the debug service variable operation used by this module and its client
 * applications.
 */
UmiDebugVariableRegistry *umi_debug_service_variable(UmiDebugService *owner) { return owner != NULL ? owner->variable : NULL; }
/*
 * Provide the debug service watch operation used by this module and its client
 * applications.
 */
UmiDebugWatchRegistry *umi_debug_service_watch(UmiDebugService *owner) { return owner != NULL ? owner->watch : NULL; }
/*
 * Provide the debug service console entry operation used by this module and its client
 * applications.
 */
UmiDebugConsoleEntryRegistry *umi_debug_service_console_entry(UmiDebugService *owner) { return owner != NULL ? owner->console_entry : NULL; }
/*
 * Provide the debug service module operation used by this module and its client
 * applications.
 */
UmiDebugModuleRegistry *umi_debug_service_module(UmiDebugService *owner) { return owner != NULL ? owner->module : NULL; }
/*
 * Provide the debug service source operation used by this module and its client
 * applications.
 */
UmiDebugSourceRegistry *umi_debug_service_source(UmiDebugService *owner) { return owner != NULL ? owner->source : NULL; }
/*
 * Provide the debug service exception operation used by this module and its client
 * applications.
 */
UmiDebugExceptionRegistry *umi_debug_service_exception(UmiDebugService *owner) { return owner != NULL ? owner->exception : NULL; }
/*
 * Provide the debug service event operation used by this module and its client
 * applications.
 */
UmiDebugEventRegistry *umi_debug_service_event(UmiDebugService *owner) { return owner != NULL ? owner->event : NULL; }
/*
 * Provide the debug service adapter profiles operation used by this module and its client
 * applications.
 */
UmiDebugAdapterProfileRegistry *umi_debug_service_adapter_profiles(UmiDebugService *owner) { return owner != NULL ? owner->adapter_profiles : NULL; }
/*
 * Provide the debug service timeline operation used by this module and its client
 * applications.
 */
UmiDebugTimeline *umi_debug_service_timeline(UmiDebugService *owner) { return owner != NULL ? owner->timeline : NULL; }
