/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_directive.c
 *
 * PURPOSE:
 *   Regression coverage for classify c preprocessor directive lines without taking ownership of macro expansion.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/directive.h"
#include <string.h>
int main(void){ UmiNativeDirective d; if(umi_nc_directive_parse(" #define VALUE 42",&d)!=UMI_STATUS_OK) return 1; if(d.kind!=UMI_NC_DIRECTIVE_DEFINE) return 2; if(strcmp(d.argument,"VALUE 42")!=0) return 3; return 0; }
