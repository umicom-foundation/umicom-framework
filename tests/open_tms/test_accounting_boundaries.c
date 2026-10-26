/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_accounting_boundaries.c
 *
 * PURPOSE:
 *   Exercise finite per-currency accounting and complete-batch approval invalidation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/open_tms/accounting_book.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static UmiOpenTmsAccountingLine Line(const char *currency, int debit, double amount)
{
    UmiOpenTmsAccountingLine line = {0};
    strcpy(line.accountId, debit ? "cash" : "receivable");
    strcpy(line.referenceId, "synthetic-transaction");
    strcpy(line.currency, currency);
    if (debit) line.debit = amount; else line.credit = amount;
    return line;
}
static void Pair(UmiOpenTmsAccountingBook *book, const char *currency, double amount)
{
    UmiOpenTmsAccountingLine debit = Line(currency, 1, amount), credit = Line(currency, 0, amount);
    OK(umi_open_tms_accounting_book_add(book, &debit));
    OK(umi_open_tms_accounting_book_add(book, &credit));
}
int main(int argc, char **argv)
{
    const char *cases[] = {"normal", "multi", "cross-currency", "edit-approved", "post-unbalanced",
        "post-nan", "post-before-approve", "post-twice", "approve-twice", "approve-posted",
        "overflow", "approve-overflow", "maximum", "tiny-one-side", "tolerance-in", "tolerance-out",
        "count", "revision-zero", "approved-flag", "posted-flag", "posted-without-approval",
        "empty-approved", "limit", "empty", "outputs", "currency-query", "revision-add",
        "revision-approve", "revision-post", "add-nan", "add-infinity", "add-negative",
        "add-both", "add-zero", "add-empty-account", "add-empty-reference", "add-blank",
        "add-control", "add-account-bound", "add-reference-bound", "add-currency", "add-currency-width",
        "add-currency-bound", "add-posted", "negative-zero"};
    CHECK(argc == 2);
    int known = 0; for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) if (!strcmp(argv[1], cases[i])) known = 1;
    CHECK(known); const char *mode = argv[1];
    UmiOpenTmsAccountingBook *book = malloc(sizeof *book), *before = malloc(sizeof *before);
    CHECK(book && before); umi_open_tms_accounting_book_init(book);
    UmiOpenTmsAccountingLine line = Line("USD", 1, 100.0);
    if (!strncmp(mode, "add-", 4U) || !strcmp(mode, "negative-zero")) {
        if (!strcmp(mode, "add-nan")) line.debit = NAN;
        if (!strcmp(mode, "add-infinity")) line.credit = INFINITY;
        if (!strcmp(mode, "add-negative")) line.debit = -1.0;
        if (!strcmp(mode, "add-both")) line.credit = 100.0;
        if (!strcmp(mode, "add-zero")) line.debit = 0.0;
        if (!strcmp(mode, "add-empty-account")) line.accountId[0] = '\0';
        if (!strcmp(mode, "add-empty-reference")) line.referenceId[0] = '\0';
        if (!strcmp(mode, "add-blank")) strcpy(line.accountId, "   ");
        if (!strcmp(mode, "add-control")) strcpy(line.referenceId, "one\n");
        if (!strcmp(mode, "add-account-bound")) memset(line.accountId, 'a', sizeof line.accountId);
        if (!strcmp(mode, "add-reference-bound")) memset(line.referenceId, 'r', sizeof line.referenceId);
        if (!strcmp(mode, "add-currency")) strcpy(line.currency, "usd");
        if (!strcmp(mode, "add-currency-width")) strcpy(line.currency, "US");
        if (!strcmp(mode, "add-currency-bound")) memset(line.currency, 'U', sizeof line.currency);
        if (!strcmp(mode, "add-posted")) { Pair(book, "USD", 100.0); OK(umi_open_tms_accounting_book_approve(book)); OK(umi_open_tms_accounting_book_post(book)); }
        if (!strcmp(mode, "negative-zero")) line.credit = -0.0;
        *before = *book; UmiStatus result = umi_open_tms_accounting_book_add(book, &line);
        if (!strcmp(mode, "negative-zero")) CHECK(result == UMI_STATUS_OK);
        else CHECK(result != UMI_STATUS_OK && !memcmp(book, before, sizeof *book));
    } else if (!strcmp(mode, "empty")) {
        double d = -1.0, c = -2.0;
        OK(umi_open_tms_accounting_book_totals(book, &d, &c)); CHECK(d == 0.0 && c == 0.0);
        *before = *book; CHECK(umi_open_tms_accounting_book_approve(book) != UMI_STATUS_OK);
        CHECK(!memcmp(book, before, sizeof *book));
    } else {
        Pair(book, "USD", !strcmp(mode, "maximum") ? DBL_MAX : 100.0);
        if (!strcmp(mode, "multi")) Pair(book, "GBP", 250.0);
        if (!strcmp(mode, "cross-currency")) strcpy(book->lines[1].currency, "GBP");
        if (!strcmp(mode, "tiny-one-side")) { book->lines[0].debit = 1e-9; book->lines[1].credit = 1e-9; strcpy(book->lines[1].currency, "GBP"); }
        if (!strcmp(mode, "tolerance-in")) book->lines[1].credit += 0.0000005;
        if (!strcmp(mode, "tolerance-out")) book->lines[1].credit += 0.000002;
        if (!strcmp(mode, "overflow") || !strcmp(mode, "approve-overflow")) {
            umi_open_tms_accounting_book_init(book); Pair(book, "USD", DBL_MAX); Pair(book, "USD", DBL_MAX);
        }
        if (!strcmp(mode, "count")) book->count = UMI_OPEN_TMS_ACCOUNTING_LINE_CAPACITY + 1U;
        if (!strcmp(mode, "revision-zero")) book->revision = 0U;
        if (!strcmp(mode, "approved-flag")) book->approved = 2;
        if (!strcmp(mode, "posted-flag")) book->posted = -1;
        if (!strcmp(mode, "posted-without-approval")) book->posted = 1;
        if (!strcmp(mode, "empty-approved")) { book->count = 0U; book->approved = 1; }
        if (!strcmp(mode, "limit")) {
            while (book->count < UMI_OPEN_TMS_ACCOUNTING_LINE_CAPACITY) OK(umi_open_tms_accounting_book_add(book, &line));
            *before = *book; CHECK(umi_open_tms_accounting_book_add(book, &line) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(!memcmp(book, before, sizeof *book));
        } else if (!strncmp(mode, "revision-", 9U) && strcmp(mode, "revision-zero")) {
            if (!strcmp(mode, "revision-post")) OK(umi_open_tms_accounting_book_approve(book));
            book->revision = UINT64_MAX; *before = *book;
            UmiStatus result = !strcmp(mode, "revision-add") ? umi_open_tms_accounting_book_add(book, &line) :
                !strcmp(mode, "revision-approve") ? umi_open_tms_accounting_book_approve(book) : umi_open_tms_accounting_book_post(book);
            CHECK(result == UMI_STATUS_CAPACITY_EXCEEDED && !memcmp(book, before, sizeof *book));
        } else if (!strcmp(mode, "outputs") || !strcmp(mode, "currency-query") || !strcmp(mode, "overflow")) {
            double d = -1.0, c = -2.0; UmiStatus result;
            if (!strcmp(mode, "overflow")) result = umi_open_tms_accounting_book_totals(book, &d, &c);
            else if (!strcmp(mode, "currency-query")) result = umi_open_tms_accounting_book_currency_totals(book, "EUR", &d, &c);
            else {
                result = umi_open_tms_accounting_book_totals(book, &d, &d);
                CHECK(umi_open_tms_accounting_book_currency_totals(book, "USD", &c, &c) == UMI_STATUS_INVALID_ARGUMENT);
                CHECK(umi_open_tms_accounting_book_totals(NULL, &d, &c) == UMI_STATUS_INVALID_ARGUMENT);
                CHECK(umi_open_tms_accounting_book_currency_totals(book, NULL, &d, &c) == UMI_STATUS_INVALID_ARGUMENT);
                CHECK(umi_open_tms_accounting_book_totals(book, NULL, &c) == UMI_STATUS_INVALID_ARGUMENT);
            }
            CHECK(result != UMI_STATUS_OK && d == -1.0 && c == -2.0);
        } else if (!strcmp(mode, "edit-approved") || !strcmp(mode, "post-unbalanced") ||
            !strcmp(mode, "post-nan") || !strcmp(mode, "post-twice") || !strcmp(mode, "approve-twice") || !strcmp(mode, "approve-posted")) {
            OK(umi_open_tms_accounting_book_approve(book));
            uint64_t approved_revision = book->revision;
            if (!strcmp(mode, "edit-approved")) {
                OK(umi_open_tms_accounting_book_add(book, &line)); CHECK(!book->approved && book->revision == approved_revision + 1U);
            }
            if (!strcmp(mode, "post-unbalanced")) book->lines[1].credit = 101.0;
            if (!strcmp(mode, "post-nan")) book->lines[1].credit = NAN;
            if (!strcmp(mode, "post-twice") || !strcmp(mode, "approve-posted")) OK(umi_open_tms_accounting_book_post(book));
            *before = *book;
            UmiStatus result = !strcmp(mode, "approve-twice") || !strcmp(mode, "approve-posted") ?
                umi_open_tms_accounting_book_approve(book) : umi_open_tms_accounting_book_post(book);
            CHECK(result != UMI_STATUS_OK && !memcmp(book, before, sizeof *book));
        } else if (!strcmp(mode, "post-before-approve")) {
            *before = *book; CHECK(umi_open_tms_accounting_book_post(book) == UMI_STATUS_INVALID_STATE);
            CHECK(!memcmp(book, before, sizeof *book));
        } else {
            *before = *book; UmiStatus result = umi_open_tms_accounting_book_approve(book);
            int valid = !strcmp(mode, "normal") || !strcmp(mode, "multi") || !strcmp(mode, "maximum") || !strcmp(mode, "tolerance-in");
            if (valid) {
                CHECK(result == UMI_STATUS_OK && book->approved && book->revision == before->revision + 1U);
                OK(umi_open_tms_accounting_book_post(book)); CHECK(book->posted);
                double d = -1.0, c = -2.0;
                if (!strcmp(mode, "multi")) {
                    CHECK(umi_open_tms_accounting_book_totals(book, &d, &c) == UMI_STATUS_INVALID_STATE);
                    CHECK(d == -1.0 && c == -2.0);
                    OK(umi_open_tms_accounting_book_currency_totals(book, "GBP", &d, &c));
                    CHECK(d == 250.0 && c == 250.0);
                } else OK(umi_open_tms_accounting_book_totals(book, &d, &c));
            } else CHECK(result != UMI_STATUS_OK && !memcmp(book, before, sizeof *book));
        }
    }
    free(before); free(book); return 0;
}
