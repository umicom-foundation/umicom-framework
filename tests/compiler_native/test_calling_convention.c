/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_calling_convention.c
 *
 * PURPOSE:
 *   Regression coverage for describe register argument, return and stack alignment rules for supported native abis.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/calling_convention.h"
int main(void){UmiNativeTargetProfile t;if(umi_nc_target_profile_init(&t,"riscv64-unknown-umicom")!=UMI_STATUS_OK)return 1;UmiNativeCallingConvention c;if(umi_nc_calling_convention_default(&t,&c)!=UMI_STATUS_OK)return 2;if(c.integer_argument_count!=8U||c.return_register!=10U||c.stack_alignment!=16U)return 3;return 0;}
