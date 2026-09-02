/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_declaration.c
 *
 * PURPOSE:
 *   Regression coverage for describe top-level and local c declarations independently from parser implementation details.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/declaration.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiNativeDeclaration d;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_declaration_init(&d,1U,UMI_NC_DECL_FUNCTION,"main",2U)!=UMI_STATUS_OK)return 1;d.definition=true;d.external_linkage=true;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_declaration_validate(&d)!=UMI_STATUS_OK)return 2;return 0; }
