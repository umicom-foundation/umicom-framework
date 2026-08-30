/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_decoder_text_edits.c
 *
 * PURPOSE:
 *   Verify the bounded text edits decoder.
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
#include "umicom/language_runtime/decoders/text_edits.h"
int main(void){UmiLanguageRuntimeTextEditList x; assert(umi_language_runtime_decode_text_edits("{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":1}},\"newText\":\"x\"}]}",&x)==UMI_STATUS_OK); assert(x.count==1U);return 0;}
