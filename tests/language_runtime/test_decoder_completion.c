/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_decoder_completion.c
 *
 * PURPOSE:
 *   Verify the bounded completion decoder.
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
#include "umicom/language_runtime/decoders/completion.h"
int main(void){UmiLanguageRuntimeCompletionResult x; assert(umi_language_runtime_decode_completion("{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":[{\"label\":\"printf\",\"kind\":3}]}",&x)==UMI_STATUS_OK); assert(x.count==1U);return 0;}
