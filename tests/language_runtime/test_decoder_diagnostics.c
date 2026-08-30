/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_decoder_diagnostics.c
 *
 * PURPOSE:
 *   Verify the bounded diagnostics decoder.
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
#include "umicom/language_runtime/decoders/diagnostics.h"
int main(void){UmiLanguageRuntimeDiagnosticList x; assert(umi_language_runtime_decode_diagnostics("{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/publishDiagnostics\",\"params\":{\"uri\":\"file:///a.c\",\"diagnostics\":[{\"range\":{\"start\":{\"line\":2,\"character\":1},\"end\":{\"line\":2,\"character\":2}},\"message\":\"bad\"}]}}",&x)==UMI_STATUS_OK); assert(x.count==1U);return 0;}
