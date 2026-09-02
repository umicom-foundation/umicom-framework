/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/types.h
 *
 * PURPOSE:
 *   Define stable debugger-workbench identifiers, source locations, phases, commands and shared value types.
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
#ifndef UMICOM_DEBUG_WORKBENCH_TYPES_H
#define UMICOM_DEBUG_WORKBENCH_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_WORKBENCH_ID_CAPACITY 128U
#define UMI_DEBUG_WORKBENCH_PATH_CAPACITY 320U
#define UMI_DEBUG_WORKBENCH_TEXT_CAPACITY 256U
#define UMI_DEBUG_WORKBENCH_DETAIL_CAPACITY 384U
#define UMI_DEBUG_WORKBENCH_MAX_ITEMS 64U
#define UMI_DEBUG_WORKBENCH_NO_INDEX UMI_DEBUG_WORKBENCH_MAX_ITEMS

/**
 * List the named debug workbench session phase values accepted by this public contract.
 */
typedef enum UmiDebugWorkbenchSessionPhase {
    UMI_DEBUG_WORKBENCH_SESSION_IDLE = 0,
    UMI_DEBUG_WORKBENCH_SESSION_INITIALIZING = 1,
    UMI_DEBUG_WORKBENCH_SESSION_RUNNING = 2,
    UMI_DEBUG_WORKBENCH_SESSION_PAUSED = 3,
    UMI_DEBUG_WORKBENCH_SESSION_STOPPED = 4,
    UMI_DEBUG_WORKBENCH_SESSION_TERMINATED = 5,
    UMI_DEBUG_WORKBENCH_SESSION_FAILED = 6
} UmiDebugWorkbenchSessionPhase;

/**
 * List the named debug workbench breakpoint kind values accepted by this public contract.
 */
typedef enum UmiDebugWorkbenchBreakpointKind {
    UMI_DEBUG_WORKBENCH_BREAKPOINT_SOURCE = 1,
    UMI_DEBUG_WORKBENCH_BREAKPOINT_FUNCTION = 2,
    UMI_DEBUG_WORKBENCH_BREAKPOINT_DATA = 3,
    UMI_DEBUG_WORKBENCH_BREAKPOINT_EXCEPTION = 4,
    UMI_DEBUG_WORKBENCH_BREAKPOINT_INSTRUCTION = 5,
    UMI_DEBUG_WORKBENCH_BREAKPOINT_LOGPOINT = 6
} UmiDebugWorkbenchBreakpointKind;

/**
 * List the named debug workbench command values accepted by this public contract.
 */
typedef enum UmiDebugWorkbenchCommand {
    UMI_DEBUG_WORKBENCH_COMMAND_START = 0,
    UMI_DEBUG_WORKBENCH_COMMAND_ATTACH = 1,
    UMI_DEBUG_WORKBENCH_COMMAND_PAUSE = 2,
    UMI_DEBUG_WORKBENCH_COMMAND_CONTINUE = 3,
    UMI_DEBUG_WORKBENCH_COMMAND_STEP_OVER = 4,
    UMI_DEBUG_WORKBENCH_COMMAND_STEP_IN = 5,
    UMI_DEBUG_WORKBENCH_COMMAND_STEP_OUT = 6,
    UMI_DEBUG_WORKBENCH_COMMAND_RESTART = 7,
    UMI_DEBUG_WORKBENCH_COMMAND_STOP = 8,
    UMI_DEBUG_WORKBENCH_COMMAND_REVERSE_CONTINUE = 9,
    UMI_DEBUG_WORKBENCH_COMMAND_STEP_BACK = 10,
    UMI_DEBUG_WORKBENCH_COMMAND_COUNT = 11
} UmiDebugWorkbenchCommand;

/**
 * List the named debug workbench view kind values accepted by this public contract.
 */
