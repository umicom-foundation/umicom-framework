/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_constant_value.c
 *
 * PURPOSE:
 *   Regression coverage for represent compile-time integer, floating-point and boolean constants without host pointer aliasing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/constant_value.h"
int main(void){UmiNativeConstantValue a=umi_nc_constant_signed(4),b=umi_nc_constant_signed(4);if(!umi_nc_constant_truthy(&a)||!umi_nc_constant_equal(&a,&b))return 1;return 0;}
