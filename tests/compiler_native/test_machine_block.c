/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_machine_block.c
 *
 * PURPOSE:
 *   Regression coverage for store selected machine instructions under stable block labels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/machine_block.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeMachineBlock b;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_machine_block_init(&b,1U,"entry")!=UMI_STATUS_OK)return 1;UmiNativeMachineInstruction i;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_machine_instruction_init(&i,1U,UMI_NC_MOPC_RET)!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_machine_block_append(&b,&i)!=UMI_STATUS_OK||b.instruction_count!=1U)return 3;return 0;}
