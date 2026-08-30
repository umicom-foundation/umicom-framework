/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/command.h
 *
 * PURPOSE:
 *   Publish stable command identities for professional debugger workflows.
 *   The catalogue includes advanced breakpoints, thread/frame inspection,
 *   reverse execution, registers,
 *   memory, disassembly, modules and adapter management.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_COMMAND_H
#define UMICOM_DEBUG_COMMAND_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_COMMAND_API_VERSION 1U

typedef enum UmiDebugCommandKind {
    UMI_DEBUG_COMMAND_START = 1,
    UMI_DEBUG_COMMAND_START_WITHOUT_DEBUGGING = 2,
    UMI_DEBUG_COMMAND_SELECT_CONFIGURATION = 3,
    UMI_DEBUG_COMMAND_OPEN_CONFIGURATION = 4,
    UMI_DEBUG_COMMAND_RESTART = 5,
    UMI_DEBUG_COMMAND_STOP = 6,
    UMI_DEBUG_COMMAND_CONTINUE = 7,
    UMI_DEBUG_COMMAND_PAUSE = 8,
    UMI_DEBUG_COMMAND_STEP_OVER = 9,
    UMI_DEBUG_COMMAND_STEP_INTO = 10,
    UMI_DEBUG_COMMAND_STEP_OUT = 11,
    UMI_DEBUG_COMMAND_RUN_TO_CURSOR = 12,
    UMI_DEBUG_COMMAND_TOGGLE_BREAKPOINT = 13,
    UMI_DEBUG_COMMAND_ENABLE_ALL_BREAKPOINTS = 14,
    UMI_DEBUG_COMMAND_DISABLE_ALL_BREAKPOINTS = 15,
    UMI_DEBUG_COMMAND_REMOVE_ALL_BREAKPOINTS = 16,
    UMI_DEBUG_COMMAND_ADD_CONDITIONAL_BREAKPOINT = 17,
    UMI_DEBUG_COMMAND_ADD_LOGPOINT = 18,
    UMI_DEBUG_COMMAND_ADD_WATCH = 19,
    UMI_DEBUG_COMMAND_REMOVE_WATCH = 20,
    UMI_DEBUG_COMMAND_EVALUATE_SELECTION = 21,
    UMI_DEBUG_COMMAND_OPEN_CONSOLE = 22,
    UMI_DEBUG_COMMAND_CLEAR_CONSOLE = 23,
    UMI_DEBUG_COMMAND_COPY_CALL_STACK = 24,
    UMI_DEBUG_COMMAND_OPEN_DISASSEMBLY = 25,
    UMI_DEBUG_COMMAND_OPEN_MEMORY = 26,
    UMI_DEBUG_COMMAND_ADD_FUNCTION_BREAKPOINT = 27,
    UMI_DEBUG_COMMAND_ADD_DATA_BREAKPOINT = 28,
    UMI_DEBUG_COMMAND_ADD_INSTRUCTION_BREAKPOINT = 29,
    UMI_DEBUG_COMMAND_CONFIGURE_EXCEPTION_BREAKPOINTS = 30,
    UMI_DEBUG_COMMAND_OPEN_ADVANCED_BREAKPOINTS = 31,
    UMI_DEBUG_COMMAND_OPEN_THREADS = 32,
    UMI_DEBUG_COMMAND_SELECT_THREAD = 33,
    UMI_DEBUG_COMMAND_SELECT_FRAME = 34,
    UMI_DEBUG_COMMAND_REFRESH_THREADS = 35,
    UMI_DEBUG_COMMAND_REFRESH_VARIABLES = 36,
    UMI_DEBUG_COMMAND_SET_VARIABLE = 37,
    UMI_DEBUG_COMMAND_SET_EXPRESSION = 38,
    UMI_DEBUG_COMMAND_STEP_BACK = 39,
    UMI_DEBUG_COMMAND_REVERSE_CONTINUE = 40,
    UMI_DEBUG_COMMAND_RESTART_FRAME = 41,
    UMI_DEBUG_COMMAND_OPEN_MODULES = 42,
    UMI_DEBUG_COMMAND_OPEN_REGISTERS = 43,
    UMI_DEBUG_COMMAND_REFRESH_REGISTERS = 44,
    UMI_DEBUG_COMMAND_REFRESH_MEMORY = 45,
    UMI_DEBUG_COMMAND_WRITE_MEMORY = 46,
    UMI_DEBUG_COMMAND_REFRESH_DISASSEMBLY = 47,
    UMI_DEBUG_COMMAND_TOGGLE_INSTRUCTION_BREAKPOINT = 48,
    UMI_DEBUG_COMMAND_OPEN_ADAPTER_CENTRE = 49,
    UMI_DEBUG_COMMAND_COPY_MEMORY_ADDRESS = 50
} UmiDebugCommandKind;

typedef struct UmiDebugCommandDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    UmiDebugCommandKind kind;
    const char *id;
    const char *label;
    const char *default_keybinding;
    int requires_configuration;
    int requires_active_session;
    int requires_paused_session;
} UmiDebugCommandDescriptor;

size_t umi_debug_command_count(void);
const UmiDebugCommandDescriptor *umi_debug_command_at(size_t index);
const UmiDebugCommandDescriptor *umi_debug_command_find(const char *command_id);
const UmiDebugCommandDescriptor *umi_debug_command_for_kind(
    UmiDebugCommandKind kind);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEBUG_COMMAND_H */
