/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_decoder_folding_ranges.c
 *
 * PURPOSE:
 *   Verify the bounded folding ranges decoder.
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
#include "umicom/language_runtime/decoders/folding_ranges.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiLanguageRuntimeFoldingRangeList x; assert(umi_language_runtime_decode_folding_ranges("{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":[{\"startLine\":1,\"endLine\":9}]}",&x)==UMI_STATUS_OK); assert(x.count==1U);return 0;}
