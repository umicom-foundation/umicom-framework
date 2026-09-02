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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <string.h>
#include "umicom/plugin/extension_host/contribution_ordering.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPluginExtensionHostContributionOrderingItem x[3]={{"b",1U,2U},{"a",3U,3U},{"c",1U,1U}}; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_contribution_ordering_sort(x,3U)!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(x[0].id,"a")||strcmp(x[1].id,"c")) return 2; return 0; }
