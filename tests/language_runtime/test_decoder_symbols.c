/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_decoder_symbols.c
 *
 * PURPOSE:
 *   Verify the bounded symbols decoder.
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
#include "umicom/language_runtime/decoders/symbols.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiLanguageRuntimeSymbolList x; assert(umi_language_runtime_decode_symbols("{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":[{\"name\":\"main\",\"kind\":12,\"selectionRange\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":4}}}]}","file:///a.c",&x)==UMI_STATUS_OK); assert(x.count==1U);return 0;}
