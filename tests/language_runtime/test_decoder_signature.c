/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_decoder_signature.c
 *
 * PURPOSE:
 *   Verify the bounded signature decoder.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/language_runtime/decoders/signature.h"
int main(void){UmiLanguageRuntimeSignatureResult x; assert(umi_language_runtime_decode_signature("{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"signatures\":[{\"label\":\"sum(int a)\"}],\"activeParameter\":0}}",&x)==UMI_STATUS_OK); assert(x.available);return 0;}
