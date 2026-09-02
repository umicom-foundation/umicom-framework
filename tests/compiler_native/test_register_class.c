/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_register_class.c
 *
 * PURPOSE:
 *   Regression coverage for describe target register classes and volatile/callee-saved availability for future allocation passes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/register_class.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeRegisterClass r;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_nc_register_class_default(UMI_NC_ARCH_RISCV64,UMI_NC_REG_GPR,&r)!=UMI_STATUS_OK)return 1;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(r.count!=32U||umi_nc_register_class_usable(&r)==0U)return 2;return 0;}
