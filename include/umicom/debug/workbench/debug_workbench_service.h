/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_workbench_service.h
 *
 * PURPOSE:
 *   Compose canonical debug runtime state into one toolkit-neutral debugger workbench service.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical Debug Service/DAP
 *   runtime state; Studio remains a thin frontend and owns no reusable debug
 *   semantics, adapter protocol, breakpoint engine or inspection engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_WORKBENCH_SERVICE_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_WORKBENCH_SERVICE_H

#include "umicom/debug/workbench/debug_session_registry.h"
#include "umicom/debug/workbench/breakpoint_collection.h"
#include "umicom/debug/workbench/thread_group.h"
#include "umicom/debug/workbench/watch_expression_collection.h"
#include "umicom/debug/workbench/debug_status_model.h"
#include "umicom/debug/register_bank.h"
#include "umicom/debug/disassembly_view.h"
#include "umicom/debug_runtime/platform.h"
#include "umicom/ui/view_model.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_WORKBENCH_LOW_LEVEL_ROW_LIMIT 24U
#define UMI_DEBUG_WORKBENCH_LOW_LEVEL_INSTRUCTION_LIMIT 128U

/**
 * Represent the debug workbench debug workbench service data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchDebugWorkbenchService {
    UmiDebugWorkbenchDebugSessionRegistry sessions;
    UmiDebugWorkbenchBreakpointCollection breakpoints;
    UmiDebugWorkbenchThreadGroup threads;
    UmiDebugWorkbenchWatchExpressionCollection watches;
    UmiDebugWorkbenchDebugStatusModel status;
    bool status_initialized;
    uint64_t revision;
} UmiDebugWorkbenchDebugWorkbenchService;

/**
 * Initialise debug workbench debug workbench service from caller-provided values so later
 * operations receive a known state.
 */
void umi_debug_workbench_debug_workbench_service_init(UmiDebugWorkbenchDebugWorkbenchService *service);
/**
 * Provide the debug workbench debug workbench service open session operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_workbench_service_open_session(UmiDebugWorkbenchDebugWorkbenchService *service, const UmiDebugWorkbenchEntry *session);
/**
 * Provide the debug workbench debug workbench service select session operation used by
 * this module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_workbench_service_select_session(UmiDebugWorkbenchDebugWorkbenchService *service, const char *session_id);
/**
 * Provide the debug workbench debug workbench service refresh status operation used by
 * this module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_workbench_service_refresh_status(UmiDebugWorkbenchDebugWorkbenchService *service, UmiDebugWorkbenchSessionPhase phase, const char *stop_reason, uint32_t frame_count, uint32_t variable_count);
/**
 * Check that debug workbench debug workbench service satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_workbench_service_valid(const UmiDebugWorkbenchDebugWorkbenchService *service);

/**
 * Find the program-counter register in a Framework register bank and copy a bounded
 * debugger memory reference suitable for a DAP disassemble request.  Common names used by
 * x86/x86-64, ARM and RISC-V adapters are recognised.  The register model remains owned by
 * the caller.
 */
UmiStatus umi_debug_workbench_program_counter_reference(
    const UmiDebugRegisterBank *bank,
    char *out_reference,
    size_t capacity);

/**
 * Refresh the active stopped session's architecture registers and, where the adapter
 * supports disassembly and exposes a program counter, refresh a bounded instruction window.
 * The operation uses Framework's existing DAP runtime, register bank and disassembly model;
 * no debugger protocol is implemented by the consuming application.
 */
UmiStatus umi_debug_workbench_refresh_low_level(
    UmiDebugRuntimePlatform *platform,
    uint32_t timeout_ms,
    uint32_t instruction_count);

/**
 * Copy the selected disassembly instruction's adapter memory reference.  When
 * no explicit selection exists, the current instruction is used.  This is the
 * canonical address source for instruction stepping and instruction breakpoints.
 */
UmiStatus umi_debug_workbench_instruction_reference(
    const UmiDebugDisassemblyView *disassembly,
    char *out_reference,
    size_t capacity);

/**
 * Advance the active stopped thread by one machine instruction.  step_into=0
 * sends DAP next with instruction granularity; step_into!=0 sends DAP stepIn
 * with instruction granularity.  The accepted request changes the adapter and
 * inspection session to running; the normal debugger event pump publishes the
 * following stopped event.
 */
UmiStatus umi_debug_workbench_step_instruction(
    UmiDebugRuntimePlatform *platform,
    int step_into,
    uint32_t timeout_ms);

/**
 * Toggle an instruction breakpoint for the selected (or current) disassembly
 * row and synchronise the complete enabled instruction-breakpoint set with the
 * active DAP adapter.  The Framework registry is changed only after the adapter
 * accepts the replacement set.
 */
UmiStatus umi_debug_workbench_toggle_instruction_breakpoint(
    UmiDebugRuntimePlatform *platform,
    uint32_t timeout_ms,
    int *out_enabled);

/**
 * Project a Framework register bank into a toolkit-neutral view model suitable for Studio,
 * headless tests and future frontends.
 */
UmiStatus umi_debug_workbench_registers_view_create(
    const char *view_id,
    const UmiDebugRegisterBank *bank,
    UmiUiViewModel **out_view);

/**
 * Project a Framework disassembly model into a toolkit-neutral view model suitable for
 * Studio, headless tests and future frontends.
 */
UmiStatus umi_debug_workbench_disassembly_view_create(
    const char *view_id,
    const UmiDebugDisassemblyView *disassembly,
    UmiUiViewModel **out_view);

#ifdef __cplusplus
}
#endif
#endif
