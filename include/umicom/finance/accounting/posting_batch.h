/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/posting_batch.h
 *
 * PURPOSE:
 *   Review and conditionally publish a complete set of exact-money journals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_POSTING_BATCH_H
#define UMICOM_FINANCE_ACCOUNTING_POSTING_BATCH_H
#include "umicom/finance/accounting/posting_review.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAccountingPostingBatchPreview {
  size_t entry_count;
  size_t ready_count;
  size_t line_count;
  size_t failed_index; /* SIZE_MAX when every entry is eligible. */
  UmiStatus refusal;
  UmiAccountingPostingPreview detail; /* Meaningful only for the refused entry. */
} UmiAccountingPostingBatchPreview;
typedef struct UmiAccountingPostingBatchReview UmiAccountingPostingBatchReview;

/* Inspect entries in order against a private evolving book, so duplicates
 * within this batch and cumulative capacity are checked too. Allocates bounded
 * temporary storage. Returns OK when a preview is produced; preview.refusal
 * describes domain refusal. Invalid arguments/allocation failure leave output
 * untouched. count must be 1..UMI_ACCOUNTING_MAX_ITEMS. */
UmiStatus umi_accounting_posting_batch_inspect(const UmiAccountingJournalEntry *entries,
                                               size_t count,
                                               const UmiAccountingChartOfAccounts *chart,
                                               const UmiAccountingAccountingCalendar *calendar,
                                               const UmiAccountingJournalBook *book,
                                               UmiAccountingPostingBatchPreview *out_preview);

/* Capture complete eligible inputs and a prepared book on the heap.
 * *out_review becomes NULL on failure. An optional preview receives the
 * inspected prefix and exact first refusal; invalid arguments or allocation
 * failure leave that preview untouched. Domain refusal returns its status.
 * Output storage must not overlap any input. No live book or journal is changed. */
UmiStatus umi_accounting_posting_batch_create(const UmiAccountingJournalEntry *entries,
                                              size_t count,
                                              const UmiAccountingChartOfAccounts *chart,
                                              const UmiAccountingAccountingCalendar *calendar,
                                              const UmiAccountingJournalBook *book,
                                              UmiAccountingPostingBatchPreview *out_preview,
                                              UmiAccountingPostingBatchReview **out_review);
void umi_accounting_posting_batch_destroy(UmiAccountingPostingBatchReview *review);
const UmiAccountingPostingBatchPreview *
umi_accounting_posting_batch_preview(const UmiAccountingPostingBatchReview *review);
/* Borrow immutable captured entry/eligibility until destroy; invalid index or
 * NULL review returns NULL. No aggregate money total is invented across entries. */
const UmiAccountingJournalEntry *
umi_accounting_posting_batch_entry(const UmiAccountingPostingBatchReview *review, size_t index);
const UmiAccountingPostingPreview *
umi_accounting_posting_batch_entry_preview(const UmiAccountingPostingBatchReview *review,
                                           size_t index);

/* Publish once into the same live book only if all active semantic fields in
 * entries, chart, calendar and book still match capture, at the same addresses
 * and count. Refusal leaves book/review unchanged. Success appends all captured
 * entries as POSTED and consumes the review. No allocation occurs during apply.
 *
 * The caller serializes capture/apply and keeps inputs alive. The final struct
 * assignment provides all-or-nothing behavior for this serialized in-memory
 * owner; it is NOT a database transaction or a concurrent atomic write.
 * Minor-unit currency/scale agreement, authorization, durability, reconciliation
 * and protection from malicious writers/address reuse belong to the surrounding
 * financial service. READY grants none of those external authorities. */
UmiStatus umi_accounting_posting_batch_apply(UmiAccountingPostingBatchReview *review,
                                             const UmiAccountingJournalEntry *entries, size_t count,
                                             const UmiAccountingChartOfAccounts *chart,
                                             const UmiAccountingAccountingCalendar *calendar,
                                             UmiAccountingJournalBook *book);
#ifdef __cplusplus
}
#endif
#endif
