/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/host_reconnect.h
 *
 * PURPOSE:
 *   Calculate bounded reconnect backoff following host interruption.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_HOST_RECONNECT_H
#define UMICOM_PLUGIN_EXTENSION_HOST_HOST_RECONNECT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host host reconnect data shared with callers of this
 * public contract.
 */
typedef struct UmiPluginExtensionHostHostReconnect { uint32_t attempts; uint32_t maximum_attempts; uint64_t base_delay_ms; uint64_t maximum_delay_ms; } UmiPluginExtensionHostHostReconnect;
/**
 * Initialise plugin extension host host reconnect from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_host_reconnect_init(UmiPluginExtensionHostHostReconnect *value);
/**
 * Provide the plugin extension host host reconnect failed operation used by this module
 * and its client applications.
 */
UmiStatus umi_plugin_extension_host_host_reconnect_failed(UmiPluginExtensionHostHostReconnect *value);
/**
 * Provide the plugin extension host host reconnect delay ms operation used by this module
 * and its client applications.
 */
uint64_t umi_plugin_extension_host_host_reconnect_delay_ms(const UmiPluginExtensionHostHostReconnect *value);
/**
 * Provide the plugin extension host host reconnect may retry operation used by this module
 * and its client applications.
 */
int umi_plugin_extension_host_host_reconnect_may_retry(const UmiPluginExtensionHostHostReconnect *value);

#ifdef __cplusplus
}
#endif

#endif
