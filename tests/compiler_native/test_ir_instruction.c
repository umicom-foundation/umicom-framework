/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_ir_instruction.c
 *
 * PURPOSE:
 *   Regression coverage for represent typed ir instructions, operands, side effects and terminators in a compact c23 form.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/ir_instruction.h"
int main(void){UmiNativeIrInstruction i;UmiNativeIrType t=umi_nc_ir_type_make(UMI_NC_IR_I32,32U,1U);if(umi_nc_ir_instruction_init(&i,1U,UMI_NC_IR_ADD,3U,t)!=UMI_STATUS_OK)return 1;if(umi_nc_ir_instruction_add_operand(&i,1U)!=UMI_STATUS_OK||umi_nc_ir_instruction_add_operand(&i,2U)!=UMI_STATUS_OK)return 2;if(!umi_nc_ir_instruction_produces_value(&i)||umi_nc_ir_instruction_is_terminator(&i))return 3;return 0;}
