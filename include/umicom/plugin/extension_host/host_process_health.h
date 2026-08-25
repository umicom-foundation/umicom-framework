/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/host_process_health.h
 *
 * PURPOSE:
 *   Calculate isolated extension-host process health from heartbeat and crash evidence.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_HOST_PROCESS_HEALTH_H
#define UMICOM_PLUGIN_EXTENSION_HOST_HOST_PROCESS_HEALTH_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostHostProcessHealth { uint64_t now_ms; uint64_t last_heartbeat_ms; uint32_t crash_count; uint32_t restart_count; int responsive; } UmiPluginExtensionHostHostProcessHealth;
UmiPluginExtensionHostHealthState umi_plugin_extension_host_host_process_health_evaluate(const UmiPluginExtensionHostHostProcessHealth *value, uint64_t heartbeat_timeout_ms);
uint32_t umi_plugin_extension_host_host_process_health_score(const UmiPluginExtensionHostHostProcessHealth *value, uint64_t heartbeat_timeout_ms);

#ifdef __cplusplus
}
#endif

#endif
