/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_machine_instruction.c
 *
 * PURPOSE:
 *   Regression coverage for represent selected target-neutral machine operations before textual assembly or object encoding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/machine_instruction.h"
int main(void){UmiNativeMachineInstruction i;if(umi_nc_machine_instruction_init(&i,1U,UMI_NC_MOPC_ADD)!=UMI_STATUS_OK)return 1;if(umi_nc_machine_instruction_add_operand(&i,umi_nc_machine_operand_register(1U))!=UMI_STATUS_OK)return 2;if(i.operand_count!=1U)return 3;return 0;}
