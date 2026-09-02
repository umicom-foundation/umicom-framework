/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/isolation_profile.h
 *
 * PURPOSE:
 *   Describe process/isolation requirements selected for one extension.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_ISOLATION_PROFILE_H
#define UMICOM_PLUGIN_EXTENSION_HOST_ISOLATION_PROFILE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host isolation profile data shared with callers of this
 * public contract.
 */
typedef struct UmiPluginExtensionHostIsolationProfile { UmiPluginExtensionHostIsolation mode; int network_isolated; int filesystem_isolated; int process_isolated; int environment_sanitized; uint32_t risk_ceiling; } UmiPluginExtensionHostIsolationProfile;
/**
 * Initialise plugin extension host isolation profile from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_isolation_profile_init(UmiPluginExtensionHostIsolationProfile *profile);
/**
 * Provide the plugin extension host isolation profile configure operation used by this
 * module and its client applications.
 */
UmiStatus umi_plugin_extension_host_isolation_profile_configure(UmiPluginExtensionHostIsolationProfile *profile, UmiPluginExtensionHostIsolation mode, uint32_t risk);
/**
 * Provide the plugin extension host isolation profile satisfies operation used by this
 * module and its client applications.
 */
int umi_plugin_extension_host_isolation_profile_satisfies(const UmiPluginExtensionHostIsolationProfile *profile, uint32_t risk, int needs_network, int needs_process);

#ifdef __cplusplus
}
#endif

#endif
