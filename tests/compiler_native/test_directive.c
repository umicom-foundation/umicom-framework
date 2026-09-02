/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_directive.c
 *
 * PURPOSE:
 *   Regression coverage for classify c preprocessor directive lines without taking ownership of macro expansion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/directive.h"
#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiNativeDirective d; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_directive_parse(" #define VALUE 42",&d)!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(d.kind!=UMI_NC_DIRECTIVE_DEFINE) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(d.argument,"VALUE 42")!=0) return 3; return 0; }
