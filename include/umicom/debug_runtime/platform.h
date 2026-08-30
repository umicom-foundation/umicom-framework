/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/platform.h
 *
 * PURPOSE:
 *   Compose the existing Debug Service and Advanced Debug Platform with a real
 *   persistent DAP adapter. This is the Framework-owned operational debugger
 *   runtime consumed by thin IDE applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_PLATFORM_H
#define UMICOM_DEBUG_RUNTIME_PLATFORM_H

#include "umicom/debug/advanced_debugging.h"
#include "umicom/debug_runtime/builtin_profiles.h"
#include "umicom/debug_runtime/capabilities.h"
#include "umicom/debug_runtime/contract_adapter.h"
#include "umicom/debug_runtime/profile_health.h"
#include "umicom/debug_runtime/service_bridge.h"
#include "umicom/language_runtime/arguments.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugRuntimePlatform UmiDebugRuntimePlatform;

typedef struct UmiDebugRuntimePlatformSnapshot {
    UmiDebugServiceSnapshot service;
    UmiDebugAdvancedPlatformSnapshot advanced;
    UmiDebugRuntimeAdapterSnapshot adapter;
    UmiDebugRuntimeCapabilities capabilities;
    char active_session_id[128];
    char active_configuration_id[128];
    char active_profile_id[128];
    uint64_t active_thread_id;
    uint64_t active_frame_id;
    uint64_t capability_bits;
    uint64_t revision;
    int active;
    int initialized;
    int attached;
    int paused;
} UmiDebugRuntimePlatformSnapshot;

UmiStatus umi_debug_runtime_platform_create(
    UmiDebugRuntimePlatform **out_platform);

void umi_debug_runtime_platform_destroy(
    UmiDebugRuntimePlatform *platform);

UmiStatus umi_debug_runtime_platform_start(
    UmiDebugRuntimePlatform *platform,
    const char *profile_id,
    const char *session_id,
    const char *configuration_id,
    const char *launch_or_attach_arguments_json,
    int attach,
    const char *working_directory,
    uint32_t timeout_ms);

UmiStatus umi_debug_runtime_platform_stop(
    UmiDebugRuntimePlatform *platform,
    int terminate_debuggee,
    uint32_t timeout_ms);

UmiStatus umi_debug_runtime_platform_restart(
    UmiDebugRuntimePlatform *platform,
    uint32_t timeout_ms);

UmiStatus umi_debug_runtime_platform_pump_event(
    UmiDebugRuntimePlatform *platform,
    uint32_t timeout_ms,
    int *out_handled);

UmiStatus umi_debug_runtime_platform_sync_breakpoints(
    UmiDebugRuntimePlatform *platform,
    const char *source_uri,
    uint32_t timeout_ms);

UmiStatus umi_debug_runtime_platform_refresh_threads(
    UmiDebugRuntimePlatform *platform,
    uint32_t timeout_ms);

UmiStatus umi_debug_runtime_platform_refresh_stack(
    UmiDebugRuntimePlatform *platform,
    uint64_t thread_id,
    uint32_t timeout_ms);

UmiStatus umi_debug_runtime_platform_refresh_scopes(
    UmiDebugRuntimePlatform *platform,
    uint64_t frame_id,
    uint32_t timeout_ms);

UmiStatus umi_debug_runtime_platform_refresh_variables(
    UmiDebugRuntimePlatform *platform,
    const char *scope_id,
    uint64_t variables_reference,
    uint32_t timeout_ms);

UmiStatus umi_debug_runtime_platform_evaluate_watch(
    UmiDebugRuntimePlatform *platform,
    const char *watch_id,
    const char *expression,
    uint64_t frame_id,
    uint32_t timeout_ms);

UmiStatus umi_debug_runtime_platform_continue(
    UmiDebugRuntimePlatform *platform,
    uint64_t thread_id,
    uint32_t timeout_ms);

UmiStatus umi_debug_runtime_platform_pause(
    UmiDebugRuntimePlatform *platform,
    uint64_t thread_id,
    uint32_t timeout_ms);

UmiStatus umi_debug_runtime_platform_step_over(
    UmiDebugRuntimePlatform *platform,
    uint64_t thread_id,
    uint32_t timeout_ms);

UmiStatus umi_debug_runtime_platform_step_into(
    UmiDebugRuntimePlatform *platform,
    uint64_t thread_id,
    uint32_t timeout_ms);

UmiStatus umi_debug_runtime_platform_step_out(
    UmiDebugRuntimePlatform *platform,
    uint64_t thread_id,
    uint32_t timeout_ms);

UmiStatus umi_debug_runtime_platform_refresh_modules(
    UmiDebugRuntimePlatform *platform,
    uint32_t timeout_ms);

UmiStatus umi_debug_runtime_platform_refresh_registers(
    UmiDebugRuntimePlatform *platform,
    uint64_t frame_id,
    uint32_t timeout_ms);

UmiStatus umi_debug_runtime_platform_step_back(
    UmiDebugRuntimePlatform *platform,
    uint64_t thread_id,
    uint32_t timeout_ms);

UmiStatus umi_debug_runtime_platform_reverse_continue(
    UmiDebugRuntimePlatform *platform,
    uint64_t thread_id,
    uint32_t timeout_ms);

UmiStatus umi_debug_runtime_platform_restart_frame(
    UmiDebugRuntimePlatform *platform,
    uint64_t frame_id,
    uint32_t timeout_ms);

UmiStatus umi_debug_runtime_platform_set_variable(
    UmiDebugRuntimePlatform *platform,
    uint64_t variables_reference,
    const char *name,
    const char *value,
    uint32_t timeout_ms,
    UmiDebugRuntimeEvaluateResult *out_result);

UmiStatus umi_debug_runtime_platform_set_expression(
    UmiDebugRuntimePlatform *platform,
    const char *expression,
    const char *value,
    uint64_t frame_id,
    uint32_t timeout_ms,
    UmiDebugRuntimeEvaluateResult *out_result);

UmiStatus umi_debug_runtime_platform_read_memory(
    UmiDebugRuntimePlatform *platform,
    const char *memory_reference,
    int64_t offset,
    uint32_t count,
    uint32_t timeout_ms,
    UmiDebugRuntimeMemoryResult *out_result);

UmiStatus umi_debug_runtime_platform_write_memory(
    UmiDebugRuntimePlatform *platform,
    const char *memory_reference,
    int64_t offset,
    const char *base64_data,
    int allow_partial,
    uint32_t timeout_ms);

UmiStatus umi_debug_runtime_platform_disassemble(
    UmiDebugRuntimePlatform *platform,
    const char *memory_reference,
    int64_t offset,
    int64_t instruction_offset,
    uint32_t instruction_count,
    uint32_t timeout_ms,
    UmiDebugRuntimeDisassembly *out_result);

UmiStatus umi_debug_runtime_platform_snapshot(
    UmiDebugRuntimePlatform *platform,
    UmiDebugRuntimePlatformSnapshot *out_snapshot);

UmiDebugService *umi_debug_runtime_platform_service(
    UmiDebugRuntimePlatform *platform);

UmiDebugAdvancedPlatform *umi_debug_runtime_platform_advanced(
    UmiDebugRuntimePlatform *platform);

UmiDebugRuntimeAdapter *umi_debug_runtime_platform_adapter(
    UmiDebugRuntimePlatform *platform);

#ifdef __cplusplus
}
#endif
#endif
