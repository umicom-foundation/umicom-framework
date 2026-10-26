/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/posting_review.c
 *
 * PURPOSE:
 *   Share checked local posting inspection and stale-review protection across financial applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/finance/accounting/posting_review.h"
#include "umicom/finance/accounting/posting_batch.h"
#include <stdlib.h>
#include <string.h>

/* Retain complete values, not pointers into editable rows. Identity fields are
 * compared only; dereferencing uses the caller's current live owner arguments. */
struct UmiAccountingPostingReview {
    const UmiAccountingJournalEntry *entry_owner;
    const UmiAccountingChartOfAccounts *chart_owner;
    const UmiAccountingAccountingCalendar *calendar_owner;
    const UmiAccountingJournalBook *book_owner;
    UmiAccountingPostingPreview preview;
    UmiAccountingJournalEntry entry;
    UmiAccountingChartOfAccounts chart;
    UmiAccountingAccountingCalendar calendar;
    UmiAccountingJournalBook book;
    bool consumed;
};

static UmiStatus Refuse(UmiAccountingPostingPreview *next,
    UmiAccountingPostingPreview *out_preview, UmiAccountingPostingBlocker blocker, UmiStatus status)
{
    next->blocker = blocker;
    next->refusal = status;
    *out_preview = *next;
    return UMI_STATUS_OK;
}

