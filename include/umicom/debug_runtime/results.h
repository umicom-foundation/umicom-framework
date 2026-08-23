/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/results.h
 *
 * PURPOSE:
 *   Define bounded decoded DAP response collections before projection into the
 *   existing Umicom Debug Service and advanced inspection models.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_RESULTS_H
#define UMICOM_DEBUG_RUNTIME_RESULTS_H
#include "umicom/debug_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugRuntimeBreakpointList {
    UmiDebugRuntimeBreakpoint items[UMI_DEBUG_RUNTIME_MAX_BREAKPOINTS];
    size_t count;
} UmiDebugRuntimeBreakpointList;

typedef struct UmiDebugRuntimeThreadList {
    UmiDebugRuntimeThread items[UMI_DEBUG_RUNTIME_MAX_ITEMS];
    size_t count;
} UmiDebugRuntimeThreadList;

typedef struct UmiDebugRuntimeStackTrace {
    UmiDebugRuntimeStackFrame items[UMI_DEBUG_RUNTIME_MAX_ITEMS];
    size_t count;
    size_t total_frames;
} UmiDebugRuntimeStackTrace;

typedef struct UmiDebugRuntimeScopeList {
    UmiDebugRuntimeScope items[UMI_DEBUG_RUNTIME_MAX_ITEMS];
    size_t count;
} UmiDebugRuntimeScopeList;

typedef struct UmiDebugRuntimeVariableList {
    UmiDebugRuntimeVariable items[UMI_DEBUG_RUNTIME_MAX_VARIABLES];
    size_t count;
} UmiDebugRuntimeVariableList;

typedef struct UmiDebugRuntimeEvaluateResult {
    char result[UMI_DEBUG_RUNTIME_TEXT_CAPACITY];
    char type[UMI_DEBUG_RUNTIME_NAME_CAPACITY];
    uint64_t variables_reference;
    uint32_t named_variables;
    uint32_t indexed_variables;
    char memory_reference[128];
} UmiDebugRuntimeEvaluateResult;

typedef struct UmiDebugRuntimeModuleList {
    UmiDebugRuntimeModule items[UMI_DEBUG_RUNTIME_MAX_ITEMS];
    size_t count;
    size_t total_modules;
} UmiDebugRuntimeModuleList;

typedef struct UmiDebugRuntimeMemoryResult {
    char address[128];
    size_t unreadable_bytes;
    unsigned char bytes[UMI_DEBUG_RUNTIME_MAX_MEMORY_BYTES];
    size_t byte_count;
} UmiDebugRuntimeMemoryResult;

typedef struct UmiDebugRuntimeDisassembly {
    UmiDebugRuntimeInstruction items[UMI_DEBUG_RUNTIME_MAX_INSTRUCTIONS];
    size_t count;
} UmiDebugRuntimeDisassembly;

typedef struct UmiDebugRuntimeSourceResult {
    char content[UMI_DEBUG_RUNTIME_JSON_CAPACITY];
    char mime_type[128];
} UmiDebugRuntimeSourceResult;

typedef struct UmiDebugRuntimeExceptionInfo {
    char exception_id[256];
    char description[UMI_DEBUG_RUNTIME_TEXT_CAPACITY];
    char break_mode[64];
    char details[UMI_DEBUG_RUNTIME_TEXT_CAPACITY];
} UmiDebugRuntimeExceptionInfo;

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

typedef struct UmiDebugRuntimeCompletionList {
    UmiDebugRuntimeCompletionTarget items[UMI_DEBUG_RUNTIME_MAX_ITEMS];
    size_t count;
} UmiDebugRuntimeCompletionList;

typedef struct UmiDebugRuntimeDataBreakpointInfo {
    char data_id[512];
    char description[1024];
    char access_types[512];
    int can_persist;
} UmiDebugRuntimeDataBreakpointInfo;

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
