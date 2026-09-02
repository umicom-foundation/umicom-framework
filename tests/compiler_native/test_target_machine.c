/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_target_machine.c
 *
 * PURPOSE:
 *   Regression coverage for combine target and optimization profiles into a validated native code-generation machine contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/target_machine.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeTargetProfile t;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_target_profile_init(&t,"x86_64-pc-windows")!=UMI_STATUS_OK)return 1;UmiNativeOptimizationProfile o=umi_nc_optimization_profile_default(UMI_NC_OPT_O2);UmiNativeTargetMachine m;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_target_machine_init(&m,&t,&o)!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(m.integer_registers!=16U||!umi_nc_target_machine_supports_opcode(&m,UMI_NC_IR_ADD))return 3;return 0;}
