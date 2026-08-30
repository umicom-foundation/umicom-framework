/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_constant_eval.c
 *
 * PURPOSE:
 *   Regression coverage for evaluate a safe integer subset of c constant expressions for folding and conditional compilation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/constant_eval.h"
int main(void){UmiNativeConstantValue a=umi_nc_constant_signed(7),b=umi_nc_constant_signed(5),o;if(umi_nc_constant_eval_binary(UMI_NC_CONST_ADD,&a,&b,&o)!=UMI_STATUS_OK||o.signed_value!=12)return 1;if(umi_nc_constant_eval_binary(UMI_NC_CONST_GT,&a,&b,&o)!=UMI_STATUS_OK||!o.bool_value)return 2;return 0;}
