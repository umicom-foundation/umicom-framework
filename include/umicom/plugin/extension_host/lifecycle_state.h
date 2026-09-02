/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/lifecycle_state.h
 *
 * PURPOSE:
 *   Track the extension lifecycle without allowing invalid state regression.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_LIFECYCLE_STATE_H
#define UMICOM_PLUGIN_EXTENSION_HOST_LIFECYCLE_STATE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host lifecycle state data shared with callers of this
 * public contract.
 */
typedef struct UmiPluginExtensionHostLifecycleState { UmiPluginExtensionHostLifecycle state; UmiPluginExtensionHostLifecycle previous; uint64_t revision; uint64_t changed_ms; } UmiPluginExtensionHostLifecycleState;
/**
 * Initialise plugin extension host lifecycle state from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_lifecycle_state_init(UmiPluginExtensionHostLifecycleState *value);
/**
 * Copy plugin extension host lifecycle state into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_plugin_extension_host_lifecycle_state_set(UmiPluginExtensionHostLifecycleState *value, UmiPluginExtensionHostLifecycle next, uint64_t now_ms);
/**
 * Provide the plugin extension host lifecycle state active operation used by this module
 * and its client applications.
 */
int umi_plugin_extension_host_lifecycle_state_active(const UmiPluginExtensionHostLifecycleState *value);

#ifdef __cplusplus
}
#endif

#endif
