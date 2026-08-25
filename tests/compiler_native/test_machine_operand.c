/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_machine_operand.c
 *
 * PURPOSE:
 *   Regression coverage for represent machine registers, immediates, symbols and frame references in target-neutral form.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/machine_operand.h"
int main(void){UmiNativeMachineOperand r=umi_nc_machine_operand_register(5U),i=umi_nc_machine_operand_immediate(7);if(r.register_id!=5U||i.immediate!=7)return 1;UmiNativeMachineOperand s;if(umi_nc_machine_operand_symbol(&s,"puts")!=UMI_STATUS_OK||s.kind!=UMI_NC_MOP_SYMBOL)return 2;return 0;}
