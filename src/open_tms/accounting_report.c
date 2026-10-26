/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/accounting_report.c
 *
 * PURPOSE:
 *   Produce a complete owned accounting report through the shared CSV boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/open_tms/accounting_report.h"
#include <math.h>
#include <string.h>

static UmiStatus ReportHeader(UmiCsvDocument *report) {
    const UmiCsvCell cells[] = {
        UmiCsvText("record_kind"), UmiCsvText("revision"), UmiCsvText("approved_local"),
        UmiCsvText("posted_local"), UmiCsvText("line_number"), UmiCsvText("account"),
        UmiCsvText("currency"), UmiCsvText("reference"), UmiCsvText("debit"),
        UmiCsvText("credit"), UmiCsvText("difference"), UmiCsvText("balanced_local")
    };
    return UmiCsvDocumentAppendRow(report, cells, sizeof cells / sizeof cells[0]);
}
static void ReportIdentity(UmiCsvCell cells[12], const UmiOpenTmsAccountingBook *book,
    const char *kind) {
    for (size_t i = 0U; i < 12U; ++i) cells[i] = UmiCsvText("");
    cells[0] = UmiCsvText(kind);
    cells[1] = UmiCsvUnsigned(book->revision);
    cells[2] = UmiCsvText(book->approved ? "yes" : "no");
    cells[3] = UmiCsvText(book->posted ? "yes" : "no");
}
UmiStatus umi_open_tms_accounting_book_report(
    const UmiOpenTmsAccountingBook *book, UmiCsvDocument **outReport) {
    if (outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outReport = NULL;
    UmiStatus status = umi_open_tms_accounting_book_validate(book);
    if (status != UMI_STATUS_OK) return status;
    UmiStatus balance = umi_open_tms_accounting_book_check_balance(book);
    if (balance != UMI_STATUS_OK && balance != UMI_STATUS_INVALID_STATE) return balance;
    if (book->approved && balance != UMI_STATUS_OK) return balance;

    /* Build off to the side. A late invalid text field or overflowing currency
     * must never expose a partially successful report to an application. */
    UmiCsvDocument *report = NULL;
    status = UmiCsvDocumentCreate(UMI_CSV_MAX_BYTES, &report);
    if (status == UMI_STATUS_OK) status = ReportHeader(report);
    UmiCsvCell cells[12];
    ReportIdentity(cells, book, "batch");
    cells[11] = UmiCsvText(balance == UMI_STATUS_OK ? "yes" : "no");
    if (status == UMI_STATUS_OK) status = UmiCsvDocumentAppendRow(report, cells, 12U);

    char currencies[UMI_OPEN_TMS_ACCOUNTING_LINE_CAPACITY][4] = {{0}};
    size_t currencyCount = 0U;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < book->count; ++i) {
        const UmiOpenTmsAccountingLine *line = &book->lines[i];
        ReportIdentity(cells, book, "line");
        cells[4] = UmiCsvUnsigned((uint64_t)i + 1U);
        cells[5] = UmiCsvText(line->accountId);
        cells[6] = UmiCsvText(line->currency);
        cells[7] = UmiCsvText(line->referenceId);
        cells[8] = UmiCsvReal(line->debit);
        cells[9] = UmiCsvReal(line->credit);
        status = UmiCsvDocumentAppendRow(report, cells, 12U);
        size_t at = 0U;
        while (at < currencyCount && strcmp(currencies[at], line->currency) < 0) ++at;
        if (at == currencyCount || strcmp(currencies[at], line->currency) != 0) {
            memmove(currencies + at + 1U, currencies + at,
                (currencyCount - at) * sizeof currencies[0]);
            memcpy(currencies[at], line->currency, sizeof currencies[0]);
            ++currencyCount;
        }
    }
    /* Check every total even when an earlier currency was unbalanced. The
     * balance predicate may stop early, but a report must not hide later overflow. */
    for (size_t i = 0U; status == UMI_STATUS_OK && i < currencyCount; ++i) {
        double debit = 0.0, credit = 0.0;
        status = umi_open_tms_accounting_book_currency_totals(book, currencies[i], &debit, &credit);
        if (status != UMI_STATUS_OK) break;
        ReportIdentity(cells, book, "currency-total");
        cells[6] = UmiCsvText(currencies[i]);
        cells[8] = UmiCsvReal(debit); cells[9] = UmiCsvReal(credit);
        cells[10] = UmiCsvReal(debit - credit);
        cells[11] = UmiCsvText(debit > 0.0 && credit > 0.0 &&
            fabs(debit - credit) <= UMI_OPEN_TMS_ACCOUNTING_BALANCE_TOLERANCE ? "yes" : "no");
        status = UmiCsvDocumentAppendRow(report, cells, 12U);
    }
    if (status != UMI_STATUS_OK) { UmiCsvDocumentDestroy(report); return status; }
    *outReport = report;
    return UMI_STATUS_OK;
}
