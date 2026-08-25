/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/contribution_ordering.c
 *
 * PURPOSE:
 *   Produce deterministic contribution ordering using priority and identity.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/contribution_ordering.h"
#include <string.h>
int umi_plugin_extension_host_contribution_ordering_before(const UmiPluginExtensionHostContributionOrderingItem *a,const UmiPluginExtensionHostContributionOrderingItem *b) { if(a==NULL||b==NULL) return 0; if(a->priority!=b->priority) return a->priority>b->priority; if(a->sequence!=b->sequence) return a->sequence<b->sequence; return strcmp(a->id,b->id)<0; }
UmiStatus umi_plugin_extension_host_contribution_ordering_sort(UmiPluginExtensionHostContributionOrderingItem *items,size_t count) { size_t i,j; UmiPluginExtensionHostContributionOrderingItem tmp; if(items==NULL&&count!=0U) return UMI_STATUS_INVALID_ARGUMENT; for(i=1U;i<count;++i) { tmp=items[i]; j=i; while(j>0U&&umi_plugin_extension_host_contribution_ordering_before(&tmp,&items[j-1U])) { items[j]=items[j-1U]; --j; } items[j]=tmp; } return UMI_STATUS_OK; }
