/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/process_resource_budget.h
 *
 * PURPOSE:
 *   Evaluate process CPU-time and memory budgets before admitting more work.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_PROCESS_RESOURCE_BUDGET_H
#define UMICOM_TERMINAL_REMOTE_PROCESS_RESOURCE_BUDGET_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote process resource budget data shared with callers of this
 * public contract.
 */
typedef struct UmiTerminalRemoteProcessResourceBudget { uint64_t max_memory_bytes; uint64_t max_cpu_ms; } UmiTerminalRemoteProcessResourceBudget;
/**
 * Initialise terminal remote process resource budget from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_process_resource_budget_init(UmiTerminalRemoteProcessResourceBudget *value,uint64_t memory_bytes,uint64_t cpu_ms);
/**
 * Provide the terminal remote process resource budget within operation used by this module
 * and its client applications.
 */
bool umi_terminal_remote_process_resource_budget_within(const UmiTerminalRemoteProcessResourceBudget *value,uint64_t memory_bytes,uint64_t cpu_ms);
#ifdef __cplusplus
}
#endif
#endif
