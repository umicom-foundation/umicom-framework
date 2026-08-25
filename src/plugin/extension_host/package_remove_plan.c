/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/package_remove_plan.c
 *
 * PURPOSE:
 *   Represent a reviewable extension removal plan that preserves recovery evidence.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/package_remove_plan.h"
#include <string.h>

static void umi_plugin_extension_host_package_remove_plan_copy(char *destination, size_t capacity, const char *source)
{
    size_t i = 0U;
    if (destination == NULL || capacity == 0U) return;
    if (source != NULL) {
        while (i + 1U < capacity && source[i] != '\0') { destination[i] = source[i]; ++i; }
    }
    destination[i] = '\0';
}

void umi_plugin_extension_host_package_remove_plan_init(UmiPluginExtensionHostPackageRemovePlan *plan) { if(plan!=NULL) memset(plan,0,sizeof(*plan)); }
UmiStatus umi_plugin_extension_host_package_remove_plan_add(UmiPluginExtensionHostPackageRemovePlan *plan,const char *id,uint32_t risk,int required)
{ size_t i; if(plan==NULL||!umi_plugin_extension_host_text_valid(id,UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY)||risk>100U) return UMI_STATUS_INVALID_ARGUMENT; if(plan->count>=UMI_PLUGIN_EXTENSION_HOST_SMALL_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED; for(i=0U;i<plan->count;++i) if(strcmp(plan->steps[i].id,id)==0) return UMI_STATUS_ALREADY_EXISTS; i=plan->count++; umi_plugin_extension_host_package_remove_plan_copy(plan->steps[i].id,sizeof(plan->steps[i].id),id); plan->steps[i].risk=risk; plan->steps[i].required=required?1:0; ++plan->revision; return UMI_STATUS_OK; }
UmiStatus umi_plugin_extension_host_package_remove_plan_complete(UmiPluginExtensionHostPackageRemovePlan *plan,const char *id)
{ size_t i; if(plan==NULL||id==NULL) return UMI_STATUS_INVALID_ARGUMENT; for(i=0U;i<plan->count;++i) if(strcmp(plan->steps[i].id,id)==0) { plan->steps[i].completed=1; ++plan->revision; return UMI_STATUS_OK; } return UMI_STATUS_NOT_FOUND; }
int umi_plugin_extension_host_package_remove_plan_ready(const UmiPluginExtensionHostPackageRemovePlan *plan) { size_t i; if(plan==NULL||plan->count==0U) return 0; for(i=0U;i<plan->count;++i) if(plan->steps[i].required && !plan->steps[i].completed) return 0; return 1; }
uint32_t umi_plugin_extension_host_package_remove_plan_maximum_risk(const UmiPluginExtensionHostPackageRemovePlan *plan) { size_t i; uint32_t r=0U; if(plan==NULL) return 0U; for(i=0U;i<plan->count;++i) if(plan->steps[i].risk>r) r=plan->steps[i].risk; return r; }
