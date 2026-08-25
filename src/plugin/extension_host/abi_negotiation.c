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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/abi_negotiation.h"
#include <string.h>
void umi_plugin_extension_host_abi_negotiation_init(UmiPluginExtensionHostAbiNegotiation *value) { if(value!=NULL) memset(value,0,sizeof(*value)); }
UmiStatus umi_plugin_extension_host_abi_negotiation_evaluate(UmiPluginExtensionHostAbiNegotiation *value) { if(value==NULL||value->minimum_version>value->maximum_version) return UMI_STATUS_INVALID_ARGUMENT; value->missing_flags=value->required_flags & ~value->supported_flags; value->negotiated_flags=(value->required_flags|value->optional_flags)&value->supported_flags; return UMI_STATUS_OK; }
int umi_plugin_extension_host_abi_negotiation_compatible(const UmiPluginExtensionHostAbiNegotiation *value) { return value!=NULL && value->host_version>=value->minimum_version && value->host_version<=value->maximum_version && value->missing_flags==0U; }
