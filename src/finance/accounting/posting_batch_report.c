/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/posting_batch_report.c
 *
 * PURPOSE:
 *   Serialize immutable journal review rows using shared exact-decimal and CSV services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/posting_batch_report.h"
#include "umicom/finance/currency.h"
#include "umicom/finance/decimal.h"
#include <inttypes.h>
#include <stdio.h>

/* CSV owns each appended row. Decimal/date buffers stay local and never become
 * borrowed state in the report, so destroying a review cannot invalidate it. */
static UmiStatus JournalReportRow(UmiCsvDocument *document, const char *kind, size_t journal_index,
                                  const UmiAccountingJournalEntry *entry,
                                  const UmiAccountingPostingPreview *preview,
                                  const UmiAccountingJournalLine *line, size_t line_index,
                                  const UmiCurrency *currency, uint8_t scale) {
  char date[16], debit[UMI_DECIMAL_TEXT_MAX], credit[UMI_DECIMAL_TEXT_MAX];
  int64_t debit_minor = line != NULL ? line->debit_minor : preview->debit_minor;
  int64_t credit_minor = line != NULL ? line->credit_minor : preview->credit_minor;
  int length =
      snprintf(date, sizeof date, "%04" PRId32 "-%02u-%02u", entry->accounting_date.year,
               (unsigned)entry->accounting_date.month, (unsigned)entry->accounting_date.day);
  if (length < 0 || (size_t)length >= sizeof date)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  UmiStatus status = UmiDecimalFormat((UmiDecimal){debit_minor, scale}, debit, sizeof debit);
  if (status != UMI_STATUS_OK)
    return status;
  status = UmiDecimalFormat((UmiDecimal){credit_minor, scale}, credit, sizeof credit);
  if (status != UMI_STATUS_OK)
    return status;
  UmiCsvCell cells[] = {UmiCsvText(kind),
                        UmiCsvText("captured-eligibility"),
                        UmiCsvUnsigned((uint64_t)journal_index + UINT64_C(1)),
                        UmiCsvText(entry->id.value),
                        UmiCsvText(date),
                        UmiCsvText(preview->period_id.value),
                        line != NULL ? UmiCsvUnsigned((uint64_t)line_index + UINT64_C(1))
                                     : UmiCsvText(""),
                        UmiCsvText(line != NULL ? line->id.value : ""),
                        UmiCsvText(line != NULL ? line->account_id.value : ""),
                        UmiCsvText(currency->code),
                        UmiCsvUnsigned(scale),
                        UmiCsvSigned(debit_minor),
                        UmiCsvSigned(credit_minor),
                        UmiCsvText(debit),
                        UmiCsvText(credit)};
  return UmiCsvDocumentAppendRow(document, cells, sizeof cells / sizeof cells[0]);
}

UmiStatus umi_accounting_posting_batch_report(const UmiAccountingPostingBatchReview *review,
                                              const UmiCurrency *currency, uint8_t scale,
                                              size_t max_bytes, UmiCsvDocument **out_report) {
  if (out_report == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out_report = NULL;
  if (review == NULL || !umi_currency_valid(currency) || scale > 9U)
    return UMI_STATUS_INVALID_ARGUMENT;
  const UmiAccountingPostingBatchPreview *batch = umi_accounting_posting_batch_preview(review);
  if (batch == NULL || batch->refusal != UMI_STATUS_OK || batch->entry_count == 0U ||
      batch->entry_count > UMI_ACCOUNTING_MAX_ITEMS || batch->ready_count != batch->entry_count)
    return UMI_STATUS_INVALID_STATE;

  UmiCsvDocument *document = NULL;
  UmiStatus status = UmiCsvDocumentCreate(max_bytes, &document);
  if (status != UMI_STATUS_OK)
    return status;
  UmiCsvCell columns[] = {
      UmiCsvText("record"),       UmiCsvText("review_kind"),     UmiCsvText("journal_index"),
      UmiCsvText("journal_id"),   UmiCsvText("accounting_date"), UmiCsvText("period_id"),
      UmiCsvText("line_index"),   UmiCsvText("line_id"),         UmiCsvText("account_id"),
      UmiCsvText("currency"),     UmiCsvText("scale"),           UmiCsvText("debit_minor"),
      UmiCsvText("credit_minor"), UmiCsvText("debit"),           UmiCsvText("credit")};
  status = UmiCsvDocumentAppendRow(document, columns, sizeof columns / sizeof columns[0]);
  for (size_t i = 0U; status == UMI_STATUS_OK && i < batch->entry_count; ++i) {
    const UmiAccountingJournalEntry *entry = umi_accounting_posting_batch_entry(review, i);
    const UmiAccountingPostingPreview *preview =
        umi_accounting_posting_batch_entry_preview(review, i);
    if (entry == NULL || preview == NULL || entry->line_count > UMI_ACCOUNTING_MAX_LINES) {
      status = UMI_STATUS_INVALID_STATE;
      break;
    }
    for (size_t j = 0U; status == UMI_STATUS_OK && j < entry->line_count; ++j)
      status = JournalReportRow(document, "line", i, entry, preview, &entry->lines[j], j, currency,
                                scale);
    if (status == UMI_STATUS_OK)
      status =
          JournalReportRow(document, "journal-total", i, entry, preview, NULL, 0U, currency, scale);
  }
  /* The report is assembled off to the side. A later invalid text field or
   * byte limit therefore cannot publish a plausible but truncated journal. */
  if (status != UMI_STATUS_OK) {
    UmiCsvDocumentDestroy(document);
    return status;
  }
  *out_report = document;
  return UMI_STATUS_OK;
}
