/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_decoder_code_actions.c
 *
 * PURPOSE:
 *   Verify the bounded code actions decoder.
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
#include "umicom/language_runtime/decoders/code_actions.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiLanguageRuntimeCodeActionList x; assert(umi_language_runtime_decode_code_actions("{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":[{\"title\":\"Fix\",\"kind\":\"quickfix\"}]}",&x)==UMI_STATUS_OK); assert(x.count==1U);return 0;}
