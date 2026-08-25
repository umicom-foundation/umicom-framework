/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_branch_simplify.c
 *
 * PURPOSE:
 *   Regression coverage for simplify conditional branches whose condition is a compile-time ir constant.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/branch_simplify.h"
int main(void){UmiNativeIrFunction f;UmiNativeIrType t=umi_nc_ir_type_make(UMI_NC_IR_I1,1U,1U);if(umi_nc_ir_function_init(&f,"f",t)!=UMI_STATUS_OK)return 1;uint32_t a=0U,b=0U,c=0U;if(umi_nc_ir_function_add_block(&f,"a",&a)!=UMI_STATUS_OK||umi_nc_ir_function_add_block(&f,"b",&b)!=UMI_STATUS_OK||umi_nc_ir_function_add_block(&f,"c",&c)!=UMI_STATUS_OK)return 2;UmiNativeIrInstruction k,j;if(umi_nc_ir_instruction_init(&k,1U,UMI_NC_IR_CONST,1U,t)!=UMI_STATUS_OK)return 3;k.immediate=1;if(umi_nc_ir_block_append(&f.blocks[0],&k)!=UMI_STATUS_OK)return 4;if(umi_nc_ir_instruction_init(&j,2U,UMI_NC_IR_CBR,0U,t)!=UMI_STATUS_OK)return 5;if(umi_nc_ir_instruction_add_operand(&j,1U)!=UMI_STATUS_OK||umi_nc_ir_instruction_add_operand(&j,b)!=UMI_STATUS_OK||umi_nc_ir_instruction_add_operand(&j,c)!=UMI_STATUS_OK||umi_nc_ir_block_append(&f.blocks[0],&j)!=UMI_STATUS_OK)return 6;size_t n=0U;if(umi_nc_branch_simplify(&f,&n)!=UMI_STATUS_OK||n!=1U||f.blocks[0].instructions[1].opcode!=UMI_NC_IR_BR||f.blocks[0].instructions[1].operands[0]!=b)return 7;return 0;}
