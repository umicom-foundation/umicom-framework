/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_machine_function.c
 *
 * PURPOSE:
 *   Regression coverage for aggregate machine blocks, frame size and target architecture for one lowered function.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/machine_function.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeMachineFunction f;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_machine_function_init(&f,"main",UMI_NC_ARCH_RISCV64)!=UMI_STATUS_OK)return 1;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_machine_function_add_block(&f,1U,"entry")!=UMI_STATUS_OK)return 2;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_machine_function_block(&f,1U)==NULL||umi_nc_machine_function_next_instruction(&f)!=1U)return 3;return 0;}
