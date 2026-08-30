/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_language_id.c
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
#include "umicom/language_runtime/language_id.h"
int main(void){char b[64];assert(umi_language_runtime_normalize_language_id("developer.language.c23",b,sizeof(b))==UMI_STATUS_OK);assert(strcmp(b,"c")==0);return 0;}
