/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_ir_verifier.c
 *
 * PURPOSE:
 *   Regression coverage for validate ir block termination, branch targets, instruction ids and value-definition uniqueness.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/ir_verifier.h"
#include "umicom/compiler/native/ir_builder.h"
int main(void){UmiNativeIrFunction f;UmiNativeIrType t=umi_nc_ir_type_make(UMI_NC_IR_I64,64U,1U);if(umi_nc_ir_function_init(&f,"f",t)!=UMI_STATUS_OK)return 1;uint32_t id=0U;if(umi_nc_ir_function_add_block(&f,"entry",&id)!=UMI_STATUS_OK)return 2;UmiNativeIrBuilder b;if(umi_nc_ir_builder_begin(&b,&f,id)!=UMI_STATUS_OK)return 3;uint32_t v=0U;if(umi_nc_ir_builder_const_i64(&b,0,&v)!=UMI_STATUS_OK||umi_nc_ir_builder_return(&b,v)!=UMI_STATUS_OK)return 4;UmiNativeIrVerifyReport r;if(umi_nc_ir_verify_function(&f,&r)!=UMI_STATUS_OK||r.errors!=0U)return 5;return 0;}
