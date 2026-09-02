/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/workbench_bridge.h
 *
 * PURPOSE:
 *   Bind the existing professional Developer Workbench debug.start,
 *   debug.attach, debug.continue and debug.stop commands to the real DAP runtime.
 *   View-only commands remain presentation-owned until Studio supplies a view.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_WORKBENCH_BRIDGE_H
#define UMICOM_DEBUG_RUNTIME_WORKBENCH_BRIDGE_H

#include "umicom/debug_runtime/platform.h"
#include "umicom/developer_workbench/workbench.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug runtime workbench context data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugRuntimeWorkbenchContext {
    char profile_id[128];
    char session_id[128];
    char configuration_id[128];
    char launch_arguments_json[UMI_DEBUG_RUNTIME_JSON_CAPACITY];
    char attach_arguments_json[UMI_DEBUG_RUNTIME_JSON_CAPACITY];
    char working_directory[UMI_DEBUG_RUNTIME_PATH_CAPACITY];
    uint64_t thread_id;
    uint32_t timeout_ms;
} UmiDebugRuntimeWorkbenchContext;

/**
 * Represent the debug runtime workbench bridge data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugRuntimeWorkbenchBridge
    UmiDebugRuntimeWorkbenchBridge;

/** Create a bridge that borrows a live workbench and debug runtime. */
UmiStatus umi_debug_runtime_workbench_bridge_create(
    UmiDeveloperWorkbench *workbench,
    UmiDebugRuntimePlatform *runtime,
    UmiDebugRuntimeWorkbenchBridge **out_bridge);

/** Unbind bridge callbacks and release the bridge before its workbench. */
void umi_debug_runtime_workbench_bridge_destroy(
    UmiDebugRuntimeWorkbenchBridge *bridge);

/** Copy launch, attach and selected-thread context into the bridge. */
UmiStatus umi_debug_runtime_workbench_bridge_set_context(
    UmiDebugRuntimeWorkbenchBridge *bridge,
    const UmiDebugRuntimeWorkbenchContext *context);

/** Bind the standard debug commands to context-aware runtime actions. */
UmiStatus umi_debug_runtime_workbench_bridge_bind(
    UmiDebugRuntimeWorkbenchBridge *bridge);

#ifdef __cplusplus
}
#endif
#endif
