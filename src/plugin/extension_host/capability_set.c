/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/capability_set.c
 *
 * PURPOSE:
 *   Maintain bounded capability bitsets and test subset compatibility.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/capability_set.h"
#include <string.h>
void umi_plugin_extension_host_capability_set_init(UmiPluginExtensionHostCapabilitySet *set) { if(set!=NULL) memset(set,0,sizeof(*set)); }
UmiStatus umi_plugin_extension_host_capability_set_add(UmiPluginExtensionHostCapabilitySet *set,uint32_t capability) { uint32_t w,b; if(set==NULL||capability>=256U) return UMI_STATUS_INVALID_ARGUMENT; w=capability/64U; b=capability%64U; set->words[w]|=UINT64_C(1)<<b; ++set->revision; return UMI_STATUS_OK; }
UmiStatus umi_plugin_extension_host_capability_set_remove(UmiPluginExtensionHostCapabilitySet *set,uint32_t capability) { uint32_t w,b; if(set==NULL||capability>=256U) return UMI_STATUS_INVALID_ARGUMENT; w=capability/64U; b=capability%64U; set->words[w]&=~(UINT64_C(1)<<b); ++set->revision; return UMI_STATUS_OK; }
int umi_plugin_extension_host_capability_set_contains(const UmiPluginExtensionHostCapabilitySet *set,uint32_t capability) { uint32_t w,b; if(set==NULL||capability>=256U) return 0; w=capability/64U; b=capability%64U; return (set->words[w]&(UINT64_C(1)<<b))!=0U; }
int umi_plugin_extension_host_capability_set_contains_all(const UmiPluginExtensionHostCapabilitySet *set,const UmiPluginExtensionHostCapabilitySet *required) { size_t i; if(set==NULL||required==NULL) return 0; for(i=0U;i<4U;++i) if((required->words[i]&~set->words[i])!=0U) return 0; return 1; }
