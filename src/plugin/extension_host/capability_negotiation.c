/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/capability_negotiation.c
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
#include "umicom/plugin/extension_host/capability_negotiation.h"
#include <string.h>
/*
 * Initialise plugin extension host capability negotiation from caller-provided values so
 * later operations receive a known state.
 */
void umi_plugin_extension_host_capability_negotiation_init(UmiPluginExtensionHostCapabilityNegotiation *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value!=NULL) memset(value,0,sizeof(*value)); }
/*
 * Provide the plugin extension host capability negotiation evaluate operation used by this
 * module and its client applications.
 */
UmiStatus umi_plugin_extension_host_capability_negotiation_evaluate(UmiPluginExtensionHostCapabilityNegotiation *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value==NULL||value->minimum_version>value->maximum_version) return UMI_STATUS_INVALID_ARGUMENT; value->missing_flags=value->required_flags & ~value->supported_flags; value->negotiated_flags=(value->required_flags|value->optional_flags)&value->supported_flags; return UMI_STATUS_OK; }
/*
 * Provide the plugin extension host capability negotiation compatible operation used by
 * this module and its client applications.
 */
int umi_plugin_extension_host_capability_negotiation_compatible(const UmiPluginExtensionHostCapabilityNegotiation *value) { return value!=NULL && value->host_version>=value->minimum_version && value->host_version<=value->maximum_version && value->missing_flags==0U; }
