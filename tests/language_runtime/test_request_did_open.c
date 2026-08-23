/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_request_did_open.c
 *
 * PURPOSE:
 *   Verify textDocument/didOpen is framed through the real server transport.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "request_test_support.h"
#include "umicom/language_runtime/requests/did_open.h"
int main(void){RequestTestFixture f;char b[UMI_LANGUAGE_RUNTIME_FRAME_CAPACITY];size_t n=0;uint64_t id=0;assert(request_test_fixture_create(&f)==UMI_STATUS_OK);assert(umi_language_runtime_request_did_open(f.server,"file:///a.c","c",1U,"int x;")==UMI_STATUS_OK);assert(umi_language_runtime_memory_transport_written(f.memory,b,sizeof(b),&n)==UMI_STATUS_OK);assert(n>0);assert(strstr(b,"textDocument/didOpen")!=NULL);request_test_fixture_destroy(&f);return 0;}
