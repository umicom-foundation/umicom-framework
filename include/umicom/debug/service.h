/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/service.h
 *
 * PURPOSE:
 *   Define the reusable debugger service aggregating launch configurations, breakpoints, sessions, threads, stack frames, variables and events.
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
#ifndef FRAMEWORK_INCLUDE_UMICOM_DEBUG_SERVICE_H
#define FRAMEWORK_INCLUDE_UMICOM_DEBUG_SERVICE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/debug/launch_configuration.h"
#include "umicom/debug/breakpoint.h"
#include "umicom/debug/session.h"
#include "umicom/debug/thread.h"
#include "umicom/debug/stack_frame.h"
#include "umicom/debug/scope.h"
#include "umicom/debug/variable.h"
#include "umicom/debug/watch.h"
#include "umicom/debug/console_entry.h"
#include "umicom/debug/module.h"
#include "umicom/debug/source.h"
#include "umicom/debug/exception.h"
#include "umicom/debug/event.h"
#include "umicom/debug/adapter_profile.h"
#include "umicom/debug/timeline.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug service data shared with callers of this public contract.
 */
typedef struct UmiDebugService UmiDebugService;
/**
 * Represent the debug service snapshot data shared with callers of this public contract.
 */
typedef struct UmiDebugServiceSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t revision;
    size_t item_count;
    size_t launch_configuration_count;
    size_t breakpoint_count;
    size_t session_count;
    size_t thread_count;
    size_t stack_frame_count;
    size_t scope_count;
    size_t variable_count;
    size_t watch_count;
    size_t console_entry_count;
    size_t module_count;
    size_t source_count;
    size_t exception_count;
    size_t event_count;
    size_t adapter_profile_count;
    size_t timeline_event_count;
} UmiDebugServiceSnapshot;

/**
 * Initialise debug service from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_debug_service_create(UmiDebugService **out_owner);
/**
 * Release or reset state held by debug service so the same storage can be reused safely.
 */
void umi_debug_service_destroy(UmiDebugService *owner);
/**
 * Provide the debug service snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_service_snapshot(const UmiDebugService *owner, UmiDebugServiceSnapshot *out_snapshot);
/**
 * Provide the debug service launch configuration operation used by this module and its
 * client applications.
 */
UmiDebugLaunchConfigurationRegistry *umi_debug_service_launch_configuration(UmiDebugService *owner);
/**
 * Provide the debug service breakpoint operation used by this module and its client
 * applications.
 */
UmiDebugBreakpointRegistry *umi_debug_service_breakpoint(UmiDebugService *owner);
/**
 * Provide the debug service session operation used by this module and its client
 * applications.
 */
UmiDebugSessionRegistry *umi_debug_service_session(UmiDebugService *owner);
/**
 * Provide the debug service thread operation used by this module and its client
 * applications.
 */
UmiDebugThreadRegistry *umi_debug_service_thread(UmiDebugService *owner);
/**
 * Provide the debug service stack frame operation used by this module and its client
 * applications.
 */
UmiDebugStackFrameRegistry *umi_debug_service_stack_frame(UmiDebugService *owner);
/**
 * Provide the debug service scope operation used by this module and its client
 * applications.
 */
UmiDebugScopeRegistry *umi_debug_service_scope(UmiDebugService *owner);
/**
 * Provide the debug service variable operation used by this module and its client
 * applications.
 */
UmiDebugVariableRegistry *umi_debug_service_variable(UmiDebugService *owner);
/**
 * Provide the debug service watch operation used by this module and its client
 * applications.
 */
UmiDebugWatchRegistry *umi_debug_service_watch(UmiDebugService *owner);
/**
 * Provide the debug service console entry operation used by this module and its client
 * applications.
 */
UmiDebugConsoleEntryRegistry *umi_debug_service_console_entry(UmiDebugService *owner);
/**
 * Provide the debug service module operation used by this module and its client
 * applications.
 */
UmiDebugModuleRegistry *umi_debug_service_module(UmiDebugService *owner);
/**
 * Provide the debug service source operation used by this module and its client
 * applications.
 */
UmiDebugSourceRegistry *umi_debug_service_source(UmiDebugService *owner);
/**
 * Provide the debug service exception operation used by this module and its client
 * applications.
 */
UmiDebugExceptionRegistry *umi_debug_service_exception(UmiDebugService *owner);
/**
 * Provide the debug service event operation used by this module and its client
 * applications.
 */
UmiDebugEventRegistry *umi_debug_service_event(UmiDebugService *owner);
/**
 * Provide the debug service adapter profiles operation used by this module and its client
 * applications.
 */
UmiDebugAdapterProfileRegistry *umi_debug_service_adapter_profiles(UmiDebugService *owner);
/**
 * Provide the debug service timeline operation used by this module and its client
 * applications.
 */
UmiDebugTimeline *umi_debug_service_timeline(UmiDebugService *owner);

#ifdef __cplusplus
}
#endif
#endif
