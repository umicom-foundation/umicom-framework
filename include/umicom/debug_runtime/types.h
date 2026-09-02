/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/types.h
 *
 * PURPOSE:
 *   Define bounded Debug Adapter Protocol runtime primitives. Existing
 *   umicom/debug models remain authoritative for product-neutral debugger state;
 *   these records represent protocol/process data before it is projected there.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_TYPES_H
#define UMICOM_DEBUG_RUNTIME_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_RUNTIME_API_VERSION 1U
#define UMI_DEBUG_RUNTIME_ID_CAPACITY 192U
#define UMI_DEBUG_RUNTIME_NAME_CAPACITY 256U
#define UMI_DEBUG_RUNTIME_PATH_CAPACITY 2048U
#define UMI_DEBUG_RUNTIME_TEXT_CAPACITY 4096U
#define UMI_DEBUG_RUNTIME_JSON_CAPACITY 65536U
#define UMI_DEBUG_RUNTIME_FRAME_CAPACITY 131072U
#define UMI_DEBUG_RUNTIME_MAX_ADAPTERS 16U
#define UMI_DEBUG_RUNTIME_MAX_PENDING 256U
#define UMI_DEBUG_RUNTIME_MAX_EVENTS 64U
#define UMI_DEBUG_RUNTIME_MAX_DEFERRED_RESPONSES 32U
#define UMI_DEBUG_RUNTIME_MAX_ITEMS 512U
#define UMI_DEBUG_RUNTIME_MAX_BREAKPOINTS 256U
#define UMI_DEBUG_RUNTIME_MAX_VARIABLES 1024U
#define UMI_DEBUG_RUNTIME_MAX_INSTRUCTIONS 1024U
#define UMI_DEBUG_RUNTIME_MAX_MEMORY_BYTES 65536U

/**
 * List the named debug runtime message kind values accepted by this public contract.
 */
typedef enum UmiDebugRuntimeMessageKind {
    UMI_DEBUG_RUNTIME_MESSAGE_UNKNOWN = 0,
    UMI_DEBUG_RUNTIME_MESSAGE_REQUEST = 1,
    UMI_DEBUG_RUNTIME_MESSAGE_RESPONSE = 2,
    UMI_DEBUG_RUNTIME_MESSAGE_EVENT = 3
} UmiDebugRuntimeMessageKind;

/**
 * List the named debug runtime adapter state values accepted by this public contract.
 */
typedef enum UmiDebugRuntimeAdapterState {
    UMI_DEBUG_RUNTIME_ADAPTER_STOPPED = 0,
    UMI_DEBUG_RUNTIME_ADAPTER_STARTING = 1,
    UMI_DEBUG_RUNTIME_ADAPTER_INITIALIZING = 2,
    UMI_DEBUG_RUNTIME_ADAPTER_READY = 3,
    UMI_DEBUG_RUNTIME_ADAPTER_RUNNING = 4,
    UMI_DEBUG_RUNTIME_ADAPTER_PAUSED = 5,
    UMI_DEBUG_RUNTIME_ADAPTER_STOPPING = 6,
    UMI_DEBUG_RUNTIME_ADAPTER_FAILED = 7
} UmiDebugRuntimeAdapterState;

/**
 * Represent the debug runtime envelope data shared with callers of this public contract.
 */
typedef struct UmiDebugRuntimeEnvelope {
    UmiDebugRuntimeMessageKind kind;
    uint64_t sequence;
    uint64_t request_sequence;
    char command[UMI_DEBUG_RUNTIME_NAME_CAPACITY];
    char event[UMI_DEBUG_RUNTIME_NAME_CAPACITY];
    int success;
    char message[UMI_DEBUG_RUNTIME_TEXT_CAPACITY];
    char json[UMI_DEBUG_RUNTIME_JSON_CAPACITY];
} UmiDebugRuntimeEnvelope;

/**
 * Represent the debug runtime source data shared with callers of this public contract.
 */
typedef struct UmiDebugRuntimeSource {
    char name[UMI_DEBUG_RUNTIME_NAME_CAPACITY];
    char path[UMI_DEBUG_RUNTIME_PATH_CAPACITY];
    uint64_t source_reference;
} UmiDebugRuntimeSource;

