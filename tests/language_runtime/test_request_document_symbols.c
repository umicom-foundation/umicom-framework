/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_request_document_symbols.c
 *
 * PURPOSE:
 *   Verify textDocument/documentSymbol is framed through the real server transport.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "request_test_support.h"
#include "umicom/language_runtime/requests/document_symbols.h"
int main(void){RequestTestFixture f;char b[UMI_LANGUAGE_RUNTIME_FRAME_CAPACITY];size_t n=0;uint64_t id=0;assert(request_test_fixture_create(&f)==UMI_STATUS_OK);assert(umi_language_runtime_request_document_symbols(f.server,"doc","file:///a.c",&id)==UMI_STATUS_OK);assert(umi_language_runtime_memory_transport_written(f.memory,b,sizeof(b),&n)==UMI_STATUS_OK);assert(n>0);assert(strstr(b,"textDocument/documentSymbol")!=NULL);request_test_fixture_destroy(&f);return 0;}
