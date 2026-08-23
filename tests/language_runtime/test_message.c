/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_message.c
 *
 * PURPOSE:
 *   Verify a focused language-runtime core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/language_runtime/message.h"
int main(void){char b[256];UmiLanguageRuntimeEnvelope e;assert(umi_language_runtime_build_request(3,"hover","{}",b,sizeof(b))==UMI_STATUS_OK);assert(strstr(b,"\"id\":3"));assert(umi_language_runtime_message_parse("{\"jsonrpc\":\"2.0\",\"id\":3,\"result\":null}",&e)==UMI_STATUS_OK);assert(e.request_id==3);return 0;}
