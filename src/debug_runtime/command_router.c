/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/command_router.c
 *
 * PURPOSE:
 *   Route operational debugger commands through the real DAP runtime while
 *   leaving view/picker commands to the thin presentation application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/command_router.h"

#include <stdio.h>
#include <string.h>

void umi_debug_runtime_command_context_init(
    UmiDebugRuntimeCommandContext *context)
{
    if (context == NULL) return;
    (void)memset(context, 0, sizeof(*context));
    context->timeout_ms = 1000U;
    context->memory_count = 256U;
    context->instruction_count = 64U;
}

static UmiStatus snapshot(
    UmiDebugRuntimePlatform *platform,
    UmiDebugRuntimePlatformSnapshot *out_snapshot)
{
    return umi_debug_runtime_platform_snapshot(platform, out_snapshot);
}

static int presentation_owned(UmiDebugCommandKind kind)
{
    switch (kind) {
        case UMI_DEBUG_COMMAND_START_WITHOUT_DEBUGGING:
        case UMI_DEBUG_COMMAND_SELECT_CONFIGURATION:
        case UMI_DEBUG_COMMAND_OPEN_CONFIGURATION:
        case UMI_DEBUG_COMMAND_RUN_TO_CURSOR:
        case UMI_DEBUG_COMMAND_TOGGLE_BREAKPOINT:
        case UMI_DEBUG_COMMAND_ENABLE_ALL_BREAKPOINTS:
        case UMI_DEBUG_COMMAND_DISABLE_ALL_BREAKPOINTS:
        case UMI_DEBUG_COMMAND_REMOVE_ALL_BREAKPOINTS:
        case UMI_DEBUG_COMMAND_ADD_CONDITIONAL_BREAKPOINT:
        case UMI_DEBUG_COMMAND_ADD_LOGPOINT:
        case UMI_DEBUG_COMMAND_REMOVE_WATCH:
        case UMI_DEBUG_COMMAND_OPEN_CONSOLE:
        case UMI_DEBUG_COMMAND_CLEAR_CONSOLE:
        case UMI_DEBUG_COMMAND_COPY_CALL_STACK:
        case UMI_DEBUG_COMMAND_OPEN_DISASSEMBLY:
        case UMI_DEBUG_COMMAND_OPEN_MEMORY:
        case UMI_DEBUG_COMMAND_ADD_FUNCTION_BREAKPOINT:
        case UMI_DEBUG_COMMAND_ADD_DATA_BREAKPOINT:
        case UMI_DEBUG_COMMAND_ADD_INSTRUCTION_BREAKPOINT:
        case UMI_DEBUG_COMMAND_CONFIGURE_EXCEPTION_BREAKPOINTS:
        case UMI_DEBUG_COMMAND_OPEN_ADVANCED_BREAKPOINTS:
        case UMI_DEBUG_COMMAND_OPEN_THREADS:
        case UMI_DEBUG_COMMAND_SELECT_THREAD:
        case UMI_DEBUG_COMMAND_SELECT_FRAME:
        case UMI_DEBUG_COMMAND_OPEN_MODULES:
        case UMI_DEBUG_COMMAND_OPEN_REGISTERS:
        case UMI_DEBUG_COMMAND_TOGGLE_INSTRUCTION_BREAKPOINT:
        case UMI_DEBUG_COMMAND_OPEN_ADAPTER_CENTRE:
        case UMI_DEBUG_COMMAND_COPY_MEMORY_ADDRESS:
            return 1;
        default:
            return 0;
    }
}

int umi_debug_runtime_command_enabled(
    UmiDebugRuntimePlatform *platform,
    UmiDebugCommandKind kind,
    const UmiDebugRuntimeCommandContext *context)
{
    const UmiDebugCommandDescriptor *descriptor;
    UmiDebugRuntimePlatformSnapshot state;

    if (platform == NULL || context == NULL) return 0;

    descriptor = umi_debug_command_for_kind(kind);
    if (descriptor == NULL) return 0;

    if (snapshot(platform, &state) != UMI_STATUS_OK) return 0;

    if (kind == UMI_DEBUG_COMMAND_START) {
        return !state.active &&
            context->profile_id[0] != '\0' &&
            context->session_id[0] != '\0' &&
            context->configuration_id[0] != '\0';
    }

    if (presentation_owned(kind)) return 0;

    if (descriptor->requires_active_session && !state.active) return 0;
    if (descriptor->requires_paused_session && !state.paused) return 0;

    switch (kind) {
        case UMI_DEBUG_COMMAND_RESTART:
            return state.capabilities.supports_restart;
        case UMI_DEBUG_COMMAND_STEP_BACK:
        case UMI_DEBUG_COMMAND_REVERSE_CONTINUE:
            return state.capabilities.supports_step_back;
        case UMI_DEBUG_COMMAND_RESTART_FRAME:
            return state.capabilities.supports_restart_frame;
        case UMI_DEBUG_COMMAND_SET_VARIABLE:
            return state.capabilities.supports_set_variable;
        case UMI_DEBUG_COMMAND_SET_EXPRESSION:
            return state.capabilities.supports_set_expression;
        case UMI_DEBUG_COMMAND_OPEN_MODULES:
            return state.capabilities.supports_modules_request;
        case UMI_DEBUG_COMMAND_REFRESH_MEMORY:
            return state.capabilities.supports_read_memory_request &&
                context->memory_reference[0] != '\0';
        case UMI_DEBUG_COMMAND_WRITE_MEMORY:
            return state.capabilities.supports_write_memory_request &&
                context->memory_reference[0] != '\0' &&
                context->value[0] != '\0';
        case UMI_DEBUG_COMMAND_REFRESH_DISASSEMBLY:
            return state.capabilities.supports_disassemble_request &&
                context->memory_reference[0] != '\0';
        default:
            return 1;
    }
}

