/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_process_stream_contract.c
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
#include "umicom/language_runtime/process_stream.h"
int main(void){UmiLanguageRuntimeProcessStreamConfig c={0};UmiLanguageRuntimeProcessStream*s=NULL;assert(umi_language_runtime_process_stream_start(&c,&s)==UMI_STATUS_INVALID_ARGUMENT);assert(s==NULL);return 0;}
