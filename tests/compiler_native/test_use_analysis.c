/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_use_analysis.c
 *
 * PURPOSE:
 *   Regression coverage for count ir value uses to support dead-code elimination and copy propagation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/use_analysis.h"
#include "umicom/compiler/native/ir_builder.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeIrFunction f;UmiNativeIrType t=umi_nc_ir_type_make(UMI_NC_IR_I64,64U,1U);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_ir_function_init(&f,"f",t)!=UMI_STATUS_OK)return 1;uint32_t id=0U;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_ir_function_add_block(&f,"e",&id)!=UMI_STATUS_OK)return 2;UmiNativeIrBuilder b;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_ir_builder_begin(&b,&f,id)!=UMI_STATUS_OK)return 3;uint32_t a=0U,c=0U,r=0U;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_ir_builder_const_i64(&b,1,&a)!=UMI_STATUS_OK||umi_nc_ir_builder_const_i64(&b,2,&c)!=UMI_STATUS_OK||umi_nc_ir_builder_binary(&b,UMI_NC_IR_ADD,a,c,t,&r)!=UMI_STATUS_OK||umi_nc_ir_builder_return(&b,r)!=UMI_STATUS_OK)return 4;UmiNativeUseAnalysis u;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_use_analyze(&f,&u)!=UMI_STATUS_OK||umi_nc_use_count(&u,a)!=1U||umi_nc_use_count(&u,r)!=1U)return 5;return 0;}
