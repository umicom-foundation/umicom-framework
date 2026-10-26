/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_accounting_report.c
 *
 * PURPOSE:
 *   Check complete accounting exports, unsafe input refusal and independent snapshot ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/open_tms/accounting_report.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); return 1; } } while (0)

static UmiStatus Add(UmiOpenTmsAccountingBook *book, const char *currency, double debit, double credit) {
    UmiOpenTmsAccountingLine line = {0};
    strcpy(line.accountId, "practice"); strcpy(line.referenceId, "example");
    strcpy(line.currency, currency); line.debit = debit; line.credit = credit;
    return umi_open_tms_accounting_book_add(book, &line);
}
int main(int argc, char **argv) {
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiOpenTmsAccountingBook *book = malloc(sizeof *book), *before = malloc(sizeof *before);
    CHECK(book && before);
    umi_open_tms_accounting_book_init(book);
    CHECK(Add(book, "GBP", 12.5, 0.0) == UMI_STATUS_OK);
    CHECK(Add(book, "GBP", 0.0, 12.5) == UMI_STATUS_OK);
    int refusal = 0;
    size_t expectedRows = 5U;
    if (!strcmp(mode, "empty")) { umi_open_tms_accounting_book_init(book); expectedRows = 2U; }
    else if (!strcmp(mode, "balanced") || !strcmp(mode, "snapshot") || !strcmp(mode, "short-copy")) {}
    else if (!strcmp(mode, "unbalanced")) book->lines[1].credit = 11.0;
    else if (!strcmp(mode, "cross-currency")) { strcpy(book->lines[1].currency, "USD"); expectedRows = 6U; }
    else if (!strcmp(mode, "currency-order")) {
        strcpy(book->lines[0].currency, "USD"); strcpy(book->lines[1].currency, "USD");
        CHECK(Add(book, "GBP", 7.0, 0.0) == UMI_STATUS_OK);
        CHECK(Add(book, "GBP", 0.0, 7.0) == UMI_STATUS_OK); expectedRows = 8U;
    } else if (!strcmp(mode, "approved") || !strcmp(mode, "posted")) {
        CHECK(umi_open_tms_accounting_book_approve(book) == UMI_STATUS_OK);
        if (!strcmp(mode, "posted")) CHECK(umi_open_tms_accounting_book_post(book) == UMI_STATUS_OK);
    } else if (!strcmp(mode, "maximum-lines")) {
        for (size_t i = book->count; i < UMI_OPEN_TMS_ACCOUNTING_LINE_CAPACITY; ++i)
            CHECK(Add(book, "GBP", i % 2U ? 0.0 : 12.5, i % 2U ? 12.5 : 0.0) == UMI_STATUS_OK);
        expectedRows = 2U + UMI_OPEN_TMS_ACCOUNTING_LINE_CAPACITY + 1U;
    } else if (!strcmp(mode, "formula-account")) strcpy(book->lines[0].accountId, "=1+1");
    else if (!strcmp(mode, "formula-reference")) strcpy(book->lines[1].referenceId, "  @example");
    else if (!strcmp(mode, "quoted-text")) strcpy(book->lines[0].referenceId, "a,\"b\"");
    else if (!strcmp(mode, "utf8")) strcpy(book->lines[0].accountId, "caf\xc3\xa9");
    else if (!strcmp(mode, "maximum-revision")) book->revision = UINT64_MAX;
    else if (!strcmp(mode, "null-output")) {
        CHECK(umi_open_tms_accounting_book_report(book, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        free(before); free(book); return 0;
    } else {
        refusal = 1;
        if (!strcmp(mode, "invalid-utf8")) { book->lines[1].referenceId[0] = (char)0xff; book->lines[1].referenceId[1] = '\0'; }
        else if (!strcmp(mode, "nonterminated")) memset(book->lines[1].referenceId, 'a', sizeof book->lines[1].referenceId);
        else if (!strcmp(mode, "bad-currency")) strcpy(book->lines[0].currency, "gbp");
        else if (!strcmp(mode, "nan")) book->lines[0].debit = NAN;
        else if (!strcmp(mode, "infinity")) book->lines[0].debit = INFINITY;
        else if (!strcmp(mode, "negative")) book->lines[0].debit = -1.0;
        else if (!strcmp(mode, "both-sides")) book->lines[0].credit = 1.0;
        else if (!strcmp(mode, "zero-sides")) book->lines[0].debit = 0.0;
        else if (!strcmp(mode, "invalid-count")) book->count = UMI_OPEN_TMS_ACCOUNTING_LINE_CAPACITY + 1U;
        else if (!strcmp(mode, "invalid-revision")) book->revision = 0U;
        else if (!strcmp(mode, "invalid-flags")) book->posted = 1;
        else if (!strcmp(mode, "unbalanced-approved")) { book->approved = 1; book->lines[1].credit = 11.0; }
        else if (!strcmp(mode, "overflow")) {
            book->lines[0].debit = DBL_MAX; book->lines[1].credit = 0.0; book->lines[1].debit = DBL_MAX;
        } else if (!strcmp(mode, "late-overflow")) {
            book->lines[1].credit = 11.0;
            CHECK(Add(book, "USD", DBL_MAX, 0.0) == UMI_STATUS_OK);
            CHECK(Add(book, "USD", DBL_MAX, 0.0) == UMI_STATUS_OK);
        } else if (strcmp(mode, "null-book")) { free(before); free(book); return 2; }
    }
    memcpy(before, book, sizeof *before);
    UmiCsvDocument *sentinel = NULL;
    CHECK(UmiCsvDocumentCreate(64U, &sentinel) == UMI_STATUS_OK);
    UmiCsvDocument *report = sentinel;
    UmiStatus status = umi_open_tms_accounting_book_report(!strcmp(mode, "null-book") ? NULL : book, &report);
    CHECK(memcmp(book, before, sizeof *book) == 0);
    if (refusal) CHECK(status != UMI_STATUS_OK && report == NULL);
    else {
        CHECK(status == UMI_STATUS_OK && report != NULL && report != sentinel);
        CHECK(UmiCsvDocumentRows(report) == expectedRows);
        const char *text = UmiCsvDocumentData(report);
        CHECK(text != NULL && strstr(text, "\"record_kind\",\"revision\",\"approved_local\"") == text);
        CHECK(strstr(text, "\"batch\"") != NULL);
        CHECK(strstr(text, "grand-total") == NULL);
        if (!strcmp(mode, "balanced")) {
            CHECK(strstr(text, "\"line\",\"3\",\"no\",\"no\",\"1\",\"practice\",\"GBP\",\"example\",\"12.5\",\"0\",\"\",\"\"") != NULL);
            CHECK(strstr(text, "\"currency-total\",\"3\",\"no\",\"no\",\"\",\"\",\"GBP\",\"\",\"12.5\",\"12.5\",\"0\",\"yes\"") != NULL);
        } else if (!strcmp(mode, "empty")) CHECK(strstr(text, "\"line\"") == NULL && strstr(text, "\"currency-total\"") == NULL);
        else if (!strcmp(mode, "unbalanced"))
            CHECK(strstr(text, "\"GBP\",\"\",\"12.5\",\"11\",\"1.5\",\"no\"") != NULL);
        else if (!strcmp(mode, "cross-currency"))
            CHECK(strstr(text, "\"GBP\",\"\",\"12.5\",\"0\",\"12.5\",\"no\"") && strstr(text, "\"USD\",\"\",\"0\",\"12.5\",\"-12.5\",\"no\""));
        else if (!strcmp(mode, "currency-order")) {
            const char *first = strstr(text, "\"currency-total\"");
            /* Retain the former assertion for review: comparing a missing search
             * result as a pointer is undefined. Check both matches first. */
#if 0
            CHECK(first && strstr(first, "\"GBP\"") < strstr(first, "\"USD\""));
#endif
            CHECK(first != NULL);
            const char *gbp = strstr(first, "\"GBP\""), *usd = strstr(first, "\"USD\"");
            CHECK(gbp != NULL && usd != NULL && gbp < usd);
        } else if (!strcmp(mode, "formula-account")) CHECK(strstr(text, "\"'=1+1\"") != NULL);
        else if (!strcmp(mode, "formula-reference")) CHECK(strstr(text, "\"'  @example\"") != NULL);
        else if (!strcmp(mode, "quoted-text")) CHECK(strstr(text, "\"a,\"\"b\"\"\"") != NULL);
        else if (!strcmp(mode, "utf8")) CHECK(strstr(text, "caf\xc3\xa9") != NULL);
        else if (!strcmp(mode, "maximum-revision")) CHECK(strstr(text, "\"18446744073709551615\"") != NULL);
        else if (!strcmp(mode, "approved")) CHECK(strstr(text, "\"batch\",\"4\",\"yes\",\"no\"") != NULL);
        else if (!strcmp(mode, "posted")) CHECK(strstr(text, "\"batch\",\"5\",\"yes\",\"yes\"") != NULL);
        else if (!strcmp(mode, "snapshot")) {
            size_t size = UmiCsvDocumentBytes(report) + 1U;
            char *copy = malloc(size); CHECK(copy); memcpy(copy, text, size);
            book->lines[0].debit = 99.0; memset(book->lines[1].referenceId, 'z', sizeof book->lines[1].referenceId);
            CHECK(memcmp(copy, UmiCsvDocumentData(report), size) == 0); free(copy);
        } else if (!strcmp(mode, "short-copy")) {
            char output[4] = "old"; size_t needed = 0U;
            CHECK(UmiCsvDocumentCopy(report, output, sizeof output, &needed) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(!strcmp(output, "old") && needed == UmiCsvDocumentBytes(report) + 1U);
        }
    }
    UmiCsvDocumentDestroy(report); UmiCsvDocumentDestroy(sentinel);
    free(before); free(book); return 0;
}
