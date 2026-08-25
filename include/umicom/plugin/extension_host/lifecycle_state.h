/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/lifecycle_state.h
 *
 * PURPOSE:
 *   Track the extension lifecycle without allowing invalid state regression.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_LIFECYCLE_STATE_H
#define UMICOM_PLUGIN_EXTENSION_HOST_LIFECYCLE_STATE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostLifecycleState { UmiPluginExtensionHostLifecycle state; UmiPluginExtensionHostLifecycle previous; uint64_t revision; uint64_t changed_ms; } UmiPluginExtensionHostLifecycleState;
void umi_plugin_extension_host_lifecycle_state_init(UmiPluginExtensionHostLifecycleState *value);
UmiStatus umi_plugin_extension_host_lifecycle_state_set(UmiPluginExtensionHostLifecycleState *value, UmiPluginExtensionHostLifecycle next, uint64_t now_ms);
int umi_plugin_extension_host_lifecycle_state_active(const UmiPluginExtensionHostLifecycleState *value);

#ifdef __cplusplus
}
#endif

#endif
