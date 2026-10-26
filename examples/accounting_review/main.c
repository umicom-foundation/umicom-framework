/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/accounting_review/main.c
 *
 * PURPOSE:
 *   Exercise complete-batch accounting review through the installed Framework SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/open_tms/accounting_review.h"
#include "umicom/open_tms/accounting_archive.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr, "Refused expectation at line %d: %s\n", __LINE__, #x); goto done; } } while (0)
static UmiStatus Pair(UmiOpenTmsAccountingBook *book, const char *currency, double amount)
{
    UmiOpenTmsAccountingLine line = {0};
    strcpy(line.accountId, "practice-cash"); strcpy(line.referenceId, "local-example");
    memcpy(line.currency, currency, 4U); line.debit = amount;
    UmiStatus status = umi_open_tms_accounting_book_add(book, &line);
    if (status != UMI_STATUS_OK) return status;
    strcpy(line.accountId, "practice-receivable"); line.debit = 0.0; line.credit = amount;
    return umi_open_tms_accounting_book_add(book, &line);
}

/* An imported batch is a separate value until its complete contents have been
 * inspected. The expected snapshot protects the local owner from a late edit. */
static int ArchiveExample(const char *mode)
{
    int result = 1;
    UmiOpenTmsAccountingBook *source = malloc(sizeof *source), *draft = malloc(sizeof *draft);
    UmiOpenTmsAccountingBook *owner = malloc(sizeof *owner), *expected = malloc(sizeof *expected);
    UmiOpenTmsAccountingBook *sentinel = malloc(sizeof *sentinel);
    unsigned char *bytes = NULL;
    REQUIRE(source && draft && owner && expected && sentinel);
    umi_open_tms_accounting_book_init(source);
    REQUIRE(Pair(source, "GBP", 125.0) == UMI_STATUS_OK);
    REQUIRE(umi_open_tms_accounting_book_approve(source) == UMI_STATUS_OK);
    size_t size = 0U;
    REQUIRE(umi_open_tms_accounting_book_archive_encode(source, NULL, 0U, &size) == UMI_STATUS_OK);
    REQUIRE(size <= umi_open_tms_accounting_book_archive_limit());
    bytes = malloc(size);
    REQUIRE(bytes != NULL);
    REQUIRE(umi_open_tms_accounting_book_archive_encode(source, bytes, size, &size) == UMI_STATUS_OK);
    REQUIRE(umi_open_tms_accounting_book_archive_decode_with_workspace(bytes, size, sentinel, draft) == UMI_STATUS_OK);
    REQUIRE(draft->count == 2U && draft->approved && !draft->posted);
    REQUIRE(umi_open_tms_accounting_book_check_balance(draft) == UMI_STATUS_OK);
    umi_open_tms_accounting_book_init(owner); *expected = *owner;
    if (!strcmp(mode, "archive-stale")) REQUIRE(Pair(owner, "USD", 7.0) == UMI_STATUS_OK);
    *sentinel = *owner;
    UmiStatus status = umi_open_tms_accounting_book_archive_decode_if_unchanged(bytes, size, expected, owner);
    if (!strcmp(mode, "archive-stale")) {
        REQUIRE(status == UMI_STATUS_INVALID_STATE);
        REQUIRE(!memcmp(owner, sentinel, sizeof *owner));
        puts("The changed local batch was preserved; import requires a fresh review.");
    } else {
        REQUIRE(status == UMI_STATUS_OK && owner->count == draft->count);
        REQUIRE(owner->approved && !owner->posted);
        REQUIRE(owner->lines[0].debit == 125.0 && !strcmp(owner->lines[0].currency, "GBP"));
        puts("Reviewed local batch restored. Imported flags provide no authenticated approval.");
    }
    result = 0;
done:
    free(bytes); free(sentinel); free(expected); free(owner); free(draft); free(source);
    return result;
}

