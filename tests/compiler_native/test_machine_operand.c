/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_machine_operand.c
 *
 * PURPOSE:
 *   Regression coverage for represent machine registers, immediates, symbols and frame references in target-neutral form.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/machine_operand.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeMachineOperand r=umi_nc_machine_operand_register(5U),i=umi_nc_machine_operand_immediate(7);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(r.register_id!=5U||i.immediate!=7)return 1;UmiNativeMachineOperand s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_machine_operand_symbol(&s,"puts")!=UMI_STATUS_OK||s.kind!=UMI_NC_MOP_SYMBOL)return 2;return 0;}