UmiStatus umi_accounting_posting_inspect(const UmiAccountingJournalEntry *entry,
    const UmiAccountingChartOfAccounts *chart, const UmiAccountingAccountingCalendar *calendar,
    const UmiAccountingJournalBook *book, UmiAccountingPostingPreview *out_preview)
{
    if (entry == NULL || chart == NULL || calendar == NULL || book == NULL || out_preview == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiAccountingPostingPreview next = {0};
    next.line_index = SIZE_MAX;
    if (!umi_accounting_journal_entry_valid(entry)) {
        return Refuse(&next, out_preview, UMI_ACCOUNTING_POSTING_INVALID_ENTRY, UMI_STATUS_INVALID_ARGUMENT);
    }
    next.line_count = entry->line_count;
    if (entry->status != UMI_ACCOUNTING_JOURNAL_DRAFT && entry->status != UMI_ACCOUNTING_JOURNAL_VALIDATED) {
        return Refuse(&next, out_preview, UMI_ACCOUNTING_POSTING_ENTRY_STATE, UMI_STATUS_INVALID_STATE);
    }
    UmiStatus status = umi_accounting_journal_entry_totals(entry, &next.debit_minor, &next.credit_minor);
    if (status != UMI_STATUS_OK) {
        return Refuse(&next, out_preview, UMI_ACCOUNTING_POSTING_AMOUNT_OVERFLOW, status);
    }
    if (entry->line_count < 2U || next.debit_minor <= 0 || next.debit_minor != next.credit_minor) {
        return Refuse(&next, out_preview, UMI_ACCOUNTING_POSTING_UNBALANCED, UMI_STATUS_INVALID_STATE);
    }
    status = umi_accounting_chart_of_accounts_validate(chart);
    if (status != UMI_STATUS_OK) {
        return Refuse(&next, out_preview, UMI_ACCOUNTING_POSTING_INVALID_CHART, status);
    }
    status = umi_accounting_accounting_calendar_validate(calendar);
    if (status != UMI_STATUS_OK) {
        return Refuse(&next, out_preview, UMI_ACCOUNTING_POSTING_INVALID_CALENDAR, status);
    }
    status = umi_accounting_journal_book_validate(book);
    if (status != UMI_STATUS_OK) {
        return Refuse(&next, out_preview, UMI_ACCOUNTING_POSTING_INVALID_BOOK, status);
    }
    const UmiAccountingAccountingPeriod *period =
        umi_accounting_accounting_calendar_find_date(calendar, entry->accounting_date);
    if (period == NULL) {
        return Refuse(&next, out_preview, UMI_ACCOUNTING_POSTING_MISSING_PERIOD, UMI_STATUS_NOT_FOUND);
    }
    next.period_id = period->id;
    if (!umi_accounting_accounting_period_open(period)) {
        return Refuse(&next, out_preview, UMI_ACCOUNTING_POSTING_CLOSED_PERIOD, UMI_STATUS_PERMISSION_DENIED);
    }
    for (size_t i = 0U; i < entry->line_count; ++i) {
        const UmiAccountingLedgerAccount *account =
            umi_accounting_chart_of_accounts_find(chart, entry->lines[i].account_id.value);
        if (account == NULL) {
            next.line_index = i;
            return Refuse(&next, out_preview, UMI_ACCOUNTING_POSTING_MISSING_ACCOUNT, UMI_STATUS_NOT_FOUND);
        }
        if (!umi_accounting_ledger_account_postable(account)) {
            next.line_index = i;
            return Refuse(&next, out_preview, UMI_ACCOUNTING_POSTING_BLOCKED_ACCOUNT, UMI_STATUS_PERMISSION_DENIED);
        }
    }
    if (book->count >= UMI_ACCOUNTING_MAX_ITEMS) {
        return Refuse(&next, out_preview, UMI_ACCOUNTING_POSTING_FULL_BOOK, UMI_STATUS_CAPACITY_EXCEEDED);
    }
    if (umi_accounting_journal_book_find(book, entry->id.value) != NULL) {
        return Refuse(&next, out_preview, UMI_ACCOUNTING_POSTING_DUPLICATE_JOURNAL, UMI_STATUS_ALREADY_EXISTS);
    }
    next.blocker = UMI_ACCOUNTING_POSTING_READY;
    next.refusal = UMI_STATUS_OK;
    *out_preview = next;
    return UMI_STATUS_OK;
}

UmiStatus umi_accounting_posting_review_create(const UmiAccountingJournalEntry *entry,
    const UmiAccountingChartOfAccounts *chart, const UmiAccountingAccountingCalendar *calendar,
    const UmiAccountingJournalBook *book, UmiAccountingPostingReview **out_review)
{
    if (out_review == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_review = NULL;
    UmiAccountingPostingPreview preview;
    UmiStatus status = umi_accounting_posting_inspect(entry, chart, calendar, book, &preview);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    if (preview.refusal != UMI_STATUS_OK) {
        return preview.refusal;
    }
    UmiAccountingPostingReview *review = calloc(1U, sizeof *review);
    if (review == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    review->entry_owner = entry;
    review->chart_owner = chart;
    review->calendar_owner = calendar;
    review->book_owner = book;
    review->preview = preview;
    review->entry = *entry;
    review->chart = *chart;
    review->calendar = *calendar;
    review->book = *book;
    *out_review = review;
    return UMI_STATUS_OK;
}

void umi_accounting_posting_review_destroy(UmiAccountingPostingReview *review)
{
    free(review);
}

const UmiAccountingPostingPreview *umi_accounting_posting_review_preview(
    const UmiAccountingPostingReview *review)
{
    return review == NULL ? NULL : &review->preview;
}

const UmiAccountingJournalEntry *umi_accounting_posting_review_entry(
    const UmiAccountingPostingReview *review)
{
    return review == NULL ? NULL : &review->entry;
}

/* Field equality follows the public values, never struct padding. Validation
 * precedes these reads. Extend these comparisons whenever active fields change. */
static bool EntryEqual(const UmiAccountingJournalEntry *left, const UmiAccountingJournalEntry *right)
{
    if (strcmp(left->id.value, right->id.value) != 0 ||
        umi_financial_date_compare(left->accounting_date, right->accounting_date) != 0 ||
        left->status != right->status || left->line_count != right->line_count) {
        return false;
    }
    for (size_t i = 0U; i < left->line_count; ++i) {
        const UmiAccountingJournalLine *a = &left->lines[i], *b = &right->lines[i];
        if (strcmp(a->id.value, b->id.value) != 0 || strcmp(a->account_id.value, b->account_id.value) != 0 ||
            a->debit_minor != b->debit_minor || a->credit_minor != b->credit_minor) {
            return false;
        }
    }
    return true;
}

static bool ChartEqual(const UmiAccountingChartOfAccounts *left, const UmiAccountingChartOfAccounts *right)
{
    if (left->count != right->count) {
        return false;
    }
    for (size_t i = 0U; i < left->count; ++i) {
        const UmiAccountingLedgerAccount *a = &left->items[i], *b = &right->items[i];
        if (strcmp(a->id.value, b->id.value) != 0 || strcmp(a->name, b->name) != 0 ||
            a->account_class != b->account_class || a->normal_side != b->normal_side ||
            a->posting_allowed != b->posting_allowed) {
            return false;
        }
    }
    return true;
}

static bool CalendarEqual(const UmiAccountingAccountingCalendar *left,
    const UmiAccountingAccountingCalendar *right)
{
    if (left->count != right->count) {
        return false;
    }
    for (size_t i = 0U; i < left->count; ++i) {
        const UmiAccountingAccountingPeriod *a = &left->items[i], *b = &right->items[i];
        if (strcmp(a->id.value, b->id.value) != 0 ||
            umi_financial_date_compare(a->start_date, b->start_date) != 0 ||
            umi_financial_date_compare(a->end_date, b->end_date) != 0 || a->status != b->status) {
            return false;
        }
    }
    return true;
}

static bool BookEqual(const UmiAccountingJournalBook *left, const UmiAccountingJournalBook *right)
{
    if (left->count != right->count) {
        return false;
    }
    for (size_t i = 0U; i < left->count; ++i) {
        if (!EntryEqual(&left->items[i], &right->items[i])) {
            return false;
        }
    }
    return true;
}

UmiStatus umi_accounting_posting_review_apply(UmiAccountingPostingReview *review,
    const UmiAccountingJournalEntry *entry, const UmiAccountingChartOfAccounts *chart,
    const UmiAccountingAccountingCalendar *calendar, UmiAccountingJournalBook *book)
{
    if (review == NULL || entry == NULL || chart == NULL || calendar == NULL || book == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (review->consumed || review->entry_owner != entry || review->chart_owner != chart ||
        review->calendar_owner != calendar || review->book_owner != book) {
        return UMI_STATUS_INVALID_STATE;
    }
    UmiAccountingPostingPreview preview;
    UmiStatus status = umi_accounting_posting_inspect(entry, chart, calendar, book, &preview);
    if (status != UMI_STATUS_OK || preview.refusal != UMI_STATUS_OK ||
        !EntryEqual(entry, &review->entry) || !ChartEqual(chart, &review->chart) ||
        !CalendarEqual(calendar, &review->calendar) || !BookEqual(book, &review->book)) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_accounting_journal_book_post(book, &review->entry);
    if (status == UMI_STATUS_OK) {
        review->consumed = true;
    }
    return status;
}

/* Batch review reuses the same private semantic ownership rules. */
#include "posting_batch.inc"
