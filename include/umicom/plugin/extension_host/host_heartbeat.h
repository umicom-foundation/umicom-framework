/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/host_heartbeat.h
 *
 * PURPOSE:
 *   Track liveness deadlines for isolated extension-host sessions.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_HOST_HEARTBEAT_H
#define UMICOM_PLUGIN_EXTENSION_HOST_HOST_HEARTBEAT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostHostHeartbeat { uint64_t interval_ms; uint64_t timeout_ms; uint64_t last_sent_ms; uint64_t last_received_ms; uint32_t missed; } UmiPluginExtensionHostHostHeartbeat;
void umi_plugin_extension_host_host_heartbeat_init(UmiPluginExtensionHostHostHeartbeat *heartbeat, uint64_t interval_ms, uint64_t timeout_ms);
void umi_plugin_extension_host_host_heartbeat_sent(UmiPluginExtensionHostHostHeartbeat *heartbeat, uint64_t now_ms);
void umi_plugin_extension_host_host_heartbeat_received(UmiPluginExtensionHostHostHeartbeat *heartbeat, uint64_t now_ms);
int umi_plugin_extension_host_host_heartbeat_expired(const UmiPluginExtensionHostHostHeartbeat *heartbeat, uint64_t now_ms);

#ifdef __cplusplus
}
#endif

#endif
