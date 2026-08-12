/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/service.h
 *
 * PURPOSE:
 *   Define the reusable debugger service aggregating launch configurations, breakpoints, sessions, threads, stack frames, variables and events.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugService UmiDebugService;
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
} UmiDebugServiceSnapshot;

UmiStatus umi_debug_service_create(UmiDebugService **out_owner);
void umi_debug_service_destroy(UmiDebugService *owner);
UmiStatus umi_debug_service_snapshot(const UmiDebugService *owner, UmiDebugServiceSnapshot *out_snapshot);
UmiDebugLaunchConfigurationRegistry *umi_debug_service_launch_configuration(UmiDebugService *owner);
UmiDebugBreakpointRegistry *umi_debug_service_breakpoint(UmiDebugService *owner);
UmiDebugSessionRegistry *umi_debug_service_session(UmiDebugService *owner);
UmiDebugThreadRegistry *umi_debug_service_thread(UmiDebugService *owner);
UmiDebugStackFrameRegistry *umi_debug_service_stack_frame(UmiDebugService *owner);
UmiDebugScopeRegistry *umi_debug_service_scope(UmiDebugService *owner);
UmiDebugVariableRegistry *umi_debug_service_variable(UmiDebugService *owner);
UmiDebugWatchRegistry *umi_debug_service_watch(UmiDebugService *owner);
UmiDebugConsoleEntryRegistry *umi_debug_service_console_entry(UmiDebugService *owner);
UmiDebugModuleRegistry *umi_debug_service_module(UmiDebugService *owner);
UmiDebugSourceRegistry *umi_debug_service_source(UmiDebugService *owner);
UmiDebugExceptionRegistry *umi_debug_service_exception(UmiDebugService *owner);
UmiDebugEventRegistry *umi_debug_service_event(UmiDebugService *owner);

#ifdef __cplusplus
}
#endif
#endif
