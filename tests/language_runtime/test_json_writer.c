/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_json_writer.c
 *
 * PURPOSE:
 *   Verify a focused language-runtime core contract.
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
#include "umicom/language_runtime/json_writer.h"
int main(void){char b[128];UmiLanguageRuntimeJsonWriter w;umi_language_runtime_json_writer_init(&w,b,sizeof(b));assert(umi_language_runtime_json_writer_string(&w,"a\"b\n")==UMI_STATUS_OK);assert(strcmp(b,"\"a\\\"b\\n\"")==0);return 0;}
