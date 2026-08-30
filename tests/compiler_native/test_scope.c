/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_scope.c
 *
 * PURPOSE:
 *   Regression coverage for represent lexical scope ancestry and bounded symbol membership for c name lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/scope.h"
int main(void){UmiNativeScope s;if(umi_nc_scope_init(&s,1U,0U,0U)!=UMI_STATUS_OK)return 1;if(umi_nc_scope_add_symbol(&s,3U)!=UMI_STATUS_OK)return 2;if(!umi_nc_scope_contains_symbol(&s,3U))return 3;return 0;}
