/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/resource_budget.h
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_RESOURCE_BUDGET_H
#define UMICOM_PLUGIN_EXTENSION_HOST_RESOURCE_BUDGET_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostResourceBudget { uint64_t memory_bytes; uint64_t cpu_time_ms; uint32_t process_count; uint32_t thread_count; } UmiPluginExtensionHostResourceBudget;
typedef struct UmiPluginExtensionHostResourceBudgetUsage { uint64_t memory_bytes; uint64_t cpu_time_ms; uint32_t process_count; uint32_t thread_count; } UmiPluginExtensionHostResourceBudgetUsage;
void umi_plugin_extension_host_resource_budget_init(UmiPluginExtensionHostResourceBudget *budget);
UmiPluginExtensionHostDecision umi_plugin_extension_host_resource_budget_evaluate(const UmiPluginExtensionHostResourceBudget *budget, const UmiPluginExtensionHostResourceBudgetUsage *usage);

#ifdef __cplusplus
}
#endif

#endif
