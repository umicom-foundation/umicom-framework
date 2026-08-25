/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_capability_negotiation.c
 *
 * PURPOSE:
 *   Exercise negotiate requested, optional and host-supported extension capabilities.
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
int main(void) { UmiPluginExtensionHostCapabilityNegotiation n; umi_plugin_extension_host_capability_negotiation_init(&n); n.host_version=3U; n.minimum_version=2U; n.maximum_version=4U; n.required_flags=UINT64_C(3); n.optional_flags=UINT64_C(4); n.supported_flags=UINT64_C(7); if(umi_plugin_extension_host_capability_negotiation_evaluate(&n)!=UMI_STATUS_OK) return 1; if(!umi_plugin_extension_host_capability_negotiation_compatible(&n)||n.negotiated_flags!=UINT64_C(7)) return 2; return 0; }
