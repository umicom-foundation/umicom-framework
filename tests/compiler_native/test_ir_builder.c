/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_ir_builder.c
 *
 * PURPOSE:
 *   Regression coverage for emit constants, arithmetic and return instructions into the current ir basic block.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/ir_builder.h"
int main(void){UmiNativeIrFunction f;UmiNativeIrType t=umi_nc_ir_type_make(UMI_NC_IR_I64,64U,1U);if(umi_nc_ir_function_init(&f,"main",t)!=UMI_STATUS_OK)return 1;uint32_t b=0U;if(umi_nc_ir_function_add_block(&f,"entry",&b)!=UMI_STATUS_OK)return 2;UmiNativeIrBuilder x;if(umi_nc_ir_builder_begin(&x,&f,b)!=UMI_STATUS_OK)return 3;uint32_t a=0U,c=0U,r=0U;if(umi_nc_ir_builder_const_i64(&x,2,&a)!=UMI_STATUS_OK||umi_nc_ir_builder_const_i64(&x,3,&c)!=UMI_STATUS_OK)return 4;if(umi_nc_ir_builder_binary(&x,UMI_NC_IR_ADD,a,c,t,&r)!=UMI_STATUS_OK||umi_nc_ir_builder_return(&x,r)!=UMI_STATUS_OK)return 5;if(!umi_nc_ir_block_terminated(x.block))return 6;return 0;}
