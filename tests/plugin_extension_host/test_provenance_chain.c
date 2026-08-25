/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_provenance_chain.c
 *
 * PURPOSE:
 *   Exercise maintain and verify a deterministic chain of package provenance evidence.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/provenance_chain.h"
int main(void) { UmiPluginExtensionHostProvenanceChain c; umi_plugin_extension_host_provenance_chain_init(&c); if(umi_plugin_extension_host_provenance_chain_append(&c,11U)!=UMI_STATUS_OK||umi_plugin_extension_host_provenance_chain_append(&c,22U)!=UMI_STATUS_OK) return 1; if(!umi_plugin_extension_host_provenance_chain_verify(&c)||umi_plugin_extension_host_provenance_chain_head(&c)==0U) return 2; c.entries[1].evidence=23U; if(umi_plugin_extension_host_provenance_chain_verify(&c)) return 3; return 0; }
