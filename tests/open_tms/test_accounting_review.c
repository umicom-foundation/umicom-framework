/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_accounting_review.c
 *
 * PURPOSE:
 *   Check complete accounting preview ownership, stale-field refusal and one-time local application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/open_tms/accounting_review.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static void Populate(UmiOpenTmsAccountingBook *book)
{
    umi_open_tms_accounting_book_init(book);
    UmiOpenTmsAccountingLine line = {0};
    strcpy(line.accountId, "cash"); strcpy(line.currency, "USD"); strcpy(line.referenceId, "example");
    line.debit = 125.0; OK(umi_open_tms_accounting_book_add(book, &line));
    strcpy(line.accountId, "receivable"); line.debit = 0.0; line.credit = 125.0;
    OK(umi_open_tms_accounting_book_add(book, &line));
}
int main(int argc, char **argv)
{
    const char *cases[] = {"approve", "post", "equivalent", "source-lifetime", "snapshot", "line",
        "inactive-storage", "text-tails", "changed-account", "changed-reference", "changed-currency",
        "changed-debit", "changed-credit", "balanced-edit", "changed-count", "changed-order",
        "changed-revision", "changed-approved", "changed-posted", "changed-nan", "changed-text-bound",
        "changed-count-bound", "create-null", "create-action", "create-empty", "create-unbalanced",
        "create-unapproved-post", "create-repeat-approval", "create-revision-limit", "outputs"};
    CHECK(argc == 2); int known = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) if (!strcmp(argv[1], cases[i])) known = 1;
    CHECK(known); const char *mode = argv[1];
    UmiOpenTmsAccountingBook *book = malloc(sizeof *book), *before = malloc(sizeof *before);
    CHECK(book && before); Populate(book);
    UmiOpenTmsAccountingAction action = !strcmp(mode, "post") ? UMI_OPEN_TMS_ACCOUNTING_POST : UMI_OPEN_TMS_ACCOUNTING_APPROVE;
    if (!strcmp(mode, "post")) OK(umi_open_tms_accounting_book_approve(book));
    UmiOpenTmsAccountingReview *review = NULL;
    if (!strncmp(mode, "create-", 7U)) {
        if (!strcmp(mode, "create-action")) action = (UmiOpenTmsAccountingAction)99;
        if (!strcmp(mode, "create-empty")) umi_open_tms_accounting_book_init(book);
        if (!strcmp(mode, "create-unbalanced")) book->lines[1].credit = 126.0;
        if (!strcmp(mode, "create-unapproved-post")) action = UMI_OPEN_TMS_ACCOUNTING_POST;
        if (!strcmp(mode, "create-repeat-approval")) OK(umi_open_tms_accounting_book_approve(book));
        if (!strcmp(mode, "create-revision-limit")) book->revision = UINT64_MAX;
        *before = *book;
        UmiStatus status = umi_open_tms_accounting_review_create(!strcmp(mode, "create-null") ? NULL : book, action, &review);
        CHECK(status != UMI_STATUS_OK && review == NULL && !memcmp(book, before, sizeof *book));
    } else {
        *before = *book; OK(umi_open_tms_accounting_review_create(book, action, &review));
        CHECK(!memcmp(book, before, sizeof *book));
        UmiOpenTmsAccountingReviewSnapshot snapshot;
        OK(umi_open_tms_accounting_review_read(review, &snapshot));
        CHECK(snapshot.line_count == 2U && snapshot.source_revision == book->revision &&
            snapshot.next_revision == book->revision + 1U && snapshot.action == action &&
            snapshot.approved_before == book->approved && snapshot.posted_before == 0 &&
            snapshot.approved_after == 1 && snapshot.posted_after == (action == UMI_OPEN_TMS_ACCOUNTING_POST));
        if (!strcmp(mode, "snapshot")) {
            memset(&snapshot, 0xFF, sizeof snapshot);
            OK(umi_open_tms_accounting_review_read(review, &snapshot)); CHECK(snapshot.line_count == 2U);
        }
        if (!strcmp(mode, "line")) {
            UmiOpenTmsAccountingLine row; OK(umi_open_tms_accounting_review_line(review, 0U, &row));
            CHECK(row.debit == 125.0 && row.credit == 0.0 && !strcmp(row.accountId, "cash"));
            row.debit = 999.0; OK(umi_open_tms_accounting_review_line(review, 0U, &row)); CHECK(row.debit == 125.0);
        }
        if (!strcmp(mode, "outputs")) {
            /* Keep preview expectations separate from failure-output sentinels. */
#if 0
            UmiOpenTmsAccountingReviewSnapshot guard; memset(&guard, 0xA5, sizeof guard); snapshot = guard;
#endif
            UmiOpenTmsAccountingReviewSnapshot guard; memset(&guard, 0xA5, sizeof guard);
            const UmiOpenTmsAccountingReviewSnapshot guard_copy = guard;
            CHECK(umi_open_tms_accounting_review_read(NULL, &guard) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(!memcmp(&guard_copy, &guard, sizeof guard));
            UmiOpenTmsAccountingLine row, copy; memset(&row, 0xA5, sizeof row); copy = row;
            CHECK(umi_open_tms_accounting_review_line(review, 2U, &row) == UMI_STATUS_NOT_FOUND);
            CHECK(!memcmp(&row, &copy, sizeof row));
            CHECK(umi_open_tms_accounting_review_line(review, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(umi_open_tms_accounting_review_create(book, action, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(umi_open_tms_accounting_review_apply(book, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(umi_open_tms_accounting_review_apply(NULL, review) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(!memcmp(book, before, sizeof *book));
        }
        if (!strcmp(mode, "source-lifetime") || !strcmp(mode, "equivalent")) {
            UmiOpenTmsAccountingBook *replacement = malloc(sizeof *replacement); CHECK(replacement);
            *replacement = *book; free(book); book = replacement;
        }
        if (!strcmp(mode, "inactive-storage")) memset(&book->lines[book->count], 0xA5, sizeof book->lines[0]);
        if (!strcmp(mode, "text-tails")) memset(book->lines[0].accountId + 5, 0xA5, sizeof book->lines[0].accountId - 5U);
        if (!strcmp(mode, "changed-account")) strcpy(book->lines[0].accountId, "other");
        if (!strcmp(mode, "changed-reference")) strcpy(book->lines[1].referenceId, "other");
        if (!strcmp(mode, "changed-currency")) strcpy(book->lines[0].currency, "GBP");
        if (!strcmp(mode, "changed-debit")) book->lines[0].debit = 124.0;
        if (!strcmp(mode, "changed-credit")) book->lines[1].credit = 124.0;
        if (!strcmp(mode, "balanced-edit")) { book->lines[0].debit = 250.0; book->lines[1].credit = 250.0; }
        if (!strcmp(mode, "changed-count")) { book->lines[2] = book->lines[0]; ++book->count; }
        if (!strcmp(mode, "changed-order")) { UmiOpenTmsAccountingLine row = book->lines[0]; book->lines[0] = book->lines[1]; book->lines[1] = row; }
        if (!strcmp(mode, "changed-revision")) ++book->revision;
        if (!strcmp(mode, "changed-approved")) book->approved = 1;
        if (!strcmp(mode, "changed-posted")) { book->approved = 1; book->posted = 1; }
        if (!strcmp(mode, "changed-nan")) book->lines[0].debit = NAN;
        if (!strcmp(mode, "changed-text-bound")) memset(book->lines[0].accountId, 'x', sizeof book->lines[0].accountId);
        if (!strcmp(mode, "changed-count-bound")) book->count = UMI_OPEN_TMS_ACCOUNTING_LINE_CAPACITY + 1U;
        *before = *book;
        UmiStatus result = umi_open_tms_accounting_review_apply(book, review);
        if (!strncmp(mode, "changed-", 8U) || !strcmp(mode, "balanced-edit")) CHECK(result != UMI_STATUS_OK && !memcmp(book, before, sizeof *book));
        else {
            CHECK(result == UMI_STATUS_OK);
            before->approved = snapshot.approved_after; before->posted = snapshot.posted_after;
            before->revision = snapshot.next_revision;
            CHECK(!memcmp(book, before, sizeof *book));
            CHECK(umi_open_tms_accounting_review_apply(book, review) == UMI_STATUS_INVALID_STATE);
            CHECK(!memcmp(book, before, sizeof *book));
        }
    }
    umi_open_tms_accounting_review_destroy(review);
    umi_open_tms_accounting_review_destroy(NULL);
    free(before); free(book); return 0;
}
