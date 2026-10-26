/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_feed/test_comparison.c
 *
 * PURPOSE:
 *   Qualify exact imported-identity matching and independent comparison ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "fixture.h"
#include "csv_fixture.h"
#include "umicom/bank_feed/comparison.h"
#include "umicom/bank_feed/csv_import.h"
#undef CHECK
#define CHECK(value) do { if (!(value)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); exit(1); } } while (0)

static UmiBankFeed *Csv(const char *rows) {
    size_t size = strlen(BANK_CSV_HEADER) + strlen(rows) + 1U;
    char *text = malloc(size);
    CHECK(text != NULL);
    (void)snprintf(text, size, "%s%s", BANK_CSV_HEADER, rows);
    UmiBankFeed *feed = NULL;
    CHECK(UmiBankFeedCsvCreate(text, strlen(text), &feed, NULL) == UMI_STATUS_OK);
    free(text);
    return feed;
}
static UmiBankFeedComparison *Compare(const char *left, const char *right) {
    UmiBankFeed *a = Csv(left), *b = Csv(right);
    UmiBankFeedComparison *out = NULL;
    CHECK(UmiBankFeedComparisonCreate(a, b, &out) == UMI_STATUS_OK && out != NULL);
    /* Every ordinary case reads after the source owners are released. */
    UmiBankFeedDestroy(a);
    UmiBankFeedDestroy(b);
    return out;
}
static UmiBankFeedComparisonRow Row(const UmiBankFeedComparison *owner, size_t index) {
    UmiBankFeedComparisonRow row;
    CHECK(UmiBankFeedComparisonAt(owner, index, &row) == UMI_STATUS_OK);
    return row;
}
static void OneChange(const char *find, const char *replacement, uint32_t flags) {
    char *after = BankReplace(BANK_CSV_CREDIT, find, replacement);
    CHECK(after != NULL);
    UmiBankFeedComparison *owner = Compare(BANK_CSV_CREDIT, after);
    free(after);
    UmiBankFeedComparisonSummary summary;
    CHECK(UmiBankFeedComparisonRead(owner, &summary) == UMI_STATUS_OK);
    CHECK(summary.rows == 1U && summary.changed == 1U && summary.same == 0U);
    UmiBankFeedComparisonRow row = Row(owner, 0U);
    CHECK(row.kind == UMI_BANK_FEED_CHANGED && row.changes == flags);
    CHECK(row.beforeIndex == 0U && row.afterIndex == 0U);
    UmiBankFeedComparisonDestroy(owner);
}
int main(int argc, char **argv) {
    CHECK(argc == 2);
    const char *mode = argv[1];
    if (!strcmp(mode, "amount")) OneChange("12.50", "12.51", UMI_BANK_FEED_CHANGE_AMOUNT | UMI_BANK_FEED_CHANGE_AMOUNT_TEXT);
    else if (!strcmp(mode, "amount-spelling")) OneChange("12.50", "12.50000", UMI_BANK_FEED_CHANGE_AMOUNT_TEXT);
    else if (!strcmp(mode, "currency")) OneChange("GBP", "EUR", UMI_BANK_FEED_CHANGE_CURRENCY);
    else if (!strcmp(mode, "direction")) OneChange("12.50", "-12.50", UMI_BANK_FEED_CHANGE_DIRECTION);
    else if (!strcmp(mode, "status")) OneChange("BOOK", "PDNG", UMI_BANK_FEED_CHANGE_STATUS | UMI_BANK_FEED_CHANGE_MUTABILITY);
    else if (!strcmp(mode, "mutability")) OneChange("Immutable", "Mutable", UMI_BANK_FEED_CHANGE_MUTABILITY);
    else if (!strcmp(mode, "booking-time")) OneChange("2026-10-12", "2026-10-13", UMI_BANK_FEED_CHANGE_BOOKING_TIME);
    else if (!strcmp(mode, "reference")) OneChange(",ref,", ",reference-two,", UMI_BANK_FEED_CHANGE_REFERENCE);
    else if (!strcmp(mode, "information")) OneChange("Example credit", "Revised credit", UMI_BANK_FEED_CHANGE_INFORMATION);
    else if (!strcmp(mode, "same") || !strcmp(mode, "copied-row")) {
        UmiBankFeedComparison *owner = Compare(BANK_CSV_CREDIT, BANK_CSV_CREDIT);
        UmiBankFeedComparisonSummary summary;
        CHECK(UmiBankFeedComparisonRead(owner, &summary) == UMI_STATUS_OK);
        CHECK(summary.rows == 1U && summary.same == 1U && summary.changed == 0U);
        UmiBankFeedComparisonRow row = Row(owner, 0U);
        CHECK(row.kind == UMI_BANK_FEED_SAME && row.changes == 0U);
        strcpy(row.before.reference, "changed caller copy");
        CHECK(!strcmp(Row(owner, 0U).before.reference, "ref"));
        UmiBankFeedComparisonDestroy(owner);
        CHECK(!strcmp(row.after.transaction, "credit-one"));
    } else if (!strcmp(mode, "reordered")) {
        UmiBankFeedComparison *owner = Compare(BANK_CSV_CREDIT BANK_CSV_DEBIT,
                                               BANK_CSV_DEBIT BANK_CSV_CREDIT);
        UmiBankFeedComparisonRow first = Row(owner, 0U), second = Row(owner, 1U);
        CHECK(first.kind == UMI_BANK_FEED_SAME && second.kind == UMI_BANK_FEED_SAME);
        CHECK(first.beforeIndex == 0U && first.afterIndex == 1U);
        CHECK(second.beforeIndex == 1U && second.afterIndex == 0U);
        UmiBankFeedComparisonDestroy(owner);
    } else if (!strcmp(mode, "one-sided") || !strcmp(mode, "before-empty") || !strcmp(mode, "after-empty")) {
        const char *a = !strcmp(mode, "before-empty") ? "" : BANK_CSV_CREDIT;
        const char *b = !strcmp(mode, "after-empty") ? "" : BANK_CSV_DEBIT;
        UmiBankFeedComparison *owner = Compare(a, b);
        UmiBankFeedComparisonSummary summary;
        CHECK(UmiBankFeedComparisonRead(owner, &summary) == UMI_STATUS_OK);
        CHECK(summary.same == 0U && summary.changed == 0U);
        CHECK(summary.beforeOnly == (a[0] == '\0' ? 0U : 1U));
        CHECK(summary.afterOnly == (b[0] == '\0' ? 0U : 1U));
        for (size_t i = 0U; i < summary.rows; ++i) {
            UmiBankFeedComparisonRow row = Row(owner, i);
            if (row.kind == UMI_BANK_FEED_BEFORE_ONLY) {
                CHECK(row.beforeIndex == 0U && row.afterIndex == SIZE_MAX);
                CHECK(row.after.account[0] == '\0' && row.after.transaction[0] == '\0' && row.after.reference[0] == '\0' && row.after.information[0] == '\0' && row.after.bookingTime[0] == '\0' && row.after.currency[0] == '\0' && row.after.amountText[0] == '\0' && row.after.amount.coefficient == 0 && row.after.amount.scale == 0U && !row.after.credit && !row.after.booked && !row.after.mutableRecord);
            } else {
                CHECK(row.kind == UMI_BANK_FEED_AFTER_ONLY);
                CHECK(row.beforeIndex == SIZE_MAX && row.afterIndex == 0U);
                CHECK(row.before.account[0] == '\0' && row.before.transaction[0] == '\0' && row.before.reference[0] == '\0' && row.before.information[0] == '\0' && row.before.bookingTime[0] == '\0' && row.before.currency[0] == '\0' && row.before.amountText[0] == '\0' && row.before.amount.coefficient == 0 && row.before.amount.scale == 0U && !row.before.credit && !row.before.booked && !row.before.mutableRecord);
            }
        }
        UmiBankFeedComparisonDestroy(owner);
    } else if (!strcmp(mode, "missing-id")) {
        char *rows = BankReplace(BANK_CSV_CREDIT, "credit-one", "");
        CHECK(rows != NULL);
        UmiBankFeedComparison *owner = Compare(rows, rows);
        free(rows);
        UmiBankFeedComparisonSummary summary;
        CHECK(UmiBankFeedComparisonRead(owner, &summary) == UMI_STATUS_OK);
        CHECK(summary.rows == 2U && summary.same == 0U);
        CHECK(summary.beforeUnidentified == 1U && summary.afterUnidentified == 1U);
        CHECK(Row(owner, 0U).kind == UMI_BANK_FEED_BEFORE_UNIDENTIFIED);
        CHECK(Row(owner, 1U).kind == UMI_BANK_FEED_AFTER_UNIDENTIFIED);
        UmiBankFeedComparisonDestroy(owner);
    } else if (!strcmp(mode, "account") || !strcmp(mode, "case-sensitive")) {
        char *rows = !strcmp(mode, "account")
            ? BankReplace(BANK_CSV_CREDIT, "practice-account", "another-account")
            : BankReplace(BANK_CSV_CREDIT, "credit-one", "Credit-one");
        CHECK(rows != NULL);
        UmiBankFeedComparison *owner = Compare(BANK_CSV_CREDIT, rows);
        free(rows);
        CHECK(Row(owner, 0U).kind == UMI_BANK_FEED_BEFORE_ONLY);
        CHECK(Row(owner, 1U).kind == UMI_BANK_FEED_AFTER_ONLY);
        UmiBankFeedComparisonDestroy(owner);
    } else if (!strcmp(mode, "same-input") || !strcmp(mode, "empty")) {
        UmiBankFeed *feed = Csv(!strcmp(mode, "empty") ? "" : BANK_CSV_CREDIT);
        UmiBankFeedComparison *owner = NULL;
        CHECK(UmiBankFeedComparisonCreate(feed, feed, &owner) == UMI_STATUS_OK);
        UmiBankFeedDestroy(feed);
        UmiBankFeedComparisonSummary summary;
        CHECK(UmiBankFeedComparisonRead(owner, &summary) == UMI_STATUS_OK);
        CHECK(summary.rows == (!strcmp(mode, "empty") ? 0U : 1U));
        CHECK(summary.same == summary.rows);
        UmiBankFeedComparisonDestroy(owner);
    } else if (!strcmp(mode, "pagination")) {
        char *plain = BankFixture(BANK_TEST_ROW);
        CHECK(plain != NULL);
        char *paged = BankReplace(plain, "{\"Data\":", "{\"Links\":{\"Prev\":\"p\",\"Next\":\"n\"},\"Data\":");
        CHECK(plain != NULL && paged != NULL);
        UmiBankFeed *a = NULL, *b = NULL;
        CHECK(UmiBankFeedCreate(plain, strlen(plain), &a) == UMI_STATUS_OK);
        CHECK(UmiBankFeedCreate(paged, strlen(paged), &b) == UMI_STATUS_OK);
        UmiBankFeedComparison *owner = NULL;
        CHECK(UmiBankFeedComparisonCreate(a, b, &owner) == UMI_STATUS_OK);
        UmiBankFeedComparisonSummary summary;
        CHECK(UmiBankFeedComparisonRead(owner, &summary) == UMI_STATUS_OK);
        CHECK(summary.same == 1U && !summary.before.hasNextPage && !summary.before.hasPreviousPage);
        CHECK(summary.after.hasNextPage && summary.after.hasPreviousPage);
        UmiBankFeedComparisonDestroy(owner); UmiBankFeedDestroy(a); UmiBankFeedDestroy(b);
        free(plain); free(paged);
    } else if (!strcmp(mode, "capacity")) {
        size_t capacity = UMI_BANK_FEED_MAX_ROWS * 128U + 1U;
        char *a = calloc(capacity, 1U), *b = calloc(capacity, 1U);
        CHECK(a != NULL && b != NULL);
        size_t usedA = 0U, usedB = 0U;
        for (size_t i = 0U; i < UMI_BANK_FEED_MAX_ROWS; ++i) {
            int wa = snprintf(a + usedA, capacity - usedA, "a,b%zu,,,2026-10-12,GBP,1,BOOK,Immutable\n", i);
            int wb = snprintf(b + usedB, capacity - usedB, "a,a%zu,,,2026-10-12,GBP,1,BOOK,Immutable\n", i);
            CHECK(wa > 0 && wb > 0 && (size_t)wa < capacity - usedA && (size_t)wb < capacity - usedB);
            usedA += (size_t)wa; usedB += (size_t)wb;
        }
        UmiBankFeedComparison *owner = Compare(a, b);
        free(a); free(b);
        UmiBankFeedComparisonSummary summary;
        CHECK(UmiBankFeedComparisonRead(owner, &summary) == UMI_STATUS_OK);
        CHECK(summary.rows == 2U * UMI_BANK_FEED_MAX_ROWS);
        CHECK(summary.beforeOnly == UMI_BANK_FEED_MAX_ROWS && summary.afterOnly == UMI_BANK_FEED_MAX_ROWS);
        CHECK(Row(owner, UMI_BANK_FEED_MAX_ROWS - 1U).beforeIndex == UMI_BANK_FEED_MAX_ROWS - 1U);
        CHECK(Row(owner, summary.rows - 1U).afterIndex == UMI_BANK_FEED_MAX_ROWS - 1U);
        UmiBankFeedComparisonDestroy(owner);
    } else if (!strcmp(mode, "invalid-api")) {
        UmiBankFeed *feed = Csv(BANK_CSV_CREDIT);
        UmiBankFeedComparison *owner = (UmiBankFeedComparison *)1;
        CHECK(UmiBankFeedComparisonCreate(NULL, feed, &owner) == UMI_STATUS_INVALID_ARGUMENT && owner == NULL);
        owner = (UmiBankFeedComparison *)1;
        CHECK(UmiBankFeedComparisonCreate(feed, NULL, &owner) == UMI_STATUS_INVALID_ARGUMENT && owner == NULL);
        CHECK(UmiBankFeedComparisonCreate(feed, feed, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBankFeedComparisonCreate(feed, feed, &owner) == UMI_STATUS_OK);
        UmiBankFeedComparisonRow row, saved;
        memset(&row, 0x5a, sizeof row); memcpy(&saved, &row, sizeof row);
        CHECK(UmiBankFeedComparisonAt(owner, SIZE_MAX, &row) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&row, &saved, sizeof row) == 0);
        CHECK(UmiBankFeedComparisonAt(NULL, 0U, &row) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&row, &saved, sizeof row) == 0);
        CHECK(UmiBankFeedComparisonAt(owner, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        UmiBankFeedComparisonSummary summary, kept;
        memset(&summary, 0x5a, sizeof summary); memcpy(&kept, &summary, sizeof summary);
        CHECK(UmiBankFeedComparisonRead(NULL, &summary) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&summary, &kept, sizeof summary) == 0);
        CHECK(UmiBankFeedComparisonRead(owner, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBankFeedComparisonKindName((UmiBankFeedComparisonKind)999) == NULL);
        UmiBankFeedComparisonDestroy(NULL); UmiBankFeedComparisonDestroy(owner); UmiBankFeedDestroy(feed);
    } else CHECK(false);
    return 0;
}