static UmiStatus format_result(
    UmiStatus status,
    const UmiDebugCommandDescriptor *descriptor,
    char *out_message,
    size_t message_capacity)
{
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(
            out_message,
            message_capacity,
            "%s: %s",
            descriptor != NULL ? descriptor->label : "Debug command",
            status == UMI_STATUS_OK ? "completed" : umi_status_text(status));
    }
    return status;
}

UmiStatus umi_debug_runtime_command_execute(
    UmiDebugRuntimePlatform *platform,
    UmiDebugCommandKind kind,
    const UmiDebugRuntimeCommandContext *context,
    char *out_message,
    size_t message_capacity)
{
    const UmiDebugCommandDescriptor *descriptor =
        umi_debug_command_for_kind(kind);
    UmiStatus status = UMI_STATUS_NOT_IMPLEMENTED;

    if (platform == NULL || context == NULL || descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (!umi_debug_runtime_command_enabled(platform, kind, context)) {
        return format_result(
            UMI_STATUS_INVALID_STATE,
            descriptor,
            out_message,
            message_capacity);
    }

    switch (kind) {
        case UMI_DEBUG_COMMAND_START:
            status = umi_debug_runtime_platform_start(
                platform,
                context->profile_id,
                context->session_id,
                context->configuration_id,
                context->launch_arguments_json[0] != '\0'
                    ? context->launch_arguments_json
                    : NULL,
                0,
                context->working_directory[0] != '\0'
                    ? context->working_directory
                    : NULL,
                context->timeout_ms);
            break;

        case UMI_DEBUG_COMMAND_STOP:
            status = umi_debug_runtime_platform_stop(
                platform, 1, context->timeout_ms);
            break;

        case UMI_DEBUG_COMMAND_RESTART:
            status = umi_debug_runtime_platform_restart(
                platform, context->timeout_ms);
            break;

        case UMI_DEBUG_COMMAND_CONTINUE:
            status = umi_debug_runtime_platform_continue(
                platform, context->thread_id, context->timeout_ms);
            break;

        case UMI_DEBUG_COMMAND_PAUSE:
            status = umi_debug_runtime_platform_pause(
                platform, context->thread_id, context->timeout_ms);
            break;

        case UMI_DEBUG_COMMAND_STEP_OVER:
            status = umi_debug_runtime_platform_step_over(
                platform, context->thread_id, context->timeout_ms);
            break;

        case UMI_DEBUG_COMMAND_STEP_INTO:
            status = umi_debug_runtime_platform_step_into(
                platform, context->thread_id, context->timeout_ms);
            break;

        case UMI_DEBUG_COMMAND_STEP_OUT:
            status = umi_debug_runtime_platform_step_out(
                platform, context->thread_id, context->timeout_ms);
            break;

        case UMI_DEBUG_COMMAND_REFRESH_THREADS:
            status = umi_debug_runtime_platform_refresh_threads(
                platform, context->timeout_ms);
            break;

        case UMI_DEBUG_COMMAND_REFRESH_VARIABLES:
            status = umi_debug_runtime_platform_refresh_variables(
                platform,
                context->scope_id,
                context->variables_reference,
                context->timeout_ms);
            break;

        case UMI_DEBUG_COMMAND_EVALUATE_SELECTION:
        case UMI_DEBUG_COMMAND_ADD_WATCH:
            status = umi_debug_runtime_platform_evaluate_watch(
                platform,
                context->watch_id[0] != '\0'
                    ? context->watch_id
                    : "watch.selection",
                context->expression,
                context->frame_id,
                context->timeout_ms);
            break;

        case UMI_DEBUG_COMMAND_SET_VARIABLE: {
            UmiDebugRuntimeEvaluateResult result;
            status = umi_debug_runtime_platform_set_variable(
                platform,
                context->variables_reference,
                context->name,
                context->value,
                context->timeout_ms,
                &result);
            break;
        }

        case UMI_DEBUG_COMMAND_SET_EXPRESSION: {
            UmiDebugRuntimeEvaluateResult result;
            status = umi_debug_runtime_platform_set_expression(
                platform,
                context->expression,
                context->value,
                context->frame_id,
                context->timeout_ms,
                &result);
            break;
        }

        case UMI_DEBUG_COMMAND_STEP_BACK:
            status = umi_debug_runtime_platform_step_back(
                platform, context->thread_id, context->timeout_ms);
            break;

        case UMI_DEBUG_COMMAND_REVERSE_CONTINUE:
            status = umi_debug_runtime_platform_reverse_continue(
                platform, context->thread_id, context->timeout_ms);
            break;

        case UMI_DEBUG_COMMAND_RESTART_FRAME:
            status = umi_debug_runtime_platform_restart_frame(
                platform, context->frame_id, context->timeout_ms);
            break;

        case UMI_DEBUG_COMMAND_OPEN_MODULES:
            status = umi_debug_runtime_platform_refresh_modules(
                platform, context->timeout_ms);
            break;

        case UMI_DEBUG_COMMAND_REFRESH_REGISTERS:
            status = umi_debug_runtime_platform_refresh_registers(
                platform, context->frame_id, context->timeout_ms);
            break;

        case UMI_DEBUG_COMMAND_REFRESH_MEMORY: {
            UmiDebugRuntimeMemoryResult result;
            status = umi_debug_runtime_platform_read_memory(
                platform,
                context->memory_reference,
                context->memory_offset,
                context->memory_count,
                context->timeout_ms,
                &result);
            break;
        }

        case UMI_DEBUG_COMMAND_WRITE_MEMORY:
            status = umi_debug_runtime_platform_write_memory(
                platform,
                context->memory_reference,
                context->memory_offset,
                context->value,
                1,
                context->timeout_ms);
            break;

        case UMI_DEBUG_COMMAND_REFRESH_DISASSEMBLY: {
            UmiDebugRuntimeDisassembly result;
            status = umi_debug_runtime_platform_disassemble(
                platform,
                context->memory_reference,
                context->memory_offset,
                0,
                context->instruction_count,
                context->timeout_ms,
                &result);
            break;
        }

        /*
         * These commands are presentation/picker/edit-planning actions. Their
         * canonical IDs stay in Framework, but a thin IDE must supply the view
         * or user input before the runtime can execute a typed mutation.
         */
        case UMI_DEBUG_COMMAND_START_WITHOUT_DEBUGGING:
        case UMI_DEBUG_COMMAND_SELECT_CONFIGURATION:
        case UMI_DEBUG_COMMAND_OPEN_CONFIGURATION:
        case UMI_DEBUG_COMMAND_RUN_TO_CURSOR:
        case UMI_DEBUG_COMMAND_ENABLE_ALL_BREAKPOINTS:
        case UMI_DEBUG_COMMAND_DISABLE_ALL_BREAKPOINTS:
        case UMI_DEBUG_COMMAND_REMOVE_ALL_BREAKPOINTS:
        case UMI_DEBUG_COMMAND_ADD_CONDITIONAL_BREAKPOINT:
        case UMI_DEBUG_COMMAND_ADD_LOGPOINT:
        case UMI_DEBUG_COMMAND_REMOVE_WATCH:
        case UMI_DEBUG_COMMAND_OPEN_CONSOLE:
        case UMI_DEBUG_COMMAND_CLEAR_CONSOLE:
        case UMI_DEBUG_COMMAND_COPY_CALL_STACK:
        case UMI_DEBUG_COMMAND_OPEN_DISASSEMBLY:
        case UMI_DEBUG_COMMAND_OPEN_MEMORY:
        case UMI_DEBUG_COMMAND_ADD_FUNCTION_BREAKPOINT:
        case UMI_DEBUG_COMMAND_ADD_DATA_BREAKPOINT:
        case UMI_DEBUG_COMMAND_ADD_INSTRUCTION_BREAKPOINT:
        case UMI_DEBUG_COMMAND_CONFIGURE_EXCEPTION_BREAKPOINTS:
        case UMI_DEBUG_COMMAND_OPEN_ADVANCED_BREAKPOINTS:
        case UMI_DEBUG_COMMAND_OPEN_THREADS:
        case UMI_DEBUG_COMMAND_SELECT_THREAD:
        case UMI_DEBUG_COMMAND_SELECT_FRAME:
        case UMI_DEBUG_COMMAND_OPEN_REGISTERS:
        case UMI_DEBUG_COMMAND_TOGGLE_INSTRUCTION_BREAKPOINT:
        case UMI_DEBUG_COMMAND_OPEN_ADAPTER_CENTRE:
        case UMI_DEBUG_COMMAND_COPY_MEMORY_ADDRESS:
        default:
            status = UMI_STATUS_NOT_IMPLEMENTED;
            break;
    }

    return format_result(
        status,
        descriptor,
        out_message,
        message_capacity);
}
