/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/abi_negotiation.c
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
#include "umicom/plugin/extension_host/abi_negotiation.h"
#include <string.h>
/*
 * Initialise plugin extension host abi negotiation from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_abi_negotiation_init(UmiPluginExtensionHostAbiNegotiation *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value!=NULL) memset(value,0,sizeof(*value)); }
/*
 * Provide the plugin extension host abi negotiation evaluate operation used by this module
 * and its client applications.
 */
UmiStatus umi_plugin_extension_host_abi_negotiation_evaluate(UmiPluginExtensionHostAbiNegotiation *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value==NULL||value->minimum_version>value->maximum_version) return UMI_STATUS_INVALID_ARGUMENT; value->missing_flags=value->required_flags & ~value->supported_flags; value->negotiated_flags=(value->required_flags|value->optional_flags)&value->supported_flags; return UMI_STATUS_OK; }
/*
 * Provide the plugin extension host abi negotiation compatible operation used by this
 * module and its client applications.
 */
int umi_plugin_extension_host_abi_negotiation_compatible(const UmiPluginExtensionHostAbiNegotiation *value) { return value!=NULL && value->host_version>=value->minimum_version && value->host_version<=value->maximum_version && value->missing_flags==0U; }
