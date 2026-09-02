/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/extension_snapshot.h
 *
 * PURPOSE:
 *   Capture a machine-readable point-in-time extension platform health snapshot.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_EXTENSION_SNAPSHOT_H
#define UMICOM_PLUGIN_EXTENSION_HOST_EXTENSION_SNAPSHOT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host extension snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiPluginExtensionHostExtensionSnapshot { uint64_t timestamp_ms; uint32_t installed; uint32_t running; uint32_t failed; uint32_t quarantined; uint32_t pending_approvals; UmiPluginExtensionHostHealthState health; uint64_t evidence; } UmiPluginExtensionHostExtensionSnapshot;
/**
 * Initialise plugin extension host extension snapshot from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_extension_snapshot_init(UmiPluginExtensionHostExtensionSnapshot *snapshot);
/**
 * Provide the plugin extension host extension snapshot capture operation used by this
 * module and its client applications.
 */
UmiStatus umi_plugin_extension_host_extension_snapshot_capture(UmiPluginExtensionHostExtensionSnapshot *snapshot, uint64_t timestamp_ms, uint32_t installed, uint32_t running, uint32_t failed, uint32_t quarantined, uint32_t pending_approvals);
/**
 * Provide the plugin extension host extension snapshot fingerprint operation used by this
 * module and its client applications.
 */
uint64_t umi_plugin_extension_host_extension_snapshot_fingerprint(const UmiPluginExtensionHostExtensionSnapshot *snapshot);

#ifdef __cplusplus
}
#endif

#endif
