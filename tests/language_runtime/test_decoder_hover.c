/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_decoder_hover.c
 *
 * PURPOSE:
 *   Verify the bounded hover decoder.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/language_runtime/decoders/hover.h"
int main(void){UmiLanguageRuntimeHoverResult x; assert(umi_language_runtime_decode_hover("{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"contents\":{\"kind\":\"markdown\",\"value\":\"int value\"}}}",&x)==UMI_STATUS_OK); assert(strstr(x.contents,"int")!=NULL);return 0;}
