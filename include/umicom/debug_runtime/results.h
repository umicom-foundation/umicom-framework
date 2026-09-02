/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/results.h
 *
 * PURPOSE:
 *   Define bounded decoded DAP response collections before projection into the
 *   existing Umicom Debug Service and advanced inspection models.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_RESULTS_H
#define UMICOM_DEBUG_RUNTIME_RESULTS_H
#include "umicom/debug_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug runtime breakpoint list data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugRuntimeBreakpointList {
    UmiDebugRuntimeBreakpoint items[UMI_DEBUG_RUNTIME_MAX_BREAKPOINTS];
    size_t count;
} UmiDebugRuntimeBreakpointList;

/**
 * Represent the debug runtime thread list data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugRuntimeThreadList {
    UmiDebugRuntimeThread items[UMI_DEBUG_RUNTIME_MAX_ITEMS];
    size_t count;
} UmiDebugRuntimeThreadList;

/**
 * Represent the debug runtime stack trace data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugRuntimeStackTrace {
    UmiDebugRuntimeStackFrame items[UMI_DEBUG_RUNTIME_MAX_ITEMS];
    size_t count;
    size_t total_frames;
} UmiDebugRuntimeStackTrace;

/**
 * Represent the debug runtime scope list data shared with callers of this public contract.
 */
typedef struct UmiDebugRuntimeScopeList {
    UmiDebugRuntimeScope items[UMI_DEBUG_RUNTIME_MAX_ITEMS];
    size_t count;
} UmiDebugRuntimeScopeList;

/**
 * Represent the debug runtime variable list data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugRuntimeVariableList {
    UmiDebugRuntimeVariable items[UMI_DEBUG_RUNTIME_MAX_VARIABLES];
    size_t count;
} UmiDebugRuntimeVariableList;

/**
 * Represent the debug runtime evaluate result data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugRuntimeEvaluateResult {
    char result[UMI_DEBUG_RUNTIME_TEXT_CAPACITY];
    char type[UMI_DEBUG_RUNTIME_NAME_CAPACITY];
    uint64_t variables_reference;
    uint32_t named_variables;
    uint32_t indexed_variables;
    char memory_reference[128];
} UmiDebugRuntimeEvaluateResult;

/**
 * Represent the debug runtime module list data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugRuntimeModuleList {
    UmiDebugRuntimeModule items[UMI_DEBUG_RUNTIME_MAX_ITEMS];
    size_t count;
    size_t total_modules;
} UmiDebugRuntimeModuleList;

/**
 * Represent the debug runtime memory result data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugRuntimeMemoryResult {
    char address[128];
    size_t unreadable_bytes;
    unsigned char bytes[UMI_DEBUG_RUNTIME_MAX_MEMORY_BYTES];
    size_t byte_count;
} UmiDebugRuntimeMemoryResult;

/**
 * Represent the debug runtime disassembly data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugRuntimeDisassembly {
    UmiDebugRuntimeInstruction items[UMI_DEBUG_RUNTIME_MAX_INSTRUCTIONS];
    size_t count;
} UmiDebugRuntimeDisassembly;

/**
 * Represent the debug runtime source result data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugRuntimeSourceResult {
    char content[UMI_DEBUG_RUNTIME_JSON_CAPACITY];
    char mime_type[128];
} UmiDebugRuntimeSourceResult;

/**
 * Represent the debug runtime exception info data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugRuntimeExceptionInfo {
    char exception_id[256];
    char description[UMI_DEBUG_RUNTIME_TEXT_CAPACITY];
    char break_mode[64];
    char details[UMI_DEBUG_RUNTIME_TEXT_CAPACITY];
} UmiDebugRuntimeExceptionInfo;

/**
 * Represent the debug runtime completion target data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugRuntimeCompletionTarget {
    char label[256];
    char text[1024];
    char sort_text[256];
    char type[128];
    uint32_t start;
    uint32_t length;
    uint32_t selection_start;
    uint32_t selection_length;
} UmiDebugRuntimeCompletionTarget;

/**
 * Represent the debug runtime completion list data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugRuntimeCompletionList {
    UmiDebugRuntimeCompletionTarget items[UMI_DEBUG_RUNTIME_MAX_ITEMS];
    size_t count;
} UmiDebugRuntimeCompletionList;

/**
 * Represent the debug runtime data breakpoint info data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugRuntimeDataBreakpointInfo {
    char data_id[512];
    char description[1024];
    char access_types[512];
    int can_persist;
} UmiDebugRuntimeDataBreakpointInfo;

/**
 * Represent the debug runtime event data shared with callers of this public contract.
 */
typedef struct UmiDebugRuntimeEvent {
    char event[128];
    char reason[256];
    char description[UMI_DEBUG_RUNTIME_TEXT_CAPACITY];
    char text[UMI_DEBUG_RUNTIME_TEXT_CAPACITY];
    char category[128];
    uint64_t thread_id;
    uint64_t exit_code;
    int all_threads_stopped;
    int all_threads_continued;
    int terminated;
} UmiDebugRuntimeEvent;

#ifdef __cplusplus
}
#endif
#endif
