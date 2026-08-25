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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiPluginExtensionHostHostProtocol { uint32_t version; uint64_t supported_features; uint64_t negotiated_features; uint64_t last_received_sequence; uint64_t next_send_sequence; } UmiPluginExtensionHostHostProtocol;
void umi_plugin_extension_host_host_protocol_init(UmiPluginExtensionHostHostProtocol *protocol, uint32_t version, uint64_t supported_features);
UmiStatus umi_plugin_extension_host_host_protocol_negotiate(UmiPluginExtensionHostHostProtocol *protocol, uint32_t peer_version, uint64_t peer_features);
UmiStatus umi_plugin_extension_host_host_protocol_accept_sequence(UmiPluginExtensionHostHostProtocol *protocol, uint64_t sequence);
uint64_t umi_plugin_extension_host_host_protocol_next_sequence(UmiPluginExtensionHostHostProtocol *protocol);

#ifdef __cplusplus
}
#endif

#endif
