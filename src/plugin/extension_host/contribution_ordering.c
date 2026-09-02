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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/contribution_ordering.h"
#include <string.h>
/*
 * Provide the plugin extension host contribution ordering before operation used by this
 * module and its client applications.
 */
int umi_plugin_extension_host_contribution_ordering_before(const UmiPluginExtensionHostContributionOrderingItem *a,const UmiPluginExtensionHostContributionOrderingItem *b) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(a==NULL||b==NULL) return 0; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(a->priority!=b->priority) return a->priority>b->priority; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(a->sequence!=b->sequence) return a->sequence<b->sequence; return strcmp(a->id,b->id)<0; }
/*
 * Provide the plugin extension host contribution ordering sort operation used by this
 * module and its client applications.
 */
UmiStatus umi_plugin_extension_host_contribution_ordering_sort(UmiPluginExtensionHostContributionOrderingItem *items,size_t count) { size_t i,j; UmiPluginExtensionHostContributionOrderingItem tmp; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(items==NULL&&count!=0U) return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for(i=1U;i<count;++i) { tmp=items[i]; j=i; /* Continue only while work remains available; the loop body advances the state on each pass. */ while(j>0U&&umi_plugin_extension_host_contribution_ordering_before(&tmp,&items[j-1U])) { items[j]=items[j-1U]; --j; } items[j]=tmp; } return UMI_STATUS_OK; }
