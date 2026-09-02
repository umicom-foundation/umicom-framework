/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_symbol.c
 *
 * PURPOSE:
 *   Regression coverage for describe native compiler symbols, linkage and type ownership independently from storage implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/symbol.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeSymbol s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_symbol_init(&s,1U,UMI_NC_SYMBOL_VARIABLE,"x",2U,1U)!=UMI_STATUS_OK)return 1;s.defined=true;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_symbol_validate(&s)!=UMI_STATUS_OK)return 2;return 0;}
