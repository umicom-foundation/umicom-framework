/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/service_bridge.h
 *
 * PURPOSE:
 *   Project decoded DAP state into the existing provider-neutral UmiDebugService
 *   registries. The Debug Service remains authoritative for Studio and other
 *   applications; protocol-specific models never become product state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_SERVICE_BRIDGE_H
#define UMICOM_DEBUG_RUNTIME_SERVICE_BRIDGE_H

#include "umicom/debug/service.h"
#include "umicom/debug_runtime/decoders/decoders.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug runtime service bridge data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugRuntimeServiceBridge {
    UmiDebugService *service;
    char session_id[128];
    uint64_t revision;
} UmiDebugRuntimeServiceBridge;

/**
 * Initialise debug runtime service bridge from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_runtime_service_bridge_init(
    UmiDebugRuntimeServiceBridge *bridge,
    UmiDebugService *service,
    const char *session_id);

/**
 * Provide the debug runtime publish session operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_publish_session(
    UmiDebugRuntimeServiceBridge *bridge,
    const char *configuration_id,
    const char *adapter_id,
    const char *state_text,
    int state,
    int attached,
    int supports_restart);

/**
 * Provide the debug runtime publish breakpoints operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_publish_breakpoints(
    UmiDebugRuntimeServiceBridge *bridge,
    const char *source_uri,
    const UmiDebugRuntimeBreakpointList *result);

/**
 * Provide the debug runtime publish threads operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_publish_threads(
    UmiDebugRuntimeServiceBridge *bridge,
    const UmiDebugRuntimeThreadList *result,
    uint64_t current_thread_id,
    int stopped);

/**
 * Provide the debug runtime publish stack operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_publish_stack(
    UmiDebugRuntimeServiceBridge *bridge,
    uint64_t thread_id,
    const UmiDebugRuntimeStackTrace *result);

/**
 * Provide the debug runtime publish scopes operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_publish_scopes(
    UmiDebugRuntimeServiceBridge *bridge,
    uint64_t frame_id,
    const UmiDebugRuntimeScopeList *result);

/**
 * Provide the debug runtime publish variables operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_publish_variables(
    UmiDebugRuntimeServiceBridge *bridge,
    const char *scope_id,
    const UmiDebugRuntimeVariableList *result);

/**
 * Provide the debug runtime publish watch operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_publish_watch(
    UmiDebugRuntimeServiceBridge *bridge,
    const char *watch_id,
    const char *expression,
    const UmiDebugRuntimeEvaluateResult *result);

/**
 * Provide the debug runtime publish modules operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_publish_modules(
    UmiDebugRuntimeServiceBridge *bridge,
    const UmiDebugRuntimeModuleList *result);

/**
 * Provide the debug runtime publish source operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_publish_source(
    UmiDebugRuntimeServiceBridge *bridge,
    const char *source_id,
    const char *uri,
    const char *name,
    uint64_t source_reference,
    int available);

/**
 * Provide the debug runtime publish exception operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_publish_exception(
    UmiDebugRuntimeServiceBridge *bridge,
    const UmiDebugRuntimeExceptionInfo *result);

/**
 * Provide the debug runtime publish event operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_publish_event(
    UmiDebugRuntimeServiceBridge *bridge,
    const UmiDebugRuntimeEvent *event);

#ifdef __cplusplus
}
#endif
#endif
