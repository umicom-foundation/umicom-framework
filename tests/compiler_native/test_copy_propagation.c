/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_copy_propagation.c
 *
 * PURPOSE:
 *   Regression coverage for replace uses of trivial ir copy results with their source values and erase redundant copy instructions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/copy_propagation.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeIrFunction f;UmiNativeIrType t=umi_nc_ir_type_make(UMI_NC_IR_I32,32U,1U);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_ir_function_init(&f,"f",t)!=UMI_STATUS_OK)return 1;uint32_t id=0U;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_ir_function_add_block(&f,"e",&id)!=UMI_STATUS_OK)return 2;UmiNativeIrInstruction c,r;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_ir_instruction_init(&c,1U,UMI_NC_IR_COPY,2U,t)!=UMI_STATUS_OK||umi_nc_ir_instruction_add_operand(&c,1U)!=UMI_STATUS_OK||umi_nc_ir_block_append(&f.blocks[0],&c)!=UMI_STATUS_OK)return 3;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_ir_instruction_init(&r,2U,UMI_NC_IR_RET,0U,t)!=UMI_STATUS_OK||umi_nc_ir_instruction_add_operand(&r,2U)!=UMI_STATUS_OK||umi_nc_ir_block_append(&f.blocks[0],&r)!=UMI_STATUS_OK)return 4;size_t a=0U,b=0U;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_copy_propagate(&f,&a,&b)!=UMI_STATUS_OK||a!=1U||b!=1U||f.blocks[0].instructions[0].operands[0]!=1U)return 5;return 0;}
