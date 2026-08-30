/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_request_workspace_symbols.c
 *
 * PURPOSE:
 *   Verify workspace/symbol is framed through the real server transport.
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
#include "request_test_support.h"
#include "umicom/language_runtime/requests/workspace_symbols.h"
int main(void){RequestTestFixture f;char b[UMI_LANGUAGE_RUNTIME_FRAME_CAPACITY];size_t n=0;uint64_t id=0;assert(request_test_fixture_create(&f)==UMI_STATUS_OK);assert(umi_language_runtime_request_workspace_symbols(f.server,"main",&id)==UMI_STATUS_OK);assert(umi_language_runtime_memory_transport_written(f.memory,b,sizeof(b),&n)==UMI_STATUS_OK);assert(n>0);assert(strstr(b,"workspace/symbol")!=NULL);request_test_fixture_destroy(&f);return 0;}
