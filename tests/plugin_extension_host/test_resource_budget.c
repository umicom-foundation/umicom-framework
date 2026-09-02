/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_resource_budget.c
 *
 * PURPOSE:
 *   Exercise enforce declared memory, CPU-time, process and thread resource budgets.
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
#include "umicom/plugin/extension_host/resource_budget.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPluginExtensionHostResourceBudget b; UmiPluginExtensionHostResourceBudgetUsage u={1024U,100U,1U,2U}; umi_plugin_extension_host_resource_budget_init(&b); /* Apply this branch only when its contract condition is satisfied. */ if(umi_plugin_extension_host_resource_budget_evaluate(&b,&u)!=UMI_PLUGIN_EXTENSION_HOST_ALLOW) return 1; u.process_count=10U; /* Apply this branch only when its contract condition is satisfied. */ if(umi_plugin_extension_host_resource_budget_evaluate(&b,&u)!=UMI_PLUGIN_EXTENSION_HOST_DENY) return 2; return 0; }
