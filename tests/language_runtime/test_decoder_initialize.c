/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_decoder_initialize.c
 *
 * PURPOSE:
 *   Verify the bounded initialize decoder.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/language_runtime/decoders/initialize.h"
int main(void){UmiLanguageRuntimeInitializeResult x; assert(umi_language_runtime_decode_initialize("{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":{\"completionProvider\":{},\"hoverProvider\":true}}}",&x)==UMI_STATUS_OK); assert(x.completion&&x.hover);return 0;}
