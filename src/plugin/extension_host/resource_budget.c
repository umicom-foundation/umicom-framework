/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/resource_budget.c
 *
 * PURPOSE:
 *   Enforce declared memory, CPU-time, process and thread resource budgets.
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
void umi_plugin_extension_host_resource_budget_init(UmiPluginExtensionHostResourceBudget *b) { if(b!=NULL) { b->memory_bytes=UINT64_C(536870912); b->cpu_time_ms=UINT64_C(60000); b->process_count=4U; b->thread_count=32U; } }
UmiPluginExtensionHostDecision umi_plugin_extension_host_resource_budget_evaluate(const UmiPluginExtensionHostResourceBudget *b,const UmiPluginExtensionHostResourceBudgetUsage *u) { if(b==NULL||u==NULL) return UMI_PLUGIN_EXTENSION_HOST_DENY; if(u->memory_bytes>b->memory_bytes||u->cpu_time_ms>b->cpu_time_ms||u->process_count>b->process_count||u->thread_count>b->thread_count) return UMI_PLUGIN_EXTENSION_HOST_DENY; if(u->memory_bytes>b->memory_bytes*8U/10U||u->cpu_time_ms>b->cpu_time_ms*8U/10U) return UMI_PLUGIN_EXTENSION_HOST_REQUIRE_APPROVAL; return UMI_PLUGIN_EXTENSION_HOST_ALLOW; }
