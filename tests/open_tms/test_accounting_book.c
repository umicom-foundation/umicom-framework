/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_accounting_book.c
 *
 * PURPOSE:
 *   Verify treasury double-entry balance and approval-before-posting.
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
#include "umicom/open_tms/accounting_book.h"
int main(void)
{
 UmiOpenTmsAccountingBook book;UmiOpenTmsAccountingLine d={0},c={0};
 umi_open_tms_accounting_book_init(&book);
 (void)strcpy(d.accountId,"cash");(void)strcpy(d.currency,"USD");(void)strcpy(d.referenceId,"fxf-1");d.debit=100.0;
 c=d;c.debit=0.0;c.credit=100.0;(void)strcpy(c.accountId,"fx-receivable");
 assert(umi_open_tms_accounting_book_add(&book,&d)==UMI_STATUS_OK);
 assert(umi_open_tms_accounting_book_add(&book,&c)==UMI_STATUS_OK);
 assert(umi_open_tms_accounting_book_approve(&book)==UMI_STATUS_OK);
 assert(umi_open_tms_accounting_book_post(&book)==UMI_STATUS_OK);
 return 0;
}
