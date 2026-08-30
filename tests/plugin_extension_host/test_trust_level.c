/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_trust_level.c
 *
 * PURPOSE:
 *   Exercise calculate extension trust from signature, publisher and provenance evidence.
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
#include "umicom/plugin/extension_host/trust_level.h"
int main(void) { UmiPluginExtensionHostTrustLevelEvidence e={1,1,1,1,0U}; if(umi_plugin_extension_host_trust_level_score(&e)!=100U) return 1; if(umi_plugin_extension_host_trust_level_classify(&e)!=UMI_PLUGIN_EXTENSION_HOST_TRUST_SYSTEM) return 2; e.signature_valid=0; e.publisher_known=0; e.risk=100U; if(umi_plugin_extension_host_trust_level_classify(&e)==UMI_PLUGIN_EXTENSION_HOST_TRUST_TRUSTED) return 3; return 0; }
