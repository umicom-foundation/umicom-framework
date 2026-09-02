/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_macro.c
 *
 * PURPOSE:
 *   Regression coverage for describe object-like and function-like preprocessor macros with stable fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/macro.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiNativeMacro m; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_macro_init(&m,"MAX","((a)>(b)?(a):(b))",true)!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_macro_add_parameter(&m,"a")!=UMI_STATUS_OK||umi_nc_macro_add_parameter(&m,"b")!=UMI_STATUS_OK) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_macro_validate(&m)!=UMI_STATUS_OK||m.fingerprint==0U) return 3; return 0; }
