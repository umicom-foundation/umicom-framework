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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/capability_set.h"
#include <string.h>
/*
 * Initialise plugin extension host capability set from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_capability_set_init(UmiPluginExtensionHostCapabilitySet *set) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(set!=NULL) memset(set,0,sizeof(*set)); }
/*
 * Add plugin extension host capability set only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_plugin_extension_host_capability_set_add(UmiPluginExtensionHostCapabilitySet *set,uint32_t capability) { uint32_t w,b; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(set==NULL||capability>=256U) return UMI_STATUS_INVALID_ARGUMENT; w=capability/64U; b=capability%64U; set->words[w]|=UINT64_C(1)<<b; ++set->revision; return UMI_STATUS_OK; }
/*
 * Remove plugin extension host capability set while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_plugin_extension_host_capability_set_remove(UmiPluginExtensionHostCapabilitySet *set,uint32_t capability) { uint32_t w,b; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(set==NULL||capability>=256U) return UMI_STATUS_INVALID_ARGUMENT; w=capability/64U; b=capability%64U; set->words[w]&=~(UINT64_C(1)<<b); ++set->revision; return UMI_STATUS_OK; }
/*
 * Provide the plugin extension host capability set contains operation used by this module
 * and its client applications.
 */
int umi_plugin_extension_host_capability_set_contains(const UmiPluginExtensionHostCapabilitySet *set,uint32_t capability) { uint32_t w,b; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(set==NULL||capability>=256U) return 0; w=capability/64U; b=capability%64U; return (set->words[w]&(UINT64_C(1)<<b))!=0U; }
/*
 * Provide the plugin extension host capability set contains all operation used by this
 * module and its client applications.
 */
int umi_plugin_extension_host_capability_set_contains_all(const UmiPluginExtensionHostCapabilitySet *set,const UmiPluginExtensionHostCapabilitySet *required) { size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(set==NULL||required==NULL) return 0; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<4U;++i) /* Protect caller-owned memory by checking that required state is available before it is used. */ if((required->words[i]&~set->words[i])!=0U) return 0; return 1; }
