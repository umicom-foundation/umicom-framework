/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/host_handshake.h
 *
 * PURPOSE:
 *   Validate host/plugin protocol and ABI handshake evidence before session activation.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_HOST_HANDSHAKE_H
#define UMICOM_PLUGIN_EXTENSION_HOST_HOST_HANDSHAKE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostHostHandshake { uint32_t host_protocol; uint32_t plugin_protocol; uint32_t host_abi; uint32_t plugin_min_abi; uint32_t plugin_max_abi; uint64_t required_features; uint64_t host_features; int signature_verified; } UmiPluginExtensionHostHostHandshake;
UmiPluginExtensionHostDecision umi_plugin_extension_host_host_handshake_evaluate(const UmiPluginExtensionHostHostHandshake *value, uint64_t *out_negotiated_features);

#ifdef __cplusplus
}
#endif

#endif
