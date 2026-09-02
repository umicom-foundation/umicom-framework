/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_ir_block.c
 *
 * PURPOSE:
 *   Regression coverage for store ordered ir instructions and control-flow edges for a basic block.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/ir_block.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeIrBlock b;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_ir_block_init(&b,1U,"entry")!=UMI_STATUS_OK)return 1;UmiNativeIrInstruction i;UmiNativeIrType v=umi_nc_ir_type_make(UMI_NC_IR_VOID,0U,1U);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_ir_instruction_init(&i,1U,UMI_NC_IR_RET,0U,v)!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_ir_block_append(&b,&i)!=UMI_STATUS_OK||!umi_nc_ir_block_terminated(&b))return 3;return 0;}
