/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_posting_review.c
 *
 * PURPOSE:
 *   Verify posting blockers, complete snapshot comparison and one-use local journal publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/finance/accounting/posting_review.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)

typedef struct Fixture {
    UmiAccountingJournalEntry entry;
    UmiAccountingChartOfAccounts chart;
    UmiAccountingAccountingCalendar calendar;
    UmiAccountingJournalBook book;
} Fixture;

static void Setup(Fixture *value)
{
    memset(value, 0, sizeof *value);
    UmiAccountingLedgerAccount account;
    CHECK(umi_accounting_ledger_account_init(&account, "cash", "Cash", UMI_ACCOUNTING_ASSET,
        UMI_ACCOUNTING_NORMAL_DEBIT, true) == UMI_STATUS_OK);
    CHECK(umi_accounting_chart_of_accounts_add(&value->chart, &account) == UMI_STATUS_OK);
    CHECK(umi_accounting_ledger_account_init(&account, "income", "Income", UMI_ACCOUNTING_REVENUE,
        UMI_ACCOUNTING_NORMAL_CREDIT, true) == UMI_STATUS_OK);
    CHECK(umi_accounting_chart_of_accounts_add(&value->chart, &account) == UMI_STATUS_OK);
    UmiAccountingAccountingPeriod period;
    CHECK(umi_accounting_accounting_period_init(&period, "october",
        (UmiFinancialDate){2026, 10U, 1U}, (UmiFinancialDate){2026, 10U, 31U},
        UMI_ACCOUNTING_PERIOD_OPEN) == UMI_STATUS_OK);
    CHECK(umi_accounting_accounting_calendar_add(&value->calendar, &period) == UMI_STATUS_OK);
    CHECK(umi_accounting_accounting_period_init(&period, "november",
        (UmiFinancialDate){2026, 11U, 1U}, (UmiFinancialDate){2026, 11U, 30U},
        UMI_ACCOUNTING_PERIOD_OPEN) == UMI_STATUS_OK);
    CHECK(umi_accounting_accounting_calendar_add(&value->calendar, &period) == UMI_STATUS_OK);
    CHECK(umi_accounting_journal_entry_init(&value->entry, "prior", (UmiFinancialDate){2026, 10U, 26U}) == UMI_STATUS_OK);
    UmiAccountingJournalLine line;
    CHECK(umi_accounting_journal_line_init(&line, "debit", "cash", 1234, 0) == UMI_STATUS_OK);
    CHECK(umi_accounting_journal_entry_add_line(&value->entry, &line) == UMI_STATUS_OK);
    CHECK(umi_accounting_journal_line_init(&line, "credit", "income", 0, 1234) == UMI_STATUS_OK);
    CHECK(umi_accounting_journal_entry_add_line(&value->entry, &line) == UMI_STATUS_OK);
    CHECK(umi_accounting_journal_book_post(&value->book, &value->entry) == UMI_STATUS_OK);
    CHECK(umi_accounting_id_assign(&value->entry.id, "journal") == UMI_STATUS_OK);
}

