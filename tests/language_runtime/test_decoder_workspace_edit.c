/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_decoder_workspace_edit.c
 *
 * PURPOSE:
 *   Verify the bounded workspace edit decoder.
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
#include "umicom/language_runtime/decoders/workspace_edit.h"
int main(void){UmiLanguageRuntimeWorkspaceEdit x; assert(umi_language_runtime_decode_workspace_edit("{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"changes\":{\"file:///a.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":1}},\"newText\":\"x\"}]}}}",&x)==UMI_STATUS_OK); assert(x.count==1U);return 0;}
