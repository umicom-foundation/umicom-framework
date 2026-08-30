/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/advanced_debugging.h
 *
 * PURPOSE:
 *   Expose the Framework-owned composition root for advanced debugging:
 *   adapter discovery and selection, session binding, inspection models,
 *   capability-aware commands and deterministic platform snapshots.
 *
 * APPLICATION BOUNDARY:
 *   Umicom Studio contributes command/menu/view placement only. This platform
 *   contains no GTK4 objects and no product-specific state. Future IDEs,
 *   headless automation, remote development hosts and teaching applications can
 *   compose the same service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_ADVANCED_DEBUGGING_H
#define UMICOM_DEBUG_ADVANCED_DEBUGGING_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/debug/inspection_session.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_ADVANCED_PLATFORM_API_VERSION 1U

typedef struct UmiDebugAdvancedPlatformSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiDebugAdapterRegistrySnapshot adapters;
    UmiDebugInspectionSessionSnapshot inspection;
    uint64_t revision;
    int session_open;
    int adapter_available;
    int ready;
} UmiDebugAdvancedPlatformSnapshot;

typedef struct UmiDebugAdvancedPlatform UmiDebugAdvancedPlatform;

UmiStatus umi_debug_advanced_platform_create(
    UmiDebugService *service,
    UmiDebugAdvancedPlatform **out_platform);
void umi_debug_advanced_platform_destroy(UmiDebugAdvancedPlatform *platform);
UmiStatus umi_debug_advanced_platform_register_adapter(
    UmiDebugAdvancedPlatform *platform,
    const UmiDebugAdapterDescriptor *descriptor);
UmiStatus umi_debug_advanced_platform_unregister_adapter(
    UmiDebugAdvancedPlatform *platform,
    const char *adapter_id);
UmiStatus umi_debug_advanced_platform_open_session(
    UmiDebugAdvancedPlatform *platform,
    const char *debug_session_id,
    const char *adapter_id,
    uint64_t advertised_capabilities);
UmiStatus umi_debug_advanced_platform_open_best_session(
    UmiDebugAdvancedPlatform *platform,
    const char *debug_session_id,
    const char *debugger_kind,
    uint64_t required_capabilities,
    uint64_t advertised_capabilities,
    int allow_remote);
UmiStatus umi_debug_advanced_platform_close_session(
    UmiDebugAdvancedPlatform *platform);
UmiStatus umi_debug_advanced_platform_refresh(
    UmiDebugAdvancedPlatform *platform);
UmiStatus umi_debug_advanced_platform_invoke(
    UmiDebugAdvancedPlatform *platform,
    const UmiDebugAdapterRequest *request,
    const UmiDebugAdapterCancellation *cancellation,
    UmiDebugAdapterResponse *out_response);
int umi_debug_advanced_platform_command_enabled(
    const UmiDebugAdvancedPlatform *platform,
    UmiDebugCommandKind command_kind);
UmiDebugAdapterRegistry *umi_debug_advanced_platform_adapters(
    UmiDebugAdvancedPlatform *platform);
UmiDebugInspectionSession *umi_debug_advanced_platform_inspection(
    UmiDebugAdvancedPlatform *platform);
UmiStatus umi_debug_advanced_platform_snapshot(
    const UmiDebugAdvancedPlatform *platform,
    UmiDebugAdvancedPlatformSnapshot *out_snapshot);
uint64_t umi_debug_advanced_platform_revision(
    const UmiDebugAdvancedPlatform *platform);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEBUG_ADVANCED_DEBUGGING_H */
