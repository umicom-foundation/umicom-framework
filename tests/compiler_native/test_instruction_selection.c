/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_instruction_selection.c
 *
 * PURPOSE:
 *   Regression coverage for lower a practical ir subset into target-neutral machine instructions while reporting unsupported operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/instruction_selection.h"
#include "umicom/compiler/native/ir_builder.h"
int main(void){UmiNativeIrFunction f;UmiNativeIrType t=umi_nc_ir_type_make(UMI_NC_IR_I64,64U,1U);if(umi_nc_ir_function_init(&f,"main",t)!=UMI_STATUS_OK)return 1;uint32_t id=0U;if(umi_nc_ir_function_add_block(&f,"entry",&id)!=UMI_STATUS_OK)return 2;UmiNativeIrBuilder b;if(umi_nc_ir_builder_begin(&b,&f,id)!=UMI_STATUS_OK)return 3;uint32_t v=0U;if(umi_nc_ir_builder_const_i64(&b,42,&v)!=UMI_STATUS_OK||umi_nc_ir_builder_return(&b,v)!=UMI_STATUS_OK)return 4;UmiNativeMachineFunction m;UmiNativeSelectionReport r;if(umi_nc_instruction_select(&f,UMI_NC_ARCH_RISCV64,&m,&r)!=UMI_STATUS_OK)return 5;if(r.lowered!=2U||r.unsupported!=0U)return 6;return 0;}
