/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/host_handshake.c
 *
 * PURPOSE:
 *   Validate host/plugin protocol and ABI handshake evidence before session activation.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/host_handshake.h"
UmiPluginExtensionHostDecision umi_plugin_extension_host_host_handshake_evaluate(const UmiPluginExtensionHostHostHandshake *v,uint64_t *out) { if(out!=NULL) *out=0U; if(v==NULL||v->host_protocol==0U||v->host_protocol!=v->plugin_protocol||v->plugin_min_abi>v->plugin_max_abi||v->host_abi<v->plugin_min_abi||v->host_abi>v->plugin_max_abi||(v->required_features&~v->host_features)!=0U) return UMI_PLUGIN_EXTENSION_HOST_DENY; if(out!=NULL) *out=v->required_features&v->host_features; return v->signature_verified?UMI_PLUGIN_EXTENSION_HOST_ALLOW:UMI_PLUGIN_EXTENSION_HOST_REQUIRE_APPROVAL; }
