/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_dead_code_elimination.c
 *
 * PURPOSE:
 *   Regression coverage for remove unused side-effect-free ir instructions while retaining terminators and observable operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/dead_code_elimination.h"
#include "umicom/compiler/native/ir_builder.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeIrFunction f;UmiNativeIrType t=umi_nc_ir_type_make(UMI_NC_IR_I64,64U,1U);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_ir_function_init(&f,"f",t)!=UMI_STATUS_OK)return 1;uint32_t id=0U;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_ir_function_add_block(&f,"e",&id)!=UMI_STATUS_OK)return 2;UmiNativeIrBuilder b;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_ir_builder_begin(&b,&f,id)!=UMI_STATUS_OK)return 3;uint32_t v=0U;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_ir_builder_const_i64(&b,99,&v)!=UMI_STATUS_OK||umi_nc_ir_builder_return(&b,0U)!=UMI_STATUS_OK)return 4;size_t n=0U;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_dead_code_eliminate(&f,&n)!=UMI_STATUS_OK||n!=1U||f.blocks[0].instruction_count!=1U)return 5;return 0;}
