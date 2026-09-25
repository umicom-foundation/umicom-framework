/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/accounting_book.c
 *
 * PURPOSE:
 *   Implement double-entry validation and approval-before-posting for treasury.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/accounting_book.h"

#include <math.h>
#include <string.h>

void umi_open_tms_accounting_book_init(UmiOpenTmsAccountingBook *book)
{
    if (book == NULL) return;
    (void)memset(book, 0, sizeof(*book));
    book->revision = 1U;
}

UmiStatus umi_open_tms_accounting_book_add(
    UmiOpenTmsAccountingBook *book,
    const UmiOpenTmsAccountingLine *line)
{
    if (book == NULL || line == NULL || line->accountId[0] == '\0' ||
        line->currency[0] == '\0' || line->referenceId[0] == '\0' ||
        book->posted || line->debit < 0.0 || line->credit < 0.0 ||
        (line->debit > 0.0 && line->credit > 0.0) ||
        (line->debit == 0.0 && line->credit == 0.0)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (book->count >= UMI_OPEN_TMS_ACCOUNTING_LINE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    book->lines[book->count++] = *line;
    book->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_open_tms_accounting_book_totals(
    const UmiOpenTmsAccountingBook *book,
    double *outDebit,
    double *outCredit)
{
    size_t index;
    double debit = 0.0;
    double credit = 0.0;

    if (book == NULL || outDebit == NULL || outCredit == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < book->count; ++index) {
        debit += book->lines[index].debit;
        credit += book->lines[index].credit;
    }
    *outDebit = debit;
    *outCredit = credit;
    return UMI_STATUS_OK;
}

UmiStatus umi_open_tms_accounting_book_approve(UmiOpenTmsAccountingBook *book)
{
    double debit;
    double credit;

    if (book == NULL || book->count < 2U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_open_tms_accounting_book_totals(book, &debit, &credit) !=
            UMI_STATUS_OK ||
        fabs(debit - credit) > 0.000001) {
        return UMI_STATUS_INVALID_STATE;
    }
    book->approved = 1;
    book->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_open_tms_accounting_book_post(UmiOpenTmsAccountingBook *book)
{
    if (book == NULL || !book->approved || book->posted) {
        return UMI_STATUS_INVALID_STATE;
    }
    book->posted = 1;
    book->revision += 1U;
    return UMI_STATUS_OK;
}
