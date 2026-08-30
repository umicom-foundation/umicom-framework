/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_language_request_ledger.c
 *
 * PURPOSE:
 *   Implement the test language request ledger behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework language request ledger test. Sammy Hegab, Umicom Foundation, MIT. */
#include <assert.h>
#include <string.h>
#include "umicom/language/request_ledger.h"
int main(void){UmiLanguageRequestLedger*l=NULL;UmiLanguageRequestRecord r={0},o;r.request_id=7;strcpy(r.method,"textDocument/hover");assert(umi_language_request_ledger_create(&l)==UMI_STATUS_OK);assert(umi_language_request_ledger_begin(l,&r)==UMI_STATUS_OK);assert(umi_language_request_ledger_pending(l)==1U);assert(umi_language_request_ledger_complete(l,7,UMI_STATUS_OK,99U)==UMI_STATUS_OK);assert(umi_language_request_ledger_find(l,7,&o)==UMI_STATUS_OK&&o.state==UMI_LANGUAGE_REQUEST_COMPLETED);umi_language_request_ledger_destroy(l);return 0;}
