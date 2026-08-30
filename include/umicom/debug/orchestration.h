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

typedef struct UmiDebugOrchestration UmiDebugOrchestration;

UmiStatus umi_debug_orchestration_create(
    UmiDebugService *service,
    UmiDebugController *controller,
    UmiDebugWorkspace *workspace,
    UmiDebugOrchestration **out_orchestration);
void umi_debug_orchestration_destroy(UmiDebugOrchestration *orchestration);
UmiStatus umi_debug_orchestration_select_configuration(
    UmiDebugOrchestration *orchestration,
    const UmiDebugConfigurationRequest *request);
UmiStatus umi_debug_orchestration_select_session(
    UmiDebugOrchestration *orchestration,
    const char *session_id);
UmiStatus umi_debug_orchestration_refresh(
    UmiDebugOrchestration *orchestration,
    const UmiDebugBreakpointFilter *breakpoint_filter,
    const UmiDebugWatchFilter *watch_filter,
    const UmiDebugConsoleFilter *console_filter);
int umi_debug_orchestration_command_enabled(
    const UmiDebugOrchestration *orchestration,
    UmiDebugCommandKind command);
UmiStatus umi_debug_orchestration_snapshot(
    UmiDebugOrchestration *orchestration,
    UmiDebugOrchestrationSnapshot *out_snapshot);
UmiDebugConfigurationResolver *umi_debug_orchestration_resolver(
    UmiDebugOrchestration *orchestration);
UmiDebugBreakpointQuery *umi_debug_orchestration_breakpoints(
    UmiDebugOrchestration *orchestration);
UmiDebugWatchQuery *umi_debug_orchestration_watches(
    UmiDebugOrchestration *orchestration);
UmiDebugConsoleQuery *umi_debug_orchestration_console(
    UmiDebugOrchestration *orchestration);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEBUG_ORCHESTRATION_H */
