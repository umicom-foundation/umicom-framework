/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/abi_negotiation.h
 *
 * PURPOSE:
 *   Negotiate ABI versions and feature flags without loading incompatible extensions.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_ABI_NEGOTIATION_H
#define UMICOM_PLUGIN_EXTENSION_HOST_ABI_NEGOTIATION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostAbiNegotiation { uint32_t host_version; uint32_t minimum_version; uint32_t maximum_version; uint64_t required_flags; uint64_t optional_flags; uint64_t supported_flags; uint64_t negotiated_flags; uint64_t missing_flags; } UmiPluginExtensionHostAbiNegotiation;
void umi_plugin_extension_host_abi_negotiation_init(UmiPluginExtensionHostAbiNegotiation *value);
UmiStatus umi_plugin_extension_host_abi_negotiation_evaluate(UmiPluginExtensionHostAbiNegotiation *value);
int umi_plugin_extension_host_abi_negotiation_compatible(const UmiPluginExtensionHostAbiNegotiation *value);

#ifdef __cplusplus
}
#endif

#endif
