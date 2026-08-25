/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_process_resource_budget.c
 *
 * PURPOSE:
 *   Verify process resource budget rejects either limit breach.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/process_resource_budget.h"
int main(void) { UmiTerminalRemoteProcessResourceBudget v; umi_terminal_remote_process_resource_budget_init(&v,1000U,500U); if(!umi_terminal_remote_process_resource_budget_within(&v,900U,400U)) return 1; return umi_terminal_remote_process_resource_budget_within(&v,1001U,400U)?2:0; }
