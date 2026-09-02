/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_liveness_analysis.c
 *
 * PURPOSE:
 *   Regression coverage for summarize per-block live-in value pressure for later register allocation work.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/liveness_analysis.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeIrFunction f;UmiNativeIrType t=umi_nc_ir_type_make(UMI_NC_IR_I32,32U,1U);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_ir_function_init(&f,"f",t)!=UMI_STATUS_OK)return 1;uint32_t id=0U;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_ir_function_add_block(&f,"e",&id)!=UMI_STATUS_OK)return 2;UmiNativeIrInstruction i;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_ir_instruction_init(&i,1U,UMI_NC_IR_ADD,2U,t)!=UMI_STATUS_OK)return 3;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_ir_instruction_add_operand(&i,99U)!=UMI_STATUS_OK||umi_nc_ir_block_append(&f.blocks[0],&i)!=UMI_STATUS_OK)return 4;UmiNativeLivenessAnalysis a;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_liveness_analyze(&f,&a)!=UMI_STATUS_OK||a.peak_live_in!=1U)return 5;return 0;}
