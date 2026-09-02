/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/package_activation_plan.c
 *
 * PURPOSE:
 *   Represent activation stages after verification, dependency and permission checks.
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
#include "umicom/plugin/extension_host/package_activation_plan.h"
#include <string.h>

/*
 * Copy plugin extension host package activation plan into module-owned storage so callers
 * keep ownership of their input values.
 */
static void umi_plugin_extension_host_package_activation_plan_copy(char *destination, size_t capacity, const char *source)
{
    size_t i = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source != NULL) {
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (i + 1U < capacity && source[i] != '\0') { destination[i] = source[i]; ++i; }
    }
    destination[i] = '\0';
}

/*
 * Initialise plugin extension host package activation plan from caller-provided values so
 * later operations receive a known state.
 */
void umi_plugin_extension_host_package_activation_plan_init(UmiPluginExtensionHostPackageActivationPlan *plan) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(plan!=NULL) memset(plan,0,sizeof(*plan)); }
/*
 * Add plugin extension host package activation plan only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_plugin_extension_host_package_activation_plan_add(UmiPluginExtensionHostPackageActivationPlan *plan,const char *id,uint32_t risk,int required)
{ size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(plan==NULL||!umi_plugin_extension_host_text_valid(id,UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY)||risk>100U) return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(plan->count>=UMI_PLUGIN_EXTENSION_HOST_SMALL_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<plan->count;++i) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(plan->steps[i].id,id)==0) return UMI_STATUS_ALREADY_EXISTS; i=plan->count++; umi_plugin_extension_host_package_activation_plan_copy(plan->steps[i].id,sizeof(plan->steps[i].id),id); plan->steps[i].risk=risk; plan->steps[i].required=required?1:0; ++plan->revision; return UMI_STATUS_OK; }
/*
 * Provide the plugin extension host package activation plan complete operation used by
 * this module and its client applications.
 */
UmiStatus umi_plugin_extension_host_package_activation_plan_complete(UmiPluginExtensionHostPackageActivationPlan *plan,const char *id)
{ size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(plan==NULL||id==NULL) return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<plan->count;++i) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(plan->steps[i].id,id)==0) { plan->steps[i].completed=1; ++plan->revision; return UMI_STATUS_OK; } return UMI_STATUS_NOT_FOUND; }
/*
 * Provide the plugin extension host package activation plan ready operation used by this
 * module and its client applications.
 */
int umi_plugin_extension_host_package_activation_plan_ready(const UmiPluginExtensionHostPackageActivationPlan *plan) { size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(plan==NULL||plan->count==0U) return 0; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<plan->count;++i) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(plan->steps[i].required && !plan->steps[i].completed) return 0; return 1; }
/*
 * Provide the plugin extension host package activation plan maximum risk operation used by
 * this module and its client applications.
 */
uint32_t umi_plugin_extension_host_package_activation_plan_maximum_risk(const UmiPluginExtensionHostPackageActivationPlan *plan) { size_t i; uint32_t r=0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(plan==NULL) return 0U; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<plan->count;++i) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(plan->steps[i].risk>r) r=plan->steps[i].risk; return r; }
