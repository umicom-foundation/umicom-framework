/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_macro.c
 *
 * PURPOSE:
 *   Regression coverage for describe object-like and function-like preprocessor macros with stable fingerprints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/macro.h"
int main(void){ UmiNativeMacro m; if(umi_nc_macro_init(&m,"MAX","((a)>(b)?(a):(b))",true)!=UMI_STATUS_OK) return 1; if(umi_nc_macro_add_parameter(&m,"a")!=UMI_STATUS_OK||umi_nc_macro_add_parameter(&m,"b")!=UMI_STATUS_OK) return 2; if(umi_nc_macro_validate(&m)!=UMI_STATUS_OK||m.fingerprint==0U) return 3; return 0; }
