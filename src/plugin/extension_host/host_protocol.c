/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/host_protocol.c
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
#include "umicom/plugin/extension_host/host_protocol.h"
/*
 * Initialise plugin extension host host protocol from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_host_protocol_init(UmiPluginExtensionHostHostProtocol *p,uint32_t version,uint64_t features) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p!=NULL) { p->version=version; p->supported_features=features; p->negotiated_features=0U; p->last_received_sequence=0U; p->next_send_sequence=1U; } }
/*
 * Provide the plugin extension host host protocol negotiate operation used by this module
 * and its client applications.
 */
UmiStatus umi_plugin_extension_host_host_protocol_negotiate(UmiPluginExtensionHostHostProtocol *p,uint32_t peer_version,uint64_t peer_features) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||p->version==0U||peer_version!=p->version) return UMI_STATUS_UNAVAILABLE; p->negotiated_features=p->supported_features&peer_features; return UMI_STATUS_OK; }
/*
 * Provide the plugin extension host host protocol accept sequence operation used by this
 * module and its client applications.
 */
UmiStatus umi_plugin_extension_host_host_protocol_accept_sequence(UmiPluginExtensionHostHostProtocol *p,uint64_t seq) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||seq==0U||seq<=p->last_received_sequence) return UMI_STATUS_INVALID_STATE; p->last_received_sequence=seq; return UMI_STATUS_OK; }
/*
 * Provide the plugin extension host host protocol next sequence operation used by this
 * module and its client applications.
 */
uint64_t umi_plugin_extension_host_host_protocol_next_sequence(UmiPluginExtensionHostHostProtocol *p) { uint64_t v; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL) return 0U; v=p->next_send_sequence; ++p->next_send_sequence; return v; }
