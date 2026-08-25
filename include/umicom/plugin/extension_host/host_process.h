/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/host_process.h
 *
 * PURPOSE:
 *   Track one isolated extension-host process and lifecycle evidence.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_HOST_PROCESS_H
#define UMICOM_PLUGIN_EXTENSION_HOST_HOST_PROCESS_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostHostProcess { char plugin_id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; uint64_t process_id; uint64_t started_ms; uint64_t last_heartbeat_ms; uint32_t restart_count; uint32_t crash_count; UmiPluginExtensionHostLifecycle lifecycle; } UmiPluginExtensionHostHostProcess;
void umi_plugin_extension_host_host_process_init(UmiPluginExtensionHostHostProcess *process);
UmiStatus umi_plugin_extension_host_host_process_start(UmiPluginExtensionHostHostProcess *process, const char *plugin_id, uint64_t process_id, uint64_t now_ms);
void umi_plugin_extension_host_host_process_heartbeat(UmiPluginExtensionHostHostProcess *process, uint64_t now_ms);
void umi_plugin_extension_host_host_process_crashed(UmiPluginExtensionHostHostProcess *process);

#ifdef __cplusplus
}
#endif

#endif
