/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_arguments.c
 *
 * PURPOSE:
 *   Verify a focused language-runtime core contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/language_runtime/arguments.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiLanguageRuntimeArguments a;assert(umi_language_runtime_arguments_parse("--stdio \"C:\\Program Files\\Tool\\config.json\"",&a)==UMI_STATUS_OK);assert(a.count==2);assert(strcmp(a.values[1],"C:\\Program Files\\Tool\\config.json")==0);return 0;}
