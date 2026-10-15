/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/accounting_book.h
 *
 * PURPOSE:
 *   Retain balanced treasury accounting batches before canonical posting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_ACCOUNTING_BOOK_H
#define UMICOM_OPEN_TMS_ACCOUNTING_BOOK_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_OPEN_TMS_ACCOUNTING_LINE_CAPACITY 256U
typedef struct UmiOpenTmsAccountingLine {
    char accountId[64]; char currency[8]; char referenceId[64];
    double debit; double credit;
} UmiOpenTmsAccountingLine;
typedef struct UmiOpenTmsAccountingBook {
    UmiOpenTmsAccountingLine lines[UMI_OPEN_TMS_ACCOUNTING_LINE_CAPACITY];
    size_t count; uint64_t revision; int approved; int posted;
} UmiOpenTmsAccountingBook;
void umi_open_tms_accounting_book_init(UmiOpenTmsAccountingBook *book);
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
UmiStatus umi_open_tms_accounting_book_add(UmiOpenTmsAccountingBook *book,const UmiOpenTmsAccountingLine *line);
UmiStatus umi_open_tms_accounting_book_totals(const UmiOpenTmsAccountingBook *book,double *outDebit,double *outCredit);
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
UmiStatus umi_open_tms_accounting_book_approve(UmiOpenTmsAccountingBook *book);
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
UmiStatus umi_open_tms_accounting_book_post(UmiOpenTmsAccountingBook *book);
#ifdef __cplusplus
}
#endif
#endif
