/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_machine_block.c
 *
 * PURPOSE:
 *   Regression coverage for store selected machine instructions under stable block labels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/machine_block.h"
int main(void){UmiNativeMachineBlock b;if(umi_nc_machine_block_init(&b,1U,"entry")!=UMI_STATUS_OK)return 1;UmiNativeMachineInstruction i;if(umi_nc_machine_instruction_init(&i,1U,UMI_NC_MOPC_RET)!=UMI_STATUS_OK)return 2;if(umi_nc_machine_block_append(&b,&i)!=UMI_STATUS_OK||b.instruction_count!=1U)return 3;return 0;}
