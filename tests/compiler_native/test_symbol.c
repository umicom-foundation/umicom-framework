/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_symbol.c
 *
 * PURPOSE:
 *   Regression coverage for describe native compiler symbols, linkage and type ownership independently from storage implementation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/symbol.h"
int main(void){UmiNativeSymbol s;if(umi_nc_symbol_init(&s,1U,UMI_NC_SYMBOL_VARIABLE,"x",2U,1U)!=UMI_STATUS_OK)return 1;s.defined=true;if(umi_nc_symbol_validate(&s)!=UMI_STATUS_OK)return 2;return 0;}
