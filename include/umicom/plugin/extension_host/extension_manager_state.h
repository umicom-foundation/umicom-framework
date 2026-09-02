/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/extension_manager_state.h
 *
 * PURPOSE:
 *   Aggregate installation, activation, host and quarantine counts for Extension Centre.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_EXTENSION_MANAGER_STATE_H
#define UMICOM_PLUGIN_EXTENSION_HOST_EXTENSION_MANAGER_STATE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host extension manager state data shared with callers of
 * this public contract.
 */
typedef struct UmiPluginExtensionHostExtensionManagerState { uint32_t discovered; uint32_t installed; uint32_t enabled; uint32_t running; uint32_t failed; uint32_t quarantined; uint32_t updates_available; uint64_t revision; } UmiPluginExtensionHostExtensionManagerState;
/**
 * Initialise plugin extension host extension manager state from caller-provided values so
 * later operations receive a known state.
 */
void umi_plugin_extension_host_extension_manager_state_init(UmiPluginExtensionHostExtensionManagerState *state);
/**
 * Provide the plugin extension host extension manager state record operation used by this
 * module and its client applications.
 */
void umi_plugin_extension_host_extension_manager_state_record(UmiPluginExtensionHostExtensionManagerState *state, UmiPluginExtensionHostLifecycle lifecycle, int update_available);
/**
 * Provide the plugin extension host extension manager state health operation used by this
 * module and its client applications.
 */
UmiPluginExtensionHostHealthState umi_plugin_extension_host_extension_manager_state_health(const UmiPluginExtensionHostExtensionManagerState *state);

#ifdef __cplusplus
}
#endif

#endif
