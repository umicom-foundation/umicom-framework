/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/provenance_chain.c
 *
 * PURPOSE:
 *   Maintain and verify a deterministic chain of package provenance evidence.
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
#include <string.h>
static uint64_t umi_plugin_extension_host_provenance_chain_mix(uint64_t previous,uint64_t evidence) { uint64_t x=previous ^ (evidence+UINT64_C(0x9e3779b97f4a7c15)+(previous<<6U)+(previous>>2U)); return x==0U?UINT64_C(1):x; }
void umi_plugin_extension_host_provenance_chain_init(UmiPluginExtensionHostProvenanceChain *chain) { if(chain!=NULL) memset(chain,0,sizeof(*chain)); }
UmiStatus umi_plugin_extension_host_provenance_chain_append(UmiPluginExtensionHostProvenanceChain *chain,uint64_t evidence) { size_t i; uint64_t prev; if(chain==NULL||evidence==0U) return UMI_STATUS_INVALID_ARGUMENT; if(chain->count>=UMI_PLUGIN_EXTENSION_HOST_SMALL_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED; i=chain->count; prev=i==0U?0U:chain->entries[i-1U].chain_hash; chain->entries[i].evidence=evidence; chain->entries[i].previous=prev; chain->entries[i].chain_hash=umi_plugin_extension_host_provenance_chain_mix(prev,evidence); ++chain->count; return UMI_STATUS_OK; }
int umi_plugin_extension_host_provenance_chain_verify(const UmiPluginExtensionHostProvenanceChain *chain) { size_t i; uint64_t prev=0U; if(chain==NULL) return 0; for(i=0U;i<chain->count;++i) { if(chain->entries[i].previous!=prev||chain->entries[i].chain_hash!=umi_plugin_extension_host_provenance_chain_mix(prev,chain->entries[i].evidence)) return 0; prev=chain->entries[i].chain_hash; } return 1; }
uint64_t umi_plugin_extension_host_provenance_chain_head(const UmiPluginExtensionHostProvenanceChain *chain) { return chain==NULL||chain->count==0U?0U:chain->entries[chain->count-1U].chain_hash; }
