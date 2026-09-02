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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the plugin extension host capability set data shared with callers of this
 * public contract.
 */
typedef struct UmiPluginExtensionHostCapabilitySet { uint64_t words[4]; uint64_t revision; } UmiPluginExtensionHostCapabilitySet;
/**
 * Initialise plugin extension host capability set from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_capability_set_init(UmiPluginExtensionHostCapabilitySet *set);
/**
 * Add plugin extension host capability set only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_plugin_extension_host_capability_set_add(UmiPluginExtensionHostCapabilitySet *set, uint32_t capability);
/**
 * Remove plugin extension host capability set while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_plugin_extension_host_capability_set_remove(UmiPluginExtensionHostCapabilitySet *set, uint32_t capability);
/**
 * Provide the plugin extension host capability set contains operation used by this module
 * and its client applications.
 */
int umi_plugin_extension_host_capability_set_contains(const UmiPluginExtensionHostCapabilitySet *set, uint32_t capability);
/**
 * Provide the plugin extension host capability set contains all operation used by this
 * module and its client applications.
 */
int umi_plugin_extension_host_capability_set_contains_all(const UmiPluginExtensionHostCapabilitySet *set, const UmiPluginExtensionHostCapabilitySet *required);

#ifdef __cplusplus
}
#endif

#endif
