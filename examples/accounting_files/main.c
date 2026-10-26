/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/accounting_files/main.c
 *
 * PURPOSE:
 *   Demonstrate explicit accounting archive persistence and owned CSV through an installed SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/open_tms/accounting_file.h"
#include "umicom/open_tms/accounting_review.h"
#include "umicom/open_tms/accounting_report.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static UmiStatus Populate(UmiOpenTmsAccountingBook *book) {
    umi_open_tms_accounting_book_init(book);
    UmiOpenTmsAccountingLine line = {0};
    strcpy(line.accountId, "practice-cash"); strcpy(line.referenceId, "synthetic-example");
    strcpy(line.currency, "GBP"); line.debit = 125.5;
    UmiStatus status = umi_open_tms_accounting_book_add(book, &line);
    if (status != UMI_STATUS_OK) return status;
    strcpy(line.accountId, "practice-receivable"); line.debit = 0.0; line.credit = 125.5;
    status = umi_open_tms_accounting_book_add(book, &line);
    UmiOpenTmsAccountingReview *review = NULL;
    if (status == UMI_STATUS_OK)
        status = umi_open_tms_accounting_review_create(book, UMI_OPEN_TMS_ACCOUNTING_APPROVE, &review);
    if (status == UMI_STATUS_OK) status = umi_open_tms_accounting_review_apply(book, review);
    umi_open_tms_accounting_review_destroy(review);
    return status;
}
static UmiStatus Same(const UmiOpenTmsAccountingBook *a, const UmiOpenTmsAccountingBook *b) {
    size_t aSize = 0U, bSize = 0U;
    UmiStatus status = umi_open_tms_accounting_book_archive_encode(a, NULL, 0U, &aSize);
    if (status == UMI_STATUS_OK) status = umi_open_tms_accounting_book_archive_encode(b, NULL, 0U, &bSize);
    if (status != UMI_STATUS_OK) return status;
    if (aSize != bSize) return UMI_STATUS_INTERNAL_ERROR;
    unsigned char *left = malloc(aSize), *right = malloc(bSize);
    if (left == NULL || right == NULL) { free(right); free(left); return UMI_STATUS_OUT_OF_MEMORY; }
    status = umi_open_tms_accounting_book_archive_encode(a, left, aSize, &aSize);
    if (status == UMI_STATUS_OK) status = umi_open_tms_accounting_book_archive_encode(b, right, bSize, &bSize);
    if (status == UMI_STATUS_OK && memcmp(left, right, aSize) != 0) status = UMI_STATUS_INTERNAL_ERROR;
    free(right); free(left); return status;
}
int main(int argc, char **argv) {
    if (argc != 3 || (strcmp(argv[1], "roundtrip") && strcmp(argv[1], "inspect"))) {
        fprintf(stderr, "Usage: accounting-files <roundtrip|inspect> <absolute-archive-path>\n"
            "roundtrip exclusively creates a synthetic local archive; inspect reads an existing archive.\n");
        return 2;
    }
    UmiOpenTmsAccountingBook *book = malloc(sizeof *book), *loaded = malloc(sizeof *loaded);
    if (book == NULL || loaded == NULL) { free(loaded); free(book); return 1; }
    UmiStatus status = UMI_STATUS_OK;
    if (!strcmp(argv[1], "roundtrip")) {
        status = Populate(book);
        UmiOutputFileSnapshot receipt = {0};
        if (status == UMI_STATUS_OK) status = umi_open_tms_accounting_book_save_new(argv[2], book, &receipt);
        if (status == UMI_STATUS_OK && (!receipt.closed || receipt.bytes_written == 0U ||
            receipt.status != UMI_STATUS_OK)) status = UMI_STATUS_INTERNAL_ERROR;
    }
    if (status == UMI_STATUS_OK) status = umi_open_tms_accounting_book_load(argv[2], loaded);
    if (status == UMI_STATUS_OK && !strcmp(argv[1], "roundtrip")) status = Same(book, loaded);
    UmiCsvDocument *report = NULL;
    if (status == UMI_STATUS_OK) status = umi_open_tms_accounting_book_report(loaded, &report);
    if (status == UMI_STATUS_OK) {
        fputs("Local practice capture only; imported flags do not authorise financial actions.\n", stderr);
        if (fwrite(UmiCsvDocumentData(report), 1U, UmiCsvDocumentBytes(report), stdout) !=
            UmiCsvDocumentBytes(report) || fflush(stdout) != 0) status = UMI_STATUS_IO_ERROR;
    }
    if (status != UMI_STATUS_OK)
        fprintf(stderr, "Accounting request refused: %s. Existing or partial files are retained.\n", umi_status_text(status));
    UmiCsvDocumentDestroy(report); free(loaded); free(book);
    return status == UMI_STATUS_OK ? 0 : 1;
}
