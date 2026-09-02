/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/host_protocol.h
 *
 * PURPOSE:
 *   Maintain protocol sequencing, feature negotiation and replay rejection state.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_HOST_PROTOCOL_H
#define UMICOM_PLUGIN_EXTENSION_HOST_HOST_PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host host protocol data shared with callers of this
 * public contract.
 */
typedef struct UmiPluginExtensionHostHostProtocol { uint32_t version; uint64_t supported_features; uint64_t negotiated_features; uint64_t last_received_sequence; uint64_t next_send_sequence; } UmiPluginExtensionHostHostProtocol;
/**
 * Initialise plugin extension host host protocol from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_host_protocol_init(UmiPluginExtensionHostHostProtocol *protocol, uint32_t version, uint64_t supported_features);
/**
 * Provide the plugin extension host host protocol negotiate operation used by this module
 * and its client applications.
 */
UmiStatus umi_plugin_extension_host_host_protocol_negotiate(UmiPluginExtensionHostHostProtocol *protocol, uint32_t peer_version, uint64_t peer_features);
/**
 * Provide the plugin extension host host protocol accept sequence operation used by this
 * module and its client applications.
 */
UmiStatus umi_plugin_extension_host_host_protocol_accept_sequence(UmiPluginExtensionHostHostProtocol *protocol, uint64_t sequence);
/**
 * Provide the plugin extension host host protocol next sequence operation used by this
 * module and its client applications.
 */
uint64_t umi_plugin_extension_host_host_protocol_next_sequence(UmiPluginExtensionHostHostProtocol *protocol);

#ifdef __cplusplus
}
#endif

#endif
