/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/process_resource_budget.c
 *
 * PURPOSE:
 *   Implement resource-budget admission.
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
#include "umicom/terminal/remote/process_resource_budget.h"
void umi_terminal_remote_process_resource_budget_init(UmiTerminalRemoteProcessResourceBudget *value,uint64_t memory_bytes,uint64_t cpu_ms) { if(value) { value->max_memory_bytes=memory_bytes; value->max_cpu_ms=cpu_ms; } }
bool umi_terminal_remote_process_resource_budget_within(const UmiTerminalRemoteProcessResourceBudget *value,uint64_t memory_bytes,uint64_t cpu_ms) { return value&&memory_bytes<=value->max_memory_bytes&&cpu_ms<=value->max_cpu_ms; }
