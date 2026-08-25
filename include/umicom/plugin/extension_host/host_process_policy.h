/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/host_process_policy.h
 *
 * PURPOSE:
 *   Decide when extensions require process isolation and restart limits.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_HOST_PROCESS_POLICY_H
#define UMICOM_PLUGIN_EXTENSION_HOST_HOST_PROCESS_POLICY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostHostProcessPolicy { uint32_t maximum_crashes; uint32_t maximum_restarts; uint32_t isolation_risk_threshold; int require_isolation_for_untrusted; } UmiPluginExtensionHostHostProcessPolicy;
void umi_plugin_extension_host_host_process_policy_init(UmiPluginExtensionHostHostProcessPolicy *policy);
UmiPluginExtensionHostIsolation umi_plugin_extension_host_host_process_policy_isolation(const UmiPluginExtensionHostHostProcessPolicy *policy, UmiPluginExtensionHostTrust trust, uint32_t risk);
int umi_plugin_extension_host_host_process_policy_may_restart(const UmiPluginExtensionHostHostProcessPolicy *policy, uint32_t crash_count, uint32_t restart_count);

#ifdef __cplusplus
}
#endif

#endif
