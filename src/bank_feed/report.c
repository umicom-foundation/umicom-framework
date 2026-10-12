/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_feed/report.c
 * PURPOSE: Export original bank observations with explicit page scope and exact amount text.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "feed_private.h"
UmiStatus UmiBankFeedExportCsv(const UmiBankFeed *feed, UmiCsvDocument **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = NULL;
  if (feed == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiCsvDocument *csv = NULL;
  UmiStatus status = UmiCsvDocumentCreate(UMI_CSV_MAX_BYTES, &csv);
  const char *headers[] = {"record_kind",  "account_id", "transaction_id", "reference",
                           "booking_time", "currency",   "amount",         "direction",
                           "status",       "mutable",    "information",    "previous_page",
                           "next_page",    "scope"};
  UmiCsvCell cells[14];
  for (size_t i = 0; i < 14U; ++i)
    cells[i] = UmiCsvText(headers[i]);
  if (status == UMI_STATUS_OK)
    status = UmiCsvDocumentAppendRow(csv, cells, 14U);
  /* Every row retains page limitations so spreadsheet filtering cannot turn a
   * partial import into an apparently complete account statement. */
  for (size_t i = 0; status == UMI_STATUS_OK && i <= feed->summary.rows; ++i) {
    for (size_t j = 0; j < 14U; ++j)
      cells[j] = UmiCsvText("");
    cells[0] = UmiCsvText(i == 0U ? "metadata" : "transaction");
    cells[11] = UmiCsvText(feed->summary.hasPreviousPage ? "true" : "false");
    cells[12] = UmiCsvText(feed->summary.hasNextPage ? "true" : "false");
    cells[13] = UmiCsvText("Imported page; completeness and authenticity unverified");
    if (i != 0U) {
      const UmiBankFeedRow *row = &feed->rows[i - 1U];
      cells[1] = UmiCsvText(row->account);
      cells[2] = UmiCsvText(row->transaction);
      cells[3] = UmiCsvText(row->reference);
      cells[4] = UmiCsvText(row->bookingTime);
      cells[5] = UmiCsvText(row->currency);
      cells[6] = UmiCsvText(row->amountText);
      cells[7] = UmiCsvText(row->credit ? "Credit" : "Debit");
      cells[8] = UmiCsvText(row->booked ? "Booked" : "Pending");
      cells[9] = UmiCsvText(row->mutableRecord ? "true" : "false");
      cells[10] = UmiCsvText(row->information);
    }
    status = UmiCsvDocumentAppendRow(csv, cells, 14U);
  }
  if (status == UMI_STATUS_OK)
    *out = csv;
  else
    UmiCsvDocumentDestroy(csv);
  return status;
}
