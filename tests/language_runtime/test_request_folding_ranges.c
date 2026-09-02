/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_request_folding_ranges.c
 *
 * PURPOSE:
 *   Verify textDocument/foldingRange is framed through the real server transport.
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
#include "umicom/language_runtime/requests/folding_ranges.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){RequestTestFixture f;char b[UMI_LANGUAGE_RUNTIME_FRAME_CAPACITY];size_t n=0;uint64_t id=0;assert(request_test_fixture_create(&f)==UMI_STATUS_OK);assert(umi_language_runtime_request_folding_ranges(f.server,"doc","file:///a.c",&id)==UMI_STATUS_OK);assert(umi_language_runtime_memory_transport_written(f.memory,b,sizeof(b),&n)==UMI_STATUS_OK);assert(n>0);assert(strstr(b,"textDocument/foldingRange")!=NULL);request_test_fixture_destroy(&f);return 0;}
