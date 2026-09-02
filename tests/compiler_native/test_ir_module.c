/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_ir_module.c
 *
 * PURPOSE:
 *   Regression coverage for aggregate native ir functions under a target triple and deterministic module revision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/ir_module.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeIrModule m;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_ir_module_init(&m,"m","riscv64-unknown-umicom")!=UMI_STATUS_OK)return 1;UmiNativeIrFunction f;UmiNativeIrType t=umi_nc_ir_type_make(UMI_NC_IR_I32,32U,1U);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_ir_function_init(&f,"main",t)!=UMI_STATUS_OK)return 2;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_ir_module_add_function(&m,&f)!=UMI_STATUS_OK||umi_nc_ir_module_find(&m,"main")==NULL)return 3;return 0;}
