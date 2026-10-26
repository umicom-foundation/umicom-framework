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

/* This compatibility book is a local double-valued preview, not the canonical
 * exact-money ledger. A posted flag performs no external or ledger operation.
 * Preserve the absolute legacy tolerance explicitly; applications needing
 * money posting use the exact-currency accounting services after conversion. */
#define UMI_OPEN_TMS_ACCOUNTING_BALANCE_TOLERANCE 0.000001
/* IDs/reference must be nonempty, nonblank, bounded and free of control bytes.
 * Currency is exactly three uppercase ASCII letters (syntax, not an ISO list).
 * Exactly one finite nonnegative debit/credit is positive. Portable text bytes
 * are not a UTF-8 guarantee; native hosts validate encoding before display. */
UmiStatus umi_open_tms_accounting_line_validate(const UmiOpenTmsAccountingLine *line);
/* Validate count before traversal, revision and boolean flags, and every active
 * line. This structural check does not assert per-currency balance. Unused line
 * storage is ignored. Approved/posted state requires at least two lines. */
UmiStatus umi_open_tms_accounting_book_validate(const UmiOpenTmsAccountingBook *book);
/* Check complete finite totals and balance separately for every currency,
 * without approving or modifying the book. Empty/incomplete batches refuse. */
UmiStatus umi_open_tms_accounting_book_check_balance(const UmiOpenTmsAccountingBook *book);
/* Same-currency finite sums only. Unknown currency returns NOT_FOUND. Overflow
 * refuses without publishing partial outputs. All outputs must be disjoint
 * from inputs, one another and the book; serialize owner access. */
UmiStatus umi_open_tms_accounting_book_currency_totals(const UmiOpenTmsAccountingBook *book,
    const char *currency, double *out_debit, double *out_credit);

void umi_open_tms_accounting_book_init(UmiOpenTmsAccountingBook *book);
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
/* Successful addition revokes approval. Posted books cannot be edited. */
UmiStatus umi_open_tms_accounting_book_add(UmiOpenTmsAccountingBook *book,const UmiOpenTmsAccountingLine *line);
/* Compatibility aggregate accepts zero/one currency only; mixed currencies
 * return INVALID_STATE instead of a meaningless cross-currency total. Empty
 * valid books produce zero; all failures preserve both output values. */
UmiStatus umi_open_tms_accounting_book_totals(const UmiOpenTmsAccountingBook *book,double *outDebit,double *outCredit);
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
/* Require both sides and balance within the explicit tolerance separately
 * for every currency. Repeated approval returns ALREADY_EXISTS. */
UmiStatus umi_open_tms_accounting_book_approve(UmiOpenTmsAccountingBook *book);
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
/* Revalidate all rows and per-currency balance before setting the local flag.
 * These public values are caller-owned, not authenticated approval evidence;
 * never treat manually written flags as a grant to post money. */
UmiStatus umi_open_tms_accounting_book_post(UmiOpenTmsAccountingBook *book);
#ifdef __cplusplus
}
#endif
#endif
