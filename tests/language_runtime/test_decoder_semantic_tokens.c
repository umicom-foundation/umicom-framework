/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_decoder_semantic_tokens.c
 *
 * PURPOSE:
 *   Verify the bounded semantic tokens decoder.
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
#include "umicom/language_runtime/decoders/semantic_tokens.h"
int main(void){UmiLanguageRuntimeSemanticTokens x; assert(umi_language_runtime_decode_semantic_tokens("{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"data\":[0,0,3,1,0]}}",&x)==UMI_STATUS_OK); assert(x.count==5U);return 0;}