typedef enum UmiDebugWorkbenchViewKind {
    UMI_DEBUG_WORKBENCH_VIEW_BREAKPOINTS = 1,
    UMI_DEBUG_WORKBENCH_VIEW_THREADS = 2,
    UMI_DEBUG_WORKBENCH_VIEW_CALL_STACK = 3,
    UMI_DEBUG_WORKBENCH_VIEW_VARIABLES = 4,
    UMI_DEBUG_WORKBENCH_VIEW_WATCH = 5,
    UMI_DEBUG_WORKBENCH_VIEW_CONSOLE = 6,
    UMI_DEBUG_WORKBENCH_VIEW_MEMORY = 7,
    UMI_DEBUG_WORKBENCH_VIEW_REGISTERS = 8,
    UMI_DEBUG_WORKBENCH_VIEW_DISASSEMBLY = 9,
    UMI_DEBUG_WORKBENCH_VIEW_MODULES = 10,
    UMI_DEBUG_WORKBENCH_VIEW_SOURCES = 11,
    UMI_DEBUG_WORKBENCH_VIEW_TIMELINE = 12
} UmiDebugWorkbenchViewKind;

/**
 * Represent the debug workbench position data shared with callers of this public contract.
 */
typedef struct UmiDebugWorkbenchPosition {
    uint32_t line;
    uint32_t column;
} UmiDebugWorkbenchPosition;

/**
 * Represent the debug workbench range data shared with callers of this public contract.
 */
typedef struct UmiDebugWorkbenchRange {
    UmiDebugWorkbenchPosition start;
    UmiDebugWorkbenchPosition end;
} UmiDebugWorkbenchRange;

/**
 * Represent the debug workbench location data shared with callers of this public contract.
 */
typedef struct UmiDebugWorkbenchLocation {
    char path[UMI_DEBUG_WORKBENCH_PATH_CAPACITY];
    UmiDebugWorkbenchRange range;
} UmiDebugWorkbenchLocation;

/**
 * Represent the debug workbench entry data shared with callers of this public contract.
 */
typedef struct UmiDebugWorkbenchEntry {
    char id[UMI_DEBUG_WORKBENCH_ID_CAPACITY];
    char label[UMI_DEBUG_WORKBENCH_TEXT_CAPACITY];
    char detail[UMI_DEBUG_WORKBENCH_DETAIL_CAPACITY];
    UmiDebugWorkbenchLocation location;
    uint32_t state;
    uint32_t flags;
    uint64_t value;
    uint64_t revision;
} UmiDebugWorkbenchEntry;

/**
 * Provide the debug workbench copy text operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_workbench_copy_text(char *destination, size_t capacity, const char *source);
/**
 * Check that debug workbench id satisfies its contract before another service relies on
 * it.
 */
int umi_debug_workbench_id_valid(const char *identifier);
/**
 * Provide the debug workbench position compare operation used by this module and its
 * client applications.
 */
int umi_debug_workbench_position_compare(UmiDebugWorkbenchPosition left, UmiDebugWorkbenchPosition right);
/**
 * Provide the debug workbench range normalize operation used by this module and its client
 * applications.
 */
UmiDebugWorkbenchRange umi_debug_workbench_range_normalize(UmiDebugWorkbenchRange range);
/**
 * Initialise debug workbench location from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_workbench_location_init(UmiDebugWorkbenchLocation *location, const char *path, UmiDebugWorkbenchRange range);
/**
 * Initialise debug workbench entry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_debug_workbench_entry_init(UmiDebugWorkbenchEntry *entry, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Check that debug workbench entry satisfies its contract before another service relies on
 * it.
 */
int umi_debug_workbench_entry_valid(const UmiDebugWorkbenchEntry *entry);
/**
 * Provide the debug workbench session transition allowed operation used by this module and
 * its client applications.
 */
int umi_debug_workbench_session_transition_allowed(UmiDebugWorkbenchSessionPhase from, UmiDebugWorkbenchSessionPhase to);
/**
 * Provide the debug workbench command bit operation used by this module and its client
 * applications.
 */
uint64_t umi_debug_workbench_command_bit(UmiDebugWorkbenchCommand command);

#ifdef __cplusplus
}
#endif
#endif
