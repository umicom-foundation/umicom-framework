/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_contribution_ordering.c
 *
 * PURPOSE:
 *   Exercise produce deterministic contribution ordering using priority and identity.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <string.h>
#include "umicom/plugin/extension_host/contribution_ordering.h"
int main(void) { UmiPluginExtensionHostContributionOrderingItem x[3]={{"b",1U,2U},{"a",3U,3U},{"c",1U,1U}}; if(umi_plugin_extension_host_contribution_ordering_sort(x,3U)!=UMI_STATUS_OK) return 1; if(strcmp(x[0].id,"a")||strcmp(x[1].id,"c")) return 2; return 0; }
