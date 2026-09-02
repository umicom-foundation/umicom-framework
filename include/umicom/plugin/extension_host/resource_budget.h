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

/**
 * Represent the plugin extension host resource budget data shared with callers of this
 * public contract.
 */
typedef struct UmiPluginExtensionHostResourceBudget { uint64_t memory_bytes; uint64_t cpu_time_ms; uint32_t process_count; uint32_t thread_count; } UmiPluginExtensionHostResourceBudget;
/**
 * Represent the plugin extension host resource budget usage data shared with callers of
 * this public contract.
 */
typedef struct UmiPluginExtensionHostResourceBudgetUsage { uint64_t memory_bytes; uint64_t cpu_time_ms; uint32_t process_count; uint32_t thread_count; } UmiPluginExtensionHostResourceBudgetUsage;
/**
 * Initialise plugin extension host resource budget from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_resource_budget_init(UmiPluginExtensionHostResourceBudget *budget);
/**
 * Provide the plugin extension host resource budget evaluate operation used by this module
 * and its client applications.
 */
UmiPluginExtensionHostDecision umi_plugin_extension_host_resource_budget_evaluate(const UmiPluginExtensionHostResourceBudget *budget, const UmiPluginExtensionHostResourceBudgetUsage *usage);

#ifdef __cplusplus
}
#endif

#endif
