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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiPluginExtensionHostIsolationProfile { UmiPluginExtensionHostIsolation mode; int network_isolated; int filesystem_isolated; int process_isolated; int environment_sanitized; uint32_t risk_ceiling; } UmiPluginExtensionHostIsolationProfile;
void umi_plugin_extension_host_isolation_profile_init(UmiPluginExtensionHostIsolationProfile *profile);
UmiStatus umi_plugin_extension_host_isolation_profile_configure(UmiPluginExtensionHostIsolationProfile *profile, UmiPluginExtensionHostIsolation mode, uint32_t risk);
int umi_plugin_extension_host_isolation_profile_satisfies(const UmiPluginExtensionHostIsolationProfile *profile, uint32_t risk, int needs_network, int needs_process);

#ifdef __cplusplus
}
#endif

#endif
