/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_arguments.c
 *
 * PURPOSE:
 *   Verify a focused language-runtime core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/language_runtime/arguments.h"
int main(void){UmiLanguageRuntimeArguments a;assert(umi_language_runtime_arguments_parse("--stdio \"C:\\Program Files\\Tool\\config.json\"",&a)==UMI_STATUS_OK);assert(a.count==2);assert(strcmp(a.values[1],"C:\\Program Files\\Tool\\config.json")==0);return 0;}
