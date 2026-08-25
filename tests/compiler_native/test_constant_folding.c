/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_constant_folding.c
 *
 * PURPOSE:
 *   Regression coverage for fold integer ir operations whose operands are constants defined in the same function.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/constant_folding.h"
#include "umicom/compiler/native/ir_builder.h"
int main(void){UmiNativeIrFunction f;UmiNativeIrType t=umi_nc_ir_type_make(UMI_NC_IR_I64,64U,1U);if(umi_nc_ir_function_init(&f,"f",t)!=UMI_STATUS_OK)return 1;uint32_t id=0U;if(umi_nc_ir_function_add_block(&f,"e",&id)!=UMI_STATUS_OK)return 2;UmiNativeIrBuilder b;if(umi_nc_ir_builder_begin(&b,&f,id)!=UMI_STATUS_OK)return 3;uint32_t a=0U,c=0U,r=0U;if(umi_nc_ir_builder_const_i64(&b,2,&a)!=UMI_STATUS_OK||umi_nc_ir_builder_const_i64(&b,3,&c)!=UMI_STATUS_OK||umi_nc_ir_builder_binary(&b,UMI_NC_IR_MUL,a,c,t,&r)!=UMI_STATUS_OK)return 4;size_t n=0U;if(umi_nc_constant_fold_function(&f,&n)!=UMI_STATUS_OK||n!=1U||f.blocks[0].instructions[2].immediate!=6)return 5;return 0;}
