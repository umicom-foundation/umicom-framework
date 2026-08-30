/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/debug_bridge.h
 *
 * PURPOSE:
 *   Bridge the persistent DAP runtime to editor navigation while allowing the
 *   debugger's stack-frame store to remain authoritative.
 *
 * NOTE:
 *   The frame resolver is injected by the existing debug service composition.
 *   It resolves a DAP frame ID into a source location; this integration layer
 *   never duplicates debugger frame storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_DEBUG_BRIDGE_H
#define UMICOM_IDE_INTEGRATION_DEBUG_BRIDGE_H
#include "umicom/debug_runtime/platform.h"
#include "umicom/ide_integration/location.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiIdeDebugFrameLocationResolver)(
    void *user_data,
    uint64_t frame_id,
    UmiIdeLocation *out_location);

typedef struct UmiIdeDebugBridge {
    UmiDebugRuntimePlatform *runtime;
    UmiIdeDebugFrameLocationResolver resolve_frame;
    void *resolver_user_data;
} UmiIdeDebugBridge;

UmiStatus umi_ide_debug_bridge_init(
    UmiIdeDebugBridge *bridge,
    UmiDebugRuntimePlatform *runtime,
    UmiIdeDebugFrameLocationResolver resolver,
    void *user_data);

UmiStatus umi_ide_debug_active_frame_target(
    UmiIdeDebugBridge *bridge,
    UmiIdeNavigationTarget *out_target);

#ifdef __cplusplus
}
#endif
#endif
