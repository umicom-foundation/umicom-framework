/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_decoder_locations.c
 *
 * PURPOSE:
 *   Verify the bounded locations decoder.
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
#include "umicom/language_runtime/decoders/locations.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiLanguageRuntimeLocationList x; assert(umi_language_runtime_decode_locations("{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":[{\"uri\":\"file:///a.c\",\"range\":{\"start\":{\"line\":1,\"character\":2},\"end\":{\"line\":1,\"character\":3}}}]}",&x)==UMI_STATUS_OK); assert(x.count==1U);return 0;}
