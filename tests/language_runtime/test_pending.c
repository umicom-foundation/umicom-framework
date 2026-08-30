/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_pending.c
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
#include "umicom/language_runtime/pending.h"
int main(void){UmiLanguageRuntimePendingTable t;UmiLanguageRuntimePendingRequest r;umi_language_runtime_pending_init(&t);assert(umi_language_runtime_pending_add(&t,7,"textDocument/hover","doc")==UMI_STATUS_OK);assert(umi_language_runtime_pending_take(&t,7,&r)==UMI_STATUS_OK);assert(strcmp(r.method,"textDocument/hover")==0);return 0;}
