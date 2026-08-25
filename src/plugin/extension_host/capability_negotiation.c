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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/capability_negotiation.h"
#include <string.h>
void umi_plugin_extension_host_capability_negotiation_init(UmiPluginExtensionHostCapabilityNegotiation *value) { if(value!=NULL) memset(value,0,sizeof(*value)); }
UmiStatus umi_plugin_extension_host_capability_negotiation_evaluate(UmiPluginExtensionHostCapabilityNegotiation *value) { if(value==NULL||value->minimum_version>value->maximum_version) return UMI_STATUS_INVALID_ARGUMENT; value->missing_flags=value->required_flags & ~value->supported_flags; value->negotiated_flags=(value->required_flags|value->optional_flags)&value->supported_flags; return UMI_STATUS_OK; }
int umi_plugin_extension_host_capability_negotiation_compatible(const UmiPluginExtensionHostCapabilityNegotiation *value) { return value!=NULL && value->host_version>=value->minimum_version && value->host_version<=value->maximum_version && value->missing_flags==0U; }
