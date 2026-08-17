/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/inspection_session.h
 *
 * PURPOSE:
 *   Coordinate advanced debugger inspection state for one selected Framework
 *   debug session: threads, frames, scopes, variables, registers, memory,
 *   disassembly, advanced breakpoints and adapter requests.
 *
 * ARCHITECTURE:
 *   The established UmiDebugService remains the authority for ordinary debug
 *   records. The inspection session owns only derived/advanced models and uses
 *   a registered adapter through the stable C contract. Application code does
 *   not parse debugger protocols or mutate debuggee state directly.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_INSPECTION_SESSION_H
#define UMICOM_DEBUG_INSPECTION_SESSION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/debug/adapter_contract.h"
#include "umicom/debug/advanced_breakpoint.h"
#include "umicom/debug/command.h"
#include "umicom/debug/disassembly_view.h"
#include "umicom/debug/memory_view.h"
#include "umicom/debug/register_bank.h"
#include "umicom/debug/service.h"
#include "umicom/debug/thread_inspector.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_INSPECTION_SESSION_API_VERSION 1U
#define UMI_DEBUG_INSPECTION_SESSION_ID_CAPACITY 128U
#define UMI_DEBUG_INSPECTION_ADAPTER_ID_CAPACITY 128U
#define UMI_DEBUG_INSPECTION_MESSAGE_CAPACITY 512U

typedef enum UmiDebugInspectionState {
    UMI_DEBUG_INSPECTION_UNBOUND = 0,
    UMI_DEBUG_INSPECTION_READY = 1,
    UMI_DEBUG_INSPECTION_RUNNING = 2,
    UMI_DEBUG_INSPECTION_PAUSED = 3,
    UMI_DEBUG_INSPECTION_BUSY = 4,
    UMI_DEBUG_INSPECTION_CANCELLED = 5,
    UMI_DEBUG_INSPECTION_FAILED = 6
} UmiDebugInspectionState;

typedef struct UmiDebugInspectionSessionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char session_id[UMI_DEBUG_INSPECTION_SESSION_ID_CAPACITY];
    char adapter_id[UMI_DEBUG_INSPECTION_ADAPTER_ID_CAPACITY];
    UmiDebugInspectionState state;
    UmiStatus last_status;
    uint64_t advertised_capabilities;
    uint64_t active_request_id;
    uint64_t last_completed_request_id;
    size_t thread_count;
    size_t frame_count;
    size_t scope_count;
    size_t variable_count;
    size_t register_count;
    size_t memory_block_count;
    size_t instruction_count;
    size_t advanced_breakpoint_count;
    uint64_t revision;
    int bound;
    int can_read_memory;
    int can_write_memory;
    int can_read_registers;
    int can_disassemble;
    int can_step_back;
    int can_reverse_continue;
    int can_data_breakpoint;
    int can_instruction_breakpoint;
    char last_message[UMI_DEBUG_INSPECTION_MESSAGE_CAPACITY];
} UmiDebugInspectionSessionSnapshot;

typedef struct UmiDebugInspectionSession UmiDebugInspectionSession;

UmiStatus umi_debug_inspection_session_create(
    UmiDebugService *service,
    UmiDebugAdapterRegistry *adapters,
    UmiDebugInspectionSession **out_session);
void umi_debug_inspection_session_destroy(UmiDebugInspectionSession *session);
UmiStatus umi_debug_inspection_session_bind(
    UmiDebugInspectionSession *session,
    const char *debug_session_id,
    const char *adapter_id,
    uint64_t advertised_capabilities);
UmiStatus umi_debug_inspection_session_unbind(
    UmiDebugInspectionSession *session);
UmiStatus umi_debug_inspection_session_set_state(
    UmiDebugInspectionSession *session,
    UmiDebugInspectionState state);
UmiStatus umi_debug_inspection_session_refresh_threads(
    UmiDebugInspectionSession *session);
UmiStatus umi_debug_inspection_session_invoke(
    UmiDebugInspectionSession *session,
    const UmiDebugAdapterRequest *request,
    const UmiDebugAdapterCancellation *cancellation,
    UmiDebugAdapterResponse *out_response);
UmiStatus umi_debug_inspection_session_cancel_active(
    UmiDebugInspectionSession *session);
UmiStatus umi_debug_inspection_session_write_memory(
    UmiDebugInspectionSession *session,
    const UmiDebugMemoryWritePlan *plan,
    const UmiDebugAdapterCancellation *cancellation,
    UmiDebugAdapterResponse *out_response);
int umi_debug_inspection_session_command_enabled(
    const UmiDebugInspectionSession *session,
    UmiDebugCommandKind command_kind);
UmiDebugThreadInspector *umi_debug_inspection_session_threads(
    UmiDebugInspectionSession *session);
UmiDebugRegisterBank *umi_debug_inspection_session_registers(
    UmiDebugInspectionSession *session);
UmiDebugMemoryView *umi_debug_inspection_session_memory(
    UmiDebugInspectionSession *session);
UmiDebugDisassemblyView *umi_debug_inspection_session_disassembly(
    UmiDebugInspectionSession *session);
UmiDebugAdvancedBreakpointRegistry *
umi_debug_inspection_session_advanced_breakpoints(
    UmiDebugInspectionSession *session);
const UmiDebugCapabilitySet *umi_debug_inspection_session_capabilities(
    const UmiDebugInspectionSession *session);
UmiStatus umi_debug_inspection_session_snapshot(
    const UmiDebugInspectionSession *session,
    UmiDebugInspectionSessionSnapshot *out_snapshot);
uint64_t umi_debug_inspection_session_revision(
    const UmiDebugInspectionSession *session);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEBUG_INSPECTION_SESSION_H */
