/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/capability_negotiation.h
 *
 * PURPOSE:
 *   Negotiate requested, optional and host-supported extension capabilities.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_CAPABILITY_NEGOTIATION_H
#define UMICOM_PLUGIN_EXTENSION_HOST_CAPABILITY_NEGOTIATION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostCapabilityNegotiation { uint32_t host_version; uint32_t minimum_version; uint32_t maximum_version; uint64_t required_flags; uint64_t optional_flags; uint64_t supported_flags; uint64_t negotiated_flags; uint64_t missing_flags; } UmiPluginExtensionHostCapabilityNegotiation;
void umi_plugin_extension_host_capability_negotiation_init(UmiPluginExtensionHostCapabilityNegotiation *value);
UmiStatus umi_plugin_extension_host_capability_negotiation_evaluate(UmiPluginExtensionHostCapabilityNegotiation *value);
int umi_plugin_extension_host_capability_negotiation_compatible(const UmiPluginExtensionHostCapabilityNegotiation *value);

#ifdef __cplusplus
}
#endif

#endif
