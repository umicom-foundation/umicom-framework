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
int main(void){UmiNativeRegisterClass r;if(umi_nc_register_class_default(UMI_NC_ARCH_RISCV64,UMI_NC_REG_GPR,&r)!=UMI_STATUS_OK)return 1;if(r.count!=32U||umi_nc_register_class_usable(&r)==0U)return 2;return 0;}
