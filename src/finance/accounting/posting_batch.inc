/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/posting_batch.inc
 *
 * PURPOSE:
 *   Prepare a complete private journal book before conditional local publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Included by posting_review.c to share its validated semantic comparators.
 * Applications must not reproduce these rules or post a reviewed batch one
 * journal at a time: a late refusal would leave a partially changed book. */
struct UmiAccountingPostingBatchReview {
  const UmiAccountingJournalEntry *entries_owner;
  const UmiAccountingChartOfAccounts *chart_owner;
  const UmiAccountingAccountingCalendar *calendar_owner;
  const UmiAccountingJournalBook *book_owner;
  UmiAccountingPostingBatchPreview preview;
  UmiAccountingJournalEntry entries[UMI_ACCOUNTING_MAX_ITEMS];
  UmiAccountingPostingPreview entry_previews[UMI_ACCOUNTING_MAX_ITEMS];
  UmiAccountingChartOfAccounts chart;
  UmiAccountingAccountingCalendar calendar;
  UmiAccountingJournalBook before;
  UmiAccountingJournalBook prepared;
  bool consumed;
};

UmiStatus umi_accounting_posting_batch_create(const UmiAccountingJournalEntry *entries,
                                              size_t count,
                                              const UmiAccountingChartOfAccounts *chart,
                                              const UmiAccountingAccountingCalendar *calendar,
                                              const UmiAccountingJournalBook *book,
                                              UmiAccountingPostingBatchPreview *out_preview,
                                              UmiAccountingPostingBatchReview **out_review) {
  if (out_review == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out_review = NULL;
  if (entries == NULL || chart == NULL || calendar == NULL || book == NULL || count == 0U ||
      count > UMI_ACCOUNTING_MAX_ITEMS)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiAccountingPostingBatchReview *review = calloc(1U, sizeof *review);
  if (review == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  review->entries_owner = entries;
  review->chart_owner = chart;
  review->calendar_owner = calendar;
  review->book_owner = book;
  review->preview.entry_count = count;
  review->preview.failed_index = SIZE_MAX;
  review->chart = *chart;
  review->calendar = *calendar;
  review->before = *book;
  review->prepared = *book;
  memcpy(review->entries, entries, count * sizeof *entries);
  for (size_t i = 0U; i < count; ++i) {
    UmiAccountingPostingPreview *item = &review->entry_previews[i];
    UmiStatus status = umi_accounting_posting_inspect(&review->entries[i], &review->chart,
                                                      &review->calendar, &review->prepared, item);
    /* Inspection has checked every active container before any comparison
     * or posting. Its public refusal identifies the exact candidate row. */
    if (status != UMI_STATUS_OK) {
      free(review);
      return status;
    }
    if (item->refusal != UMI_STATUS_OK) {
      review->preview.failed_index = i;
      review->preview.refusal = item->refusal;
      review->preview.detail = *item;
      if (out_preview != NULL)
        *out_preview = review->preview;
      status = item->refusal;
      free(review);
      return status;
    }
    status = umi_accounting_journal_book_post(&review->prepared, &review->entries[i]);
    if (status != UMI_STATUS_OK) {
      /* A private post should agree with the preceding inspection.
       * Preserve live state even if a future posting rule adds a refusal. */
      free(review);
      return status;
    }
    ++review->preview.ready_count;
    review->preview.line_count += item->line_count;
  }
  if (out_preview != NULL)
    *out_preview = review->preview;
  *out_review = review;
  return UMI_STATUS_OK;
}

UmiStatus umi_accounting_posting_batch_inspect(const UmiAccountingJournalEntry *entries,
                                               size_t count,
                                               const UmiAccountingChartOfAccounts *chart,
                                               const UmiAccountingAccountingCalendar *calendar,
                                               const UmiAccountingJournalBook *book,
                                               UmiAccountingPostingBatchPreview *out_preview) {
  if (out_preview == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiAccountingPostingBatchPreview preview = {0};
  UmiAccountingPostingBatchReview *review = NULL;
  UmiStatus status =
      umi_accounting_posting_batch_create(entries, count, chart, calendar, book, &preview, &review);
  if (status == UMI_STATUS_OK || preview.entry_count != 0U) {
    *out_preview = preview;
    status = UMI_STATUS_OK;
  }
  umi_accounting_posting_batch_destroy(review);
  return status;
}

void umi_accounting_posting_batch_destroy(UmiAccountingPostingBatchReview *review) { free(review); }

const UmiAccountingPostingBatchPreview *
umi_accounting_posting_batch_preview(const UmiAccountingPostingBatchReview *review) {
  return review != NULL ? &review->preview : NULL;
}

const UmiAccountingJournalEntry *
umi_accounting_posting_batch_entry(const UmiAccountingPostingBatchReview *review, size_t index) {
  return review != NULL && index < review->preview.entry_count ? &review->entries[index] : NULL;
}

const UmiAccountingPostingPreview *
umi_accounting_posting_batch_entry_preview(const UmiAccountingPostingBatchReview *review,
                                           size_t index) {
  return review != NULL && index < review->preview.entry_count ? &review->entry_previews[index]
                                                               : NULL;
}

UmiStatus umi_accounting_posting_batch_apply(UmiAccountingPostingBatchReview *review,
                                             const UmiAccountingJournalEntry *entries, size_t count,
                                             const UmiAccountingChartOfAccounts *chart,
                                             const UmiAccountingAccountingCalendar *calendar,
                                             UmiAccountingJournalBook *book) {
  if (review == NULL || entries == NULL || chart == NULL || calendar == NULL || book == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (review->consumed || entries != review->entries_owner || chart != review->chart_owner ||
      calendar != review->calendar_owner || book != review->book_owner ||
      count != review->preview.entry_count)
    return UMI_STATUS_INVALID_STATE;
  if (umi_accounting_chart_of_accounts_validate(chart) != UMI_STATUS_OK ||
      umi_accounting_accounting_calendar_validate(calendar) != UMI_STATUS_OK ||
      umi_accounting_journal_book_validate(book) != UMI_STATUS_OK ||
      !ChartEqual(chart, &review->chart) || !CalendarEqual(calendar, &review->calendar) ||
      !BookEqual(book, &review->before))
    return UMI_STATUS_INVALID_STATE;
  for (size_t i = 0U; i < count; ++i) {
    if (!umi_accounting_journal_entry_valid(&entries[i]) ||
        !EntryEqual(&entries[i], &review->entries[i]))
      return UMI_STATUS_INVALID_STATE;
  }
  /* Every entry was already posted into a private candidate. After all live
   * preconditions match, this is the sole write to the serialized live book. */
  *book = review->prepared;
  review->consumed = true;
  return UMI_STATUS_OK;
}
