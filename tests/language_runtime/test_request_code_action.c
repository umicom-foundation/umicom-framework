/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_request_code_action.c
 *
 * PURPOSE:
 *   Verify textDocument/codeAction is framed through the real server transport.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "request_test_support.h"
#include "umicom/language_runtime/requests/code_action.h"
int main(void){RequestTestFixture f;char b[UMI_LANGUAGE_RUNTIME_FRAME_CAPACITY];size_t n=0;uint64_t id=0;UmiLanguageRuntimeRange r={{1U,0U},{2U,0U}};assert(request_test_fixture_create(&f)==UMI_STATUS_OK);assert(umi_language_runtime_request_code_action(f.server,"doc","file:///a.c",&r,&id)==UMI_STATUS_OK);assert(umi_language_runtime_memory_transport_written(f.memory,b,sizeof(b),&n)==UMI_STATUS_OK);assert(n>0);assert(strstr(b,"textDocument/codeAction")!=NULL);request_test_fixture_destroy(&f);return 0;}
