/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/command_router.h
 *
 * PURPOSE:
 *   Execute the existing 50-command Framework debugger catalogue against the
 *   real DAP runtime. UI-only commands remain explicitly not implemented here;
 *   process/state-changing commands are routed through typed runtime APIs.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_COMMAND_ROUTER_H
#define UMICOM_DEBUG_RUNTIME_COMMAND_ROUTER_H

#include "umicom/debug/command.h"
#include "umicom/debug_runtime/platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugRuntimeCommandContext {
    char profile_id[128];
    char session_id[128];
    char configuration_id[128];
    char launch_arguments_json[UMI_DEBUG_RUNTIME_JSON_CAPACITY];
    char working_directory[UMI_DEBUG_RUNTIME_PATH_CAPACITY];

    char source_uri[UMI_DEBUG_RUNTIME_PATH_CAPACITY];
    char scope_id[128];
    char watch_id[128];
    char expression[1024];
    char name[256];
    char value[UMI_DEBUG_RUNTIME_TEXT_CAPACITY];
    char memory_reference[512];

    uint64_t thread_id;
    uint64_t frame_id;
    uint64_t variables_reference;
    int64_t memory_offset;
    uint32_t memory_count;
    uint32_t instruction_count;
    uint32_t timeout_ms;
} UmiDebugRuntimeCommandContext;

void umi_debug_runtime_command_context_init(
    UmiDebugRuntimeCommandContext *context);

int umi_debug_runtime_command_enabled(
    UmiDebugRuntimePlatform *platform,
    UmiDebugCommandKind kind,
    const UmiDebugRuntimeCommandContext *context);

UmiStatus umi_debug_runtime_command_execute(
    UmiDebugRuntimePlatform *platform,
    UmiDebugCommandKind kind,
    const UmiDebugRuntimeCommandContext *context,
    char *out_message,
    size_t message_capacity);

#ifdef __cplusplus
}
#endif
#endif
