/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_decoder_signature.c
 *
 * PURPOSE:
 *   Verify the bounded signature decoder.
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
#include "umicom/language_runtime/decoders/signature.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiLanguageRuntimeSignatureResult x; assert(umi_language_runtime_decode_signature("{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"signatures\":[{\"label\":\"sum(int a)\"}],\"activeParameter\":0}}",&x)==UMI_STATUS_OK); assert(x.available);return 0;}
