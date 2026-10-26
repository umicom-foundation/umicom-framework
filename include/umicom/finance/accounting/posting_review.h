/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/posting_review.h
 *
 * PURPOSE:
 *   Capture a journal and its local posting prerequisites before conditional in-memory publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_FINANCE_ACCOUNTING_POSTING_REVIEW_H
#define UMICOM_FINANCE_ACCOUNTING_POSTING_REVIEW_H
#include "umicom/finance/accounting/journal_book.h"
#include "umicom/finance/accounting/chart_of_accounts.h"
#include "umicom/finance/accounting/accounting_calendar.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiAccountingPostingBlocker {
    UMI_ACCOUNTING_POSTING_READY = 0,
    UMI_ACCOUNTING_POSTING_INVALID_ENTRY,
    UMI_ACCOUNTING_POSTING_ENTRY_STATE,
    UMI_ACCOUNTING_POSTING_AMOUNT_OVERFLOW,
    UMI_ACCOUNTING_POSTING_UNBALANCED,
    UMI_ACCOUNTING_POSTING_INVALID_CHART,
    UMI_ACCOUNTING_POSTING_INVALID_CALENDAR,
    UMI_ACCOUNTING_POSTING_INVALID_BOOK,
    UMI_ACCOUNTING_POSTING_MISSING_PERIOD,
    UMI_ACCOUNTING_POSTING_CLOSED_PERIOD,
    UMI_ACCOUNTING_POSTING_MISSING_ACCOUNT,
    UMI_ACCOUNTING_POSTING_BLOCKED_ACCOUNT,
    UMI_ACCOUNTING_POSTING_FULL_BOOK,
    UMI_ACCOUNTING_POSTING_DUPLICATE_JOURNAL
} UmiAccountingPostingBlocker;

typedef struct UmiAccountingPostingPreview {
    UmiAccountingPostingBlocker blocker;
    UmiStatus refusal;
    size_t line_index; /* SIZE_MAX unless a particular line is refused. */
    size_t line_count;
    int64_t debit_minor;
    int64_t credit_minor;
    UmiFinancialId period_id; /* Empty until a matching period is resolved. */
} UmiAccountingPostingPreview;

typedef struct UmiAccountingPostingReview UmiAccountingPostingReview;

/* Inspect without allocation or mutation. With non-NULL inputs/output, OK means
 * the preview was produced; preview.refusal describes a domain refusal. NULL
 * arguments return INVALID_ARGUMENT without changing out_preview. Output must
 * not overlap input. Inspection validates complete public containers first.
 *
 * A journal has no currency/scale metadata. The surrounding owner must establish
 * consistent minor units, account currencies, approval and durable authority.
 * READY means only these local structural prerequisites permit in-memory posting. */
UmiStatus umi_accounting_posting_inspect(const UmiAccountingJournalEntry *entry,
    const UmiAccountingChartOfAccounts *chart, const UmiAccountingAccountingCalendar *calendar,
    const UmiAccountingJournalBook *book, UmiAccountingPostingPreview *out_preview);

/* Capture an eligible journal and all three complete owners on the heap. Sets
 * *out_review to NULL on failure; input and output storage must be separate.
 * The caller owns the returned review. No external action or file I/O occurs. */
UmiStatus umi_accounting_posting_review_create(const UmiAccountingJournalEntry *entry,
    const UmiAccountingChartOfAccounts *chart, const UmiAccountingAccountingCalendar *calendar,
    const UmiAccountingJournalBook *book, UmiAccountingPostingReview **out_review);
void umi_accounting_posting_review_destroy(UmiAccountingPostingReview *review);

/* Borrow immutable captured data until destroy. NULL review returns NULL. */
const UmiAccountingPostingPreview *umi_accounting_posting_review_preview(
    const UmiAccountingPostingReview *review);
const UmiAccountingJournalEntry *umi_accounting_posting_review_entry(
    const UmiAccountingPostingReview *review);

/* Publish the captured entry only into the original book, once, and only while
 * candidate, chart, calendar and book retain their captured semantic fields.
 * The same live owner addresses are required. Padding, unused array slots and
 * bytes after text terminators are ignored; order and every active field count.
 * Any refusal leaves book and review unchanged. Success consumes the review.
 *
 * The owner serializes capture/apply and keeps inputs alive through apply.
 * Identity plus content is a local stale-review guard, not authentication or
 * protection from address reuse, malicious writers, races or persisted replay.
 * Use a durable authorized service for real financial posting. */
UmiStatus umi_accounting_posting_review_apply(UmiAccountingPostingReview *review,
    const UmiAccountingJournalEntry *entry, const UmiAccountingChartOfAccounts *chart,
    const UmiAccountingAccountingCalendar *calendar, UmiAccountingJournalBook *book);

#ifdef __cplusplus
}
#endif
#endif
