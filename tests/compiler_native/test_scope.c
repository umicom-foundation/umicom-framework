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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeScope s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_scope_init(&s,1U,0U,0U)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_scope_add_symbol(&s,3U)!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_nc_scope_contains_symbol(&s,3U))return 3;return 0;}
