/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_reconciliation_book.c
 *
 * PURPOSE:
 *   Verify treasury matching, break resolution and coverage.
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
#include "umicom/open_tms/reconciliation_book.h"
int main(void)
{
 UmiOpenTmsReconciliationBook book;UmiOpenTmsReconciliationItem a={0},b={0};
 umi_open_tms_reconciliation_book_init(&book);(void)strcpy(a.id,"a");(void)strcpy(a.sourceId,"s");(void)strcpy(a.targetId,"t");a.sourceValue=100.0;a.targetValue=100.1;a.tolerance=.5;
 b=a;(void)strcpy(b.id,"b");b.targetValue=102.0;
 assert(umi_open_tms_reconciliation_book_add(&book,&a)==UMI_STATUS_OK);assert(umi_open_tms_reconciliation_book_add(&book,&b)==UMI_STATUS_OK);
 assert(book.breakCount==1U);assert(umi_open_tms_reconciliation_book_resolve(&book,"b")==UMI_STATUS_OK);assert(umi_open_tms_reconciliation_book_coverage(&book)>99.9);return 0;
}
