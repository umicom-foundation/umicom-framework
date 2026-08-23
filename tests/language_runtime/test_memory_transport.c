/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_memory_transport.c
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
#include "umicom/language_runtime/memory_transport.h"
int main(void){UmiLanguageRuntimeMemoryTransport*m;UmiLanguageRuntimeTransport t;char b[16];size_t n=0;assert(umi_language_runtime_memory_transport_create(&m,&t)==UMI_STATUS_OK);assert(t.write(t.instance,"out",3)==UMI_STATUS_OK);assert(umi_language_runtime_memory_transport_written(m,b,sizeof(b),&n)==UMI_STATUS_OK);assert(strcmp(b,"out")==0);t.destroy(t.instance);return 0;}