int main(int argc, char **argv)
{
    /* Retain the original scenario admission for review; the replacement also
     * qualifies the public archive API through the same installed SDK. */
#if 0
    if (argc != 2 || (strcmp(argv[1], "apply") && strcmp(argv[1], "stale") &&
        strcmp(argv[1], "currencies"))) {
        fputs("Usage: accounting-review-consumer apply|stale|currencies\n", stderr);
        return 2;
    }
#endif
    if (argc != 2 || (strcmp(argv[1], "apply") && strcmp(argv[1], "stale") &&
        strcmp(argv[1], "currencies") && strcmp(argv[1], "archive-roundtrip") &&
        strcmp(argv[1], "archive-stale"))) {
        fputs("Usage: accounting-review-consumer apply|stale|currencies|archive-roundtrip|archive-stale\n", stderr);
        return 2;
    }
    if (!strncmp(argv[1], "archive-", 8U)) return ArchiveExample(argv[1]);
    int result = 1;
    UmiOpenTmsAccountingBook *book = malloc(sizeof *book), *before = malloc(sizeof *before);
    UmiOpenTmsAccountingReview *review = NULL;
    REQUIRE(book && before);
    umi_open_tms_accounting_book_init(book);
    REQUIRE(Pair(book, "GBP", 125.0) == UMI_STATUS_OK);
    if (!strcmp(argv[1], "currencies")) {
        REQUIRE(Pair(book, "USD", 75.0) == UMI_STATUS_OK);
        double debit = -1.0, credit = -1.0;
        REQUIRE(umi_open_tms_accounting_book_totals(book, &debit, &credit) == UMI_STATUS_INVALID_STATE);
        REQUIRE(debit == -1.0 && credit == -1.0);
        REQUIRE(umi_open_tms_accounting_book_currency_totals(book, "GBP", &debit, &credit) == UMI_STATUS_OK);
        REQUIRE(debit == 125.0 && credit == 125.0);
        REQUIRE(umi_open_tms_accounting_book_currency_totals(book, "USD", &debit, &credit) == UMI_STATUS_OK);
        REQUIRE(debit == 75.0 && credit == 75.0);
    }
    *before = *book;
    REQUIRE(umi_open_tms_accounting_review_create(book, UMI_OPEN_TMS_ACCOUNTING_APPROVE, &review) == UMI_STATUS_OK);
    REQUIRE(!memcmp(book, before, sizeof *book));
    UmiOpenTmsAccountingReviewSnapshot preview;
    REQUIRE(umi_open_tms_accounting_review_read(review, &preview) == UMI_STATUS_OK);
    REQUIRE(preview.source_revision == book->revision && preview.line_count == book->count);
    for (size_t i = 0U; i < preview.line_count; ++i) {
        UmiOpenTmsAccountingLine row;
        REQUIRE(umi_open_tms_accounting_review_line(review, i, &row) == UMI_STATUS_OK);
        printf("%s %s debit %.2f credit %.2f\n", row.accountId, row.currency, row.debit, row.credit);
    }
    if (!strcmp(argv[1], "stale")) {
        /* Even an unversioned balanced edit changes what the user reviewed. */
        book->lines[0].debit = 150.0; book->lines[1].credit = 150.0; *before = *book;
        REQUIRE(umi_open_tms_accounting_review_apply(book, review) == UMI_STATUS_INVALID_STATE);
        REQUIRE(!memcmp(book, before, sizeof *book));
        puts("Changed batch refused; create a fresh review before another decision.");
    } else {
        REQUIRE(umi_open_tms_accounting_review_apply(book, review) == UMI_STATUS_OK);
        REQUIRE(book->approved && !book->posted && book->revision == preview.next_revision);
        REQUIRE(umi_open_tms_accounting_review_apply(book, review) == UMI_STATUS_INVALID_STATE);
        umi_open_tms_accounting_review_destroy(review); review = NULL;
        REQUIRE(umi_open_tms_accounting_review_create(book, UMI_OPEN_TMS_ACCOUNTING_POST, &review) == UMI_STATUS_OK);
        REQUIRE(umi_open_tms_accounting_review_apply(book, review) == UMI_STATUS_OK);
        REQUIRE(book->posted);
        puts("Local preview flags updated. No ledger, bank or file was changed.");
    }
    result = 0;
done:
    umi_open_tms_accounting_review_destroy(review);
    free(before); free(book);
    return result;
}
