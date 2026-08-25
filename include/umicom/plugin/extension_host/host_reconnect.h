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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiPluginExtensionHostHostReconnect { uint32_t attempts; uint32_t maximum_attempts; uint64_t base_delay_ms; uint64_t maximum_delay_ms; } UmiPluginExtensionHostHostReconnect;
void umi_plugin_extension_host_host_reconnect_init(UmiPluginExtensionHostHostReconnect *value);
UmiStatus umi_plugin_extension_host_host_reconnect_failed(UmiPluginExtensionHostHostReconnect *value);
uint64_t umi_plugin_extension_host_host_reconnect_delay_ms(const UmiPluginExtensionHostHostReconnect *value);
int umi_plugin_extension_host_host_reconnect_may_retry(const UmiPluginExtensionHostHostReconnect *value);

#ifdef __cplusplus
}
#endif

#endif
