/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_function_decl.c
 *
 * PURPOSE:
 *   Regression coverage for model native function signatures, parameter type ids, variadic state and abi-relevant flags.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/function_decl.h"
int main(void){UmiNativeFunctionDecl d;if(umi_nc_function_decl_init(&d,"sum",1U)!=UMI_STATUS_OK)return 1;if(umi_nc_function_decl_add_parameter(&d,2U)!=UMI_STATUS_OK||umi_nc_function_decl_add_parameter(&d,2U)!=UMI_STATUS_OK)return 2;if(umi_nc_function_decl_signature(&d)==0U)return 3;return 0;}