static int Inspect(const char *mode, Fixture *value, Fixture *saved)
{
    UmiAccountingPostingBlocker blocker = UMI_ACCOUNTING_POSTING_READY;
    size_t line = SIZE_MAX;
    bool invalid_argument = false;
    if (!strcmp(mode, "inspect-ready")) {}
    else if (!strcmp(mode, "inspect-invalid-entry")) {
        value->entry.line_count = SIZE_MAX; blocker = UMI_ACCOUNTING_POSTING_INVALID_ENTRY;
    } else if (!strcmp(mode, "inspect-posted") || !strcmp(mode, "inspect-reversed")) {
        value->entry.status = !strcmp(mode, "inspect-posted") ? UMI_ACCOUNTING_JOURNAL_POSTED : UMI_ACCOUNTING_JOURNAL_REVERSED;
        blocker = UMI_ACCOUNTING_POSTING_ENTRY_STATE;
    } else if (!strcmp(mode, "inspect-overflow")) {
        value->entry.lines[0].debit_minor = INT64_MAX;
        value->entry.lines[2] = value->entry.lines[0]; value->entry.line_count = 3U;
        blocker = UMI_ACCOUNTING_POSTING_AMOUNT_OVERFLOW;
    } else if (!strcmp(mode, "inspect-unbalanced") || !strcmp(mode, "inspect-empty")) {
        if (!strcmp(mode, "inspect-empty")) value->entry.line_count = 0U;
        else value->entry.lines[1].credit_minor = 1;
        blocker = UMI_ACCOUNTING_POSTING_UNBALANCED;
    } else if (!strcmp(mode, "inspect-chart")) {
        value->chart.count = SIZE_MAX; blocker = UMI_ACCOUNTING_POSTING_INVALID_CHART;
    } else if (!strcmp(mode, "inspect-calendar")) {
        value->calendar.count = SIZE_MAX; blocker = UMI_ACCOUNTING_POSTING_INVALID_CALENDAR;
    } else if (!strcmp(mode, "inspect-book")) {
        value->book.count = SIZE_MAX; blocker = UMI_ACCOUNTING_POSTING_INVALID_BOOK;
    } else if (!strcmp(mode, "inspect-missing-period")) {
        value->entry.accounting_date.month = 12U; blocker = UMI_ACCOUNTING_POSTING_MISSING_PERIOD;
    } else if (!strcmp(mode, "inspect-closed") || !strcmp(mode, "inspect-soft-closed")) {
        value->calendar.items[0].status = !strcmp(mode, "inspect-closed") ?
            UMI_ACCOUNTING_PERIOD_CLOSED : UMI_ACCOUNTING_PERIOD_SOFT_CLOSED;
        blocker = UMI_ACCOUNTING_POSTING_CLOSED_PERIOD;
    } else if (!strcmp(mode, "inspect-missing-account")) {
        CHECK(umi_accounting_id_assign(&value->entry.lines[1].account_id, "unknown") == UMI_STATUS_OK);
        blocker = UMI_ACCOUNTING_POSTING_MISSING_ACCOUNT; line = 1U;
    } else if (!strcmp(mode, "inspect-blocked-account")) {
        value->chart.items[1].posting_allowed = false;
        blocker = UMI_ACCOUNTING_POSTING_BLOCKED_ACCOUNT; line = 1U;
    } else if (!strcmp(mode, "inspect-duplicate")) {
        CHECK(umi_accounting_id_assign(&value->entry.id, "prior") == UMI_STATUS_OK);
        blocker = UMI_ACCOUNTING_POSTING_DUPLICATE_JOURNAL;
    } else if (!strcmp(mode, "inspect-full")) {
        for (size_t i = 1U; i < UMI_ACCOUNTING_MAX_ITEMS; ++i) {
            char id[32];
            int n = snprintf(id, sizeof id, "stored-%zu", i);
            CHECK(n > 0 && (size_t)n < sizeof id);
            value->book.items[i] = value->book.items[0];
            CHECK(umi_accounting_id_assign(&value->book.items[i].id, id) == UMI_STATUS_OK);
        }
        value->book.count = UMI_ACCOUNTING_MAX_ITEMS;
        blocker = UMI_ACCOUNTING_POSTING_FULL_BOOK;
    } else if (!strcmp(mode, "inspect-null-entry") || !strcmp(mode, "inspect-null-chart") ||
               !strcmp(mode, "inspect-null-calendar") || !strcmp(mode, "inspect-null-book") ||
               !strcmp(mode, "inspect-null-output")) invalid_argument = true;
    else return 2;
    memcpy(saved, value, sizeof *saved);
    UmiAccountingPostingPreview preview, previous;
    memset(&preview, 0xA5, sizeof preview); memcpy(&previous, &preview, sizeof previous);
    const UmiAccountingJournalEntry *entry = !strcmp(mode, "inspect-null-entry") ? NULL : &value->entry;
    const UmiAccountingChartOfAccounts *chart = !strcmp(mode, "inspect-null-chart") ? NULL : &value->chart;
    const UmiAccountingAccountingCalendar *calendar = !strcmp(mode, "inspect-null-calendar") ? NULL : &value->calendar;
    const UmiAccountingJournalBook *book = !strcmp(mode, "inspect-null-book") ? NULL : &value->book;
    UmiStatus status = umi_accounting_posting_inspect(entry, chart, calendar, book,
        !strcmp(mode, "inspect-null-output") ? NULL : &preview);
    CHECK(memcmp(value, saved, sizeof *value) == 0);
    if (invalid_argument) {
        CHECK(status == UMI_STATUS_INVALID_ARGUMENT && memcmp(&preview, &previous, sizeof preview) == 0);
    } else {
        CHECK(status == UMI_STATUS_OK && preview.blocker == blocker && preview.line_index == line);
        CHECK((preview.refusal == UMI_STATUS_OK) == (blocker == UMI_ACCOUNTING_POSTING_READY));
        UmiAccountingPostingReview *review = NULL;
        status = umi_accounting_posting_review_create(entry, chart, calendar, book, &review);
        if (blocker == UMI_ACCOUNTING_POSTING_READY) {
            CHECK(status == UMI_STATUS_OK && review != NULL);
            CHECK(umi_accounting_posting_review_preview(review)->debit_minor == 1234);
        } else {
            CHECK(status == preview.refusal && review == NULL);
        }
        umi_accounting_posting_review_destroy(review);
        CHECK(memcmp(value, saved, sizeof *value) == 0);
    }
    return 0;
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    Fixture *value = malloc(sizeof *value), *saved = malloc(sizeof *saved), *captured = malloc(sizeof *captured);
    CHECK(value && saved && captured);
    Setup(value);
    if (!strncmp(mode, "inspect-", 8U)) {
        int result = Inspect(mode, value, saved);
        free(captured); free(saved); free(value);
        return result;
    }
    if (!strcmp(mode, "null-accessors")) {
        CHECK(umi_accounting_posting_review_entry(NULL) == NULL);
        CHECK(umi_accounting_posting_review_preview(NULL) == NULL);
        umi_accounting_posting_review_destroy(NULL);
        CHECK(umi_accounting_posting_review_create(&value->entry, &value->chart, &value->calendar,
            &value->book, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        free(captured); free(saved); free(value);
        return 0;
    }
    UmiAccountingPostingReview *review = NULL;
    CHECK(umi_accounting_posting_review_create(&value->entry, &value->chart, &value->calendar,
        &value->book, &review) == UMI_STATUS_OK);
    CHECK(umi_accounting_posting_review_entry(review)->lines[0].debit_minor == 1234);
    memcpy(captured, value, sizeof *captured);
    bool success = false;
    if (!strcmp(mode, "apply")) success = true;
    else if (!strcmp(mode, "tails")) {
        size_t used = strlen(value->entry.id.value) + 1U;
        memset(value->entry.id.value + used, 0xA5, sizeof value->entry.id.value - used);
        memset(&value->entry.lines[value->entry.line_count], 0xA5,
            (UMI_ACCOUNTING_MAX_LINES - value->entry.line_count) * sizeof value->entry.lines[0]);
        success = true;
    } else if (!strcmp(mode, "entry-amount")) {
        value->entry.lines[0].debit_minor++; value->entry.lines[1].credit_minor++;
    } else if (!strcmp(mode, "entry-date")) value->entry.accounting_date.day++;
    else if (!strcmp(mode, "entry-state")) value->entry.status = UMI_ACCOUNTING_JOURNAL_VALIDATED;
    else if (!strcmp(mode, "entry-id")) CHECK(umi_accounting_id_assign(&value->entry.id, "different") == UMI_STATUS_OK);
    else if (!strcmp(mode, "line-id")) CHECK(umi_accounting_id_assign(&value->entry.lines[0].id, "different") == UMI_STATUS_OK);
    else if (!strcmp(mode, "line-order")) {
        UmiAccountingJournalLine line = value->entry.lines[0];
        value->entry.lines[0] = value->entry.lines[1]; value->entry.lines[1] = line;
    } else if (!strcmp(mode, "chart-name")) strcpy(value->chart.items[0].name, "Other");
    else if (!strcmp(mode, "chart-class")) value->chart.items[0].account_class = UMI_ACCOUNTING_EQUITY;
    else if (!strcmp(mode, "chart-side")) value->chart.items[0].normal_side = UMI_ACCOUNTING_NORMAL_CREDIT;
    else if (!strcmp(mode, "chart-blocked")) value->chart.items[0].posting_allowed = false;
    else if (!strcmp(mode, "chart-invalid")) value->chart.count = SIZE_MAX;
    else if (!strcmp(mode, "period-id")) CHECK(umi_accounting_id_assign(&value->calendar.items[0].id, "different") == UMI_STATUS_OK);
    else if (!strcmp(mode, "period-range")) value->calendar.items[0].start_date.day++;
    else if (!strcmp(mode, "period-closed")) value->calendar.items[0].status = UMI_ACCOUNTING_PERIOD_CLOSED;
    else if (!strcmp(mode, "period-order")) {
        UmiAccountingAccountingPeriod period = value->calendar.items[0];
        value->calendar.items[0] = value->calendar.items[1]; value->calendar.items[1] = period;
    } else if (!strcmp(mode, "period-invalid")) value->calendar.count = SIZE_MAX;
    else if (!strcmp(mode, "book-amount")) {
        value->book.items[0].lines[0].debit_minor++; value->book.items[0].lines[1].credit_minor++;
    } else if (!strcmp(mode, "book-id")) CHECK(umi_accounting_id_assign(&value->book.items[0].id, "different") == UMI_STATUS_OK);
    else if (!strcmp(mode, "book-invalid")) value->book.items[0].line_count = SIZE_MAX;
    else if (!strcmp(mode, "book-append")) {
        CHECK(umi_accounting_journal_book_post(&value->book, &value->entry) == UMI_STATUS_OK);
    } else if (strcmp(mode, "different-entry") && strcmp(mode, "different-chart") &&
               strcmp(mode, "different-calendar") && strcmp(mode, "different-book") &&
               strcmp(mode, "null-review") && strcmp(mode, "null-entry") &&
               strcmp(mode, "null-chart") && strcmp(mode, "null-calendar") && strcmp(mode, "null-book")) return 2;
    memcpy(saved, value, sizeof *saved);
    UmiStatus status = umi_accounting_posting_review_apply(!strcmp(mode, "null-review") ? NULL : review,
        !strcmp(mode, "null-entry") ? NULL : !strcmp(mode, "different-entry") ? &captured->entry : &value->entry,
        !strcmp(mode, "null-chart") ? NULL : !strcmp(mode, "different-chart") ? &captured->chart : &value->chart,
        !strcmp(mode, "null-calendar") ? NULL : !strcmp(mode, "different-calendar") ? &captured->calendar : &value->calendar,
        !strcmp(mode, "null-book") ? NULL : !strcmp(mode, "different-book") ? &captured->book : &value->book);
    if (success) {
        CHECK(status == UMI_STATUS_OK && value->book.count == saved->book.count + 1U);
        CHECK(value->entry.status == UMI_ACCOUNTING_JOURNAL_DRAFT);
        CHECK(value->book.items[1].status == UMI_ACCOUNTING_JOURNAL_POSTED);
        CHECK(!strcmp(value->book.items[1].id.value, "journal"));
        CHECK(umi_accounting_journal_book_validate(&value->book) == UMI_STATUS_OK);
        memcpy(saved, value, sizeof *saved);
        CHECK(umi_accounting_posting_review_apply(review, &value->entry, &value->chart,
            &value->calendar, &value->book) == UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(value, saved, sizeof *value) == 0);
    } else {
        CHECK(status != UMI_STATUS_OK && memcmp(value, saved, sizeof *value) == 0);
        /* Refusal must leave the review reusable after the owner explicitly
         * restores the captured semantic state at the same live addresses. */
        CHECK(captured->book.count == 1U && !strcmp(captured->book.items[0].id.value, "prior"));
        memcpy(value, captured, sizeof *value);
        CHECK(umi_accounting_posting_review_apply(review, &value->entry, &value->chart,
            &value->calendar, &value->book) == UMI_STATUS_OK);
    }
    umi_accounting_posting_review_destroy(review);
    free(captured); free(saved); free(value);
    return 0;
}
