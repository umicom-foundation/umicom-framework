/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_package_remove_plan.c
 *
 * PURPOSE:
 *   Exercise represent a reviewable extension removal plan that preserves recovery evidence.
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
int main(void) { UmiPluginExtensionHostPackageRemovePlan p; umi_plugin_extension_host_package_remove_plan_init(&p); if(umi_plugin_extension_host_package_remove_plan_add(&p,"verify",20U,1)!=UMI_STATUS_OK) return 1; if(umi_plugin_extension_host_package_remove_plan_ready(&p)) return 2; if(umi_plugin_extension_host_package_remove_plan_complete(&p,"verify")!=UMI_STATUS_OK) return 3; if(!umi_plugin_extension_host_package_remove_plan_ready(&p)||umi_plugin_extension_host_package_remove_plan_maximum_risk(&p)!=20U) return 4; return 0; }
