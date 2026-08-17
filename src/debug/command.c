/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/command.c
 *
 * PURPOSE:
 *   Implement the immutable Framework debugger command catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/command.h"

#include <string.h>

#define COMMAND(kind_value, command_id, command_label, keybinding, config,    \
                session, paused)                                             \
    {                                                                         \
        (uint32_t)sizeof(UmiDebugCommandDescriptor),                          \
        UMI_DEBUG_COMMAND_API_VERSION, kind_value, command_id, command_label,  \
        keybinding, config, session, paused                                   \
    }

static const UmiDebugCommandDescriptor COMMANDS[] = {
    COMMAND(UMI_DEBUG_COMMAND_START, "debug.start", "Start Debugging", "F5",
            1, 0, 0),
    COMMAND(UMI_DEBUG_COMMAND_START_WITHOUT_DEBUGGING,
            "debug.start-without-debugging", "Start Without Debugging",
            "Ctrl+F5", 1, 0, 0),
    COMMAND(UMI_DEBUG_COMMAND_SELECT_CONFIGURATION,
            "debug.configuration.select", "Select Debug Configuration…", "",
            0, 0, 0),
    COMMAND(UMI_DEBUG_COMMAND_OPEN_CONFIGURATION,
            "debug.configuration.open", "Open Debug Configuration", "", 0, 0,
            0),
    COMMAND(UMI_DEBUG_COMMAND_RESTART, "debug.restart", "Restart Debugging",
            "Ctrl+Shift+F5", 1, 1, 0),
    COMMAND(UMI_DEBUG_COMMAND_STOP, "debug.stop", "Stop Debugging", "Shift+F5",
            0, 1, 0),
    COMMAND(UMI_DEBUG_COMMAND_CONTINUE, "debug.continue", "Continue", "F5", 0,
            1, 1),
    COMMAND(UMI_DEBUG_COMMAND_PAUSE, "debug.pause", "Pause", "F6", 0, 1, 0),
    COMMAND(UMI_DEBUG_COMMAND_STEP_OVER, "debug.step-over", "Step Over", "F10",
            0, 1, 1),
    COMMAND(UMI_DEBUG_COMMAND_STEP_INTO, "debug.step-into", "Step Into", "F11",
            0, 1, 1),
    COMMAND(UMI_DEBUG_COMMAND_STEP_OUT, "debug.step-out", "Step Out",
            "Shift+F11", 0, 1, 1),
    COMMAND(UMI_DEBUG_COMMAND_RUN_TO_CURSOR, "debug.run-to-cursor",
            "Run to Cursor", "", 0, 1, 1),
    COMMAND(UMI_DEBUG_COMMAND_TOGGLE_BREAKPOINT, "debug.breakpoint.toggle",
            "Toggle Breakpoint", "F9", 0, 0, 0),
    COMMAND(UMI_DEBUG_COMMAND_ENABLE_ALL_BREAKPOINTS,
            "debug.breakpoint.enable-all", "Enable All Breakpoints", "", 0, 0,
            0),
    COMMAND(UMI_DEBUG_COMMAND_DISABLE_ALL_BREAKPOINTS,
            "debug.breakpoint.disable-all", "Disable All Breakpoints", "", 0,
            0, 0),
    COMMAND(UMI_DEBUG_COMMAND_REMOVE_ALL_BREAKPOINTS,
            "debug.breakpoint.remove-all", "Remove All Breakpoints", "", 0, 0,
            0),
    COMMAND(UMI_DEBUG_COMMAND_ADD_CONDITIONAL_BREAKPOINT,
            "debug.breakpoint.add-conditional", "Add Conditional Breakpoint…",
            "", 0, 0, 0),
    COMMAND(UMI_DEBUG_COMMAND_ADD_LOGPOINT, "debug.breakpoint.add-logpoint",
            "Add Logpoint…", "", 0, 0, 0),
    COMMAND(UMI_DEBUG_COMMAND_ADD_WATCH, "debug.watch.add", "Add Watch…", "", 0,
            1, 1),
    COMMAND(UMI_DEBUG_COMMAND_REMOVE_WATCH, "debug.watch.remove",
            "Remove Watch", "", 0, 0, 0),
    COMMAND(UMI_DEBUG_COMMAND_EVALUATE_SELECTION, "debug.evaluate-selection",
            "Evaluate Selection", "Ctrl+Shift+E", 0, 1, 1),
    COMMAND(UMI_DEBUG_COMMAND_OPEN_CONSOLE, "debug.console.open",
            "Open Debug Console", "Ctrl+Shift+Y", 0, 0, 0),
    COMMAND(UMI_DEBUG_COMMAND_CLEAR_CONSOLE, "debug.console.clear",
            "Clear Debug Console", "", 0, 0, 0),
    COMMAND(UMI_DEBUG_COMMAND_COPY_CALL_STACK, "debug.call-stack.copy",
            "Copy Call Stack", "", 0, 1, 1),
    COMMAND(UMI_DEBUG_COMMAND_OPEN_DISASSEMBLY, "debug.disassembly.open",
            "Open Disassembly", "", 0, 1, 1),
    COMMAND(UMI_DEBUG_COMMAND_OPEN_MEMORY, "debug.memory.open", "Open Memory",
            "", 0, 1, 1),

    COMMAND(UMI_DEBUG_COMMAND_ADD_FUNCTION_BREAKPOINT,
            "debug.breakpoint.add-function", "Add Function Breakpoint…", "",
            0, 1, 0),
    COMMAND(UMI_DEBUG_COMMAND_ADD_DATA_BREAKPOINT,
            "debug.breakpoint.add-data", "Add Data Breakpoint…", "", 0, 1, 1),
    COMMAND(UMI_DEBUG_COMMAND_ADD_INSTRUCTION_BREAKPOINT,
            "debug.breakpoint.add-instruction", "Add Instruction Breakpoint…",
            "", 0, 1, 1),
    COMMAND(UMI_DEBUG_COMMAND_CONFIGURE_EXCEPTION_BREAKPOINTS,
            "debug.breakpoint.configure-exceptions",
            "Configure Exception Breakpoints…", "", 0, 1, 0),
    COMMAND(UMI_DEBUG_COMMAND_OPEN_ADVANCED_BREAKPOINTS,
            "debug.breakpoints.open-advanced", "Open Advanced Breakpoints", "",
            0, 0, 0),
    COMMAND(UMI_DEBUG_COMMAND_OPEN_THREADS, "debug.threads.open",
            "Open Threads", "", 0, 0, 0),
    COMMAND(UMI_DEBUG_COMMAND_SELECT_THREAD, "debug.thread.select",
            "Select Debug Thread…", "", 0, 1, 1),
    COMMAND(UMI_DEBUG_COMMAND_SELECT_FRAME, "debug.frame.select",
            "Select Stack Frame…", "", 0, 1, 1),
    COMMAND(UMI_DEBUG_COMMAND_REFRESH_THREADS, "debug.threads.refresh",
            "Refresh Threads", "", 0, 1, 0),
    COMMAND(UMI_DEBUG_COMMAND_REFRESH_VARIABLES, "debug.variables.refresh",
            "Refresh Variables", "", 0, 1, 1),
    COMMAND(UMI_DEBUG_COMMAND_SET_VARIABLE, "debug.variable.set",
            "Set Variable…", "", 0, 1, 1),
    COMMAND(UMI_DEBUG_COMMAND_SET_EXPRESSION, "debug.expression.set",
            "Set Expression…", "", 0, 1, 1),
    COMMAND(UMI_DEBUG_COMMAND_STEP_BACK, "debug.step-back", "Step Back", "",
            0, 1, 1),
    COMMAND(UMI_DEBUG_COMMAND_REVERSE_CONTINUE, "debug.reverse-continue",
            "Reverse Continue", "", 0, 1, 1),
    COMMAND(UMI_DEBUG_COMMAND_RESTART_FRAME, "debug.frame.restart",
            "Restart Frame", "", 0, 1, 1),
    COMMAND(UMI_DEBUG_COMMAND_OPEN_MODULES, "debug.modules.open",
            "Open Loaded Modules", "", 0, 1, 0),
    COMMAND(UMI_DEBUG_COMMAND_OPEN_REGISTERS, "debug.registers.open",
            "Open Registers", "", 0, 0, 0),
    COMMAND(UMI_DEBUG_COMMAND_REFRESH_REGISTERS, "debug.registers.refresh",
            "Refresh Registers", "", 0, 1, 1),
    COMMAND(UMI_DEBUG_COMMAND_REFRESH_MEMORY, "debug.memory.refresh",
            "Refresh Memory", "", 0, 1, 1),
    COMMAND(UMI_DEBUG_COMMAND_WRITE_MEMORY, "debug.memory.write",
            "Write Memory…", "", 0, 1, 1),
    COMMAND(UMI_DEBUG_COMMAND_REFRESH_DISASSEMBLY,
            "debug.disassembly.refresh", "Refresh Disassembly", "", 0, 1, 1),
    COMMAND(UMI_DEBUG_COMMAND_TOGGLE_INSTRUCTION_BREAKPOINT,
            "debug.breakpoint.toggle-instruction",
            "Toggle Instruction Breakpoint", "", 0, 1, 1),
    COMMAND(UMI_DEBUG_COMMAND_OPEN_ADAPTER_CENTRE, "debug.adapters.open",
            "Open Debug Adapter Centre", "", 0, 0, 0),
    COMMAND(UMI_DEBUG_COMMAND_COPY_MEMORY_ADDRESS,
            "debug.memory.copy-address", "Copy Memory Address", "", 0, 1, 1)
};

#undef COMMAND

size_t umi_debug_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

const UmiDebugCommandDescriptor *umi_debug_command_at(size_t index)
{
    return index < umi_debug_command_count() ? &COMMANDS[index] : NULL;
}

const UmiDebugCommandDescriptor *umi_debug_command_find(const char *command_id)
{
    size_t index;
    if (command_id == NULL) return NULL;
    for (index = 0U; index < umi_debug_command_count(); ++index) {
        if (strcmp(COMMANDS[index].id, command_id) == 0) return &COMMANDS[index];
    }
    return NULL;
}

const UmiDebugCommandDescriptor *umi_debug_command_for_kind(
    UmiDebugCommandKind kind)
{
    size_t index;
    for (index = 0U; index < umi_debug_command_count(); ++index) {
        if (COMMANDS[index].kind == kind) return &COMMANDS[index];
    }
    return NULL;
}
