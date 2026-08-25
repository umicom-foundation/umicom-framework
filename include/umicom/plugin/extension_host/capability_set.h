/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/capability_set.h
 *
 * PURPOSE:
 *   Maintain bounded capability bitsets and test subset compatibility.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_CAPABILITY_SET_H
#define UMICOM_PLUGIN_EXTENSION_HOST_CAPABILITY_SET_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostCapabilitySet { uint64_t words[4]; uint64_t revision; } UmiPluginExtensionHostCapabilitySet;
void umi_plugin_extension_host_capability_set_init(UmiPluginExtensionHostCapabilitySet *set);
UmiStatus umi_plugin_extension_host_capability_set_add(UmiPluginExtensionHostCapabilitySet *set, uint32_t capability);
UmiStatus umi_plugin_extension_host_capability_set_remove(UmiPluginExtensionHostCapabilitySet *set, uint32_t capability);
int umi_plugin_extension_host_capability_set_contains(const UmiPluginExtensionHostCapabilitySet *set, uint32_t capability);
int umi_plugin_extension_host_capability_set_contains_all(const UmiPluginExtensionHostCapabilitySet *set, const UmiPluginExtensionHostCapabilitySet *required);

#ifdef __cplusplus
}
#endif

#endif
