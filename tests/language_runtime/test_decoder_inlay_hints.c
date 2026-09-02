/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_decoder_inlay_hints.c
 *
 * PURPOSE:
 *   Verify the bounded inlay hints decoder.
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
#include "umicom/language_runtime/decoders/inlay_hints.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiLanguageRuntimeInlayHintList x; assert(umi_language_runtime_decode_inlay_hints("{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":[{\"position\":{\"line\":1,\"character\":2},\"label\":\": int\"}]}",&x)==UMI_STATUS_OK); assert(x.count==1U);return 0;}