/**
 * Represent the debug runtime breakpoint data shared with callers of this public contract.
 */
typedef struct UmiDebugRuntimeBreakpoint {
    uint64_t id;
    int verified;
    char message[UMI_DEBUG_RUNTIME_TEXT_CAPACITY];
    UmiDebugRuntimeSource source;
    uint32_t line;
    uint32_t column;
    char instruction_reference[128];
    int64_t offset;
} UmiDebugRuntimeBreakpoint;

/**
 * Represent the debug runtime thread data shared with callers of this public contract.
 */
typedef struct UmiDebugRuntimeThread {
    uint64_t id;
    char name[UMI_DEBUG_RUNTIME_NAME_CAPACITY];
} UmiDebugRuntimeThread;

/**
 * Represent the debug runtime stack frame data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugRuntimeStackFrame {
    uint64_t id;
    char name[UMI_DEBUG_RUNTIME_NAME_CAPACITY];
    UmiDebugRuntimeSource source;
    uint32_t line;
    uint32_t column;
    char instruction_pointer[128];
    char presentation_hint[64];
} UmiDebugRuntimeStackFrame;

/**
 * Represent the debug runtime scope data shared with callers of this public contract.
 */
typedef struct UmiDebugRuntimeScope {
    char name[UMI_DEBUG_RUNTIME_NAME_CAPACITY];
    uint64_t variables_reference;
    uint32_t named_variables;
    uint32_t indexed_variables;
    int expensive;
    char presentation_hint[64];
} UmiDebugRuntimeScope;

/**
 * Represent the debug runtime variable data shared with callers of this public contract.
 */
typedef struct UmiDebugRuntimeVariable {
    char name[UMI_DEBUG_RUNTIME_NAME_CAPACITY];
    char value[UMI_DEBUG_RUNTIME_TEXT_CAPACITY];
    char type[UMI_DEBUG_RUNTIME_NAME_CAPACITY];
    char evaluate_name[1024];
    uint64_t variables_reference;
    uint32_t named_variables;
    uint32_t indexed_variables;
    char memory_reference[128];
} UmiDebugRuntimeVariable;

/**
 * Represent the debug runtime module data shared with callers of this public contract.
 */
typedef struct UmiDebugRuntimeModule {
    char id[UMI_DEBUG_RUNTIME_ID_CAPACITY];
    char name[UMI_DEBUG_RUNTIME_NAME_CAPACITY];
    char path[UMI_DEBUG_RUNTIME_PATH_CAPACITY];
    char version[128];
    char symbol_status[256];
    int optimized;
} UmiDebugRuntimeModule;

/**
 * Represent the debug runtime instruction data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugRuntimeInstruction {
    char address[128];
    char instruction_bytes[256];
    char instruction[1024];
    char symbol[256];
    UmiDebugRuntimeSource location;
    uint32_t line;
    uint32_t column;
} UmiDebugRuntimeInstruction;

/**
 * Represent the debug runtime capabilities data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugRuntimeCapabilities {
    int supports_configuration_done;
    int supports_function_breakpoints;
    int supports_conditional_breakpoints;
    int supports_hit_conditions;
    int supports_evaluate_for_hovers;
    int supports_set_variable;
    int supports_restart;
    int supports_terminate_request;
    int supports_modules_request;
    int supports_read_memory_request;
    int supports_disassemble_request;
    int supports_step_back;
    int supports_data_breakpoints;
    int supports_instruction_breakpoints;
    int supports_exception_filter_options;
    int supports_write_memory_request;
    int supports_set_expression;
    int supports_restart_frame;
    int supports_completions_request;
    int supports_cancel_request;
    int supports_loaded_sources_request;
    int supports_terminate_threads_request;
    int supports_goto_targets_request;
} UmiDebugRuntimeCapabilities;

/**
 * Provide the debug runtime message kind text operation used by this module and its client
 * applications.
 */
const char *umi_debug_runtime_message_kind_text(
    UmiDebugRuntimeMessageKind kind);

/**
 * Provide the debug runtime adapter state text operation used by this module and its
 * client applications.
 */
const char *umi_debug_runtime_adapter_state_text(
    UmiDebugRuntimeAdapterState state);

#ifdef __cplusplus
}
#endif
#endif
