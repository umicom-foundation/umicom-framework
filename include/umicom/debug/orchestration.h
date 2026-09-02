/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/orchestration.h
 *
 * PURPOSE:
 *   Coordinate configuration resolution, debug queries and command state over
 *   the canonical Framework debugger service, controller and workspace.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_ORCHESTRATION_H
#define UMICOM_DEBUG_ORCHESTRATION_H

#include "umicom/debug/breakpoint_query.h"
#include "umicom/debug/command.h"
#include "umicom/debug/compound_configuration.h"
#include "umicom/debug/console_query.h"
#include "umicom/debug/service.h"
#include "umicom/debug/watch_query.h"
#include "umicom/debug/workspace.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_ORCHESTRATION_API_VERSION 1U

/**
 * Represent the debug orchestration snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugOrchestrationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char selected_configuration_id[128];
    char selected_session_id[128];
    UmiDebugControllerState controller_state;
    size_t configuration_count;
    size_t session_count;
    size_t breakpoint_result_count;
    size_t watch_result_count;
    size_t console_result_count;
    uint64_t revision;
    int configuration_ready;
    int can_start;
    int can_continue;
    int can_pause;
    int can_step;
    int can_stop;
} UmiDebugOrchestrationSnapshot;

/**
 * Represent the debug orchestration data shared with callers of this public contract.
 */
typedef struct UmiDebugOrchestration UmiDebugOrchestration;

/**
 * Initialise debug orchestration from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_debug_orchestration_create(
    UmiDebugService *service,
    UmiDebugController *controller,
    UmiDebugWorkspace *workspace,
    UmiDebugOrchestration **out_orchestration);
/**
 * Release or reset state held by debug orchestration so the same storage can be reused
 * safely.
 */
void umi_debug_orchestration_destroy(UmiDebugOrchestration *orchestration);
/**
 * Provide the debug orchestration select configuration operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_orchestration_select_configuration(
    UmiDebugOrchestration *orchestration,
    const UmiDebugConfigurationRequest *request);
/**
 * Provide the debug orchestration select session operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_orchestration_select_session(
    UmiDebugOrchestration *orchestration,
    const char *session_id);
/**
 * Provide the debug orchestration refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_orchestration_refresh(
    UmiDebugOrchestration *orchestration,
    const UmiDebugBreakpointFilter *breakpoint_filter,
    const UmiDebugWatchFilter *watch_filter,
    const UmiDebugConsoleFilter *console_filter);
/**
 * Provide the debug orchestration command enabled operation used by this module and its
 * client applications.
 */
int umi_debug_orchestration_command_enabled(
    const UmiDebugOrchestration *orchestration,
    UmiDebugCommandKind command);
/**
 * Provide the debug orchestration snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_orchestration_snapshot(
    UmiDebugOrchestration *orchestration,
    UmiDebugOrchestrationSnapshot *out_snapshot);
/**
 * Provide the debug orchestration resolver operation used by this module and its client
 * applications.
 */
UmiDebugConfigurationResolver *umi_debug_orchestration_resolver(
    UmiDebugOrchestration *orchestration);
/**
 * Provide the debug orchestration breakpoints operation used by this module and its client
 * applications.
 */
UmiDebugBreakpointQuery *umi_debug_orchestration_breakpoints(
    UmiDebugOrchestration *orchestration);
/**
 * Provide the debug orchestration watches operation used by this module and its client
 * applications.
 */
UmiDebugWatchQuery *umi_debug_orchestration_watches(
    UmiDebugOrchestration *orchestration);
/**
 * Provide the debug orchestration console operation used by this module and its client
 * applications.
 */
UmiDebugConsoleQuery *umi_debug_orchestration_console(
    UmiDebugOrchestration *orchestration);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEBUG_ORCHESTRATION_H */
