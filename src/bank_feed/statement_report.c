/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_feed/statement_report.c
 * PURPOSE: Export period evidence together with balance assumptions and page limitations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "statement_private.h"
#include <stdio.h>

UmiStatus UmiBankStatementExportCsv(const UmiBankFeed *feed, const UmiBankStatementRequest *request,
                                    UmiCsvDocument **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = NULL;
  UmiBankStatementResult review;
  UmiStatus status = UmiBankStatementReview(feed, request, &review);
  if (status != UMI_STATUS_OK)
    return status;
  char numbers[7][UMI_DECIMAL_TEXT_MAX];
  const UmiDecimal values[] = {
      request->openingBalance, request->closingBalance, review.credits,   review.debits,
      review.netMovement,      review.expectedClosing,  review.difference};
  for (size_t i = 0; i < 7U; ++i) {
    status = UmiDecimalFormat(values[i], numbers[i], sizeof numbers[i]);
    if (status != UMI_STATUS_OK)
      return status;
  }
  char first[16], last[16];
  (void)snprintf(first, sizeof first, "%04d-%02u-%02u", (int)request->firstDate.year,
                 (unsigned)request->firstDate.month, (unsigned)request->firstDate.day);
  (void)snprintf(last, sizeof last, "%04d-%02u-%02u", (int)request->lastDate.year,
                 (unsigned)request->lastDate.month, (unsigned)request->lastDate.day);
  const char *headings[] = {"record_kind",
                            "account_id",
                            "currency",
                            "first_booking_date",
                            "last_booking_date",
                            "opening_balance",
                            "reported_closing",
                            "booked_credits",
                            "booked_debits",
                            "net_movement",
                            "expected_closing",
                            "difference",
                            "arithmetic_matches",
                            "booked_rows",
                            "pending_rows",
                            "outside_period_rows",
                            "mutable_booked_rows",
                            "unidentified_rows",
                            "previous_page",
                            "next_page",
                            "transaction_id",
                            "booking_time",
                            "amount",
                            "direction",
                            "status",
                            "mutable",
                            "reference",
                            "information",
                            "scope"};
  UmiCsvDocument *csv = NULL;
  status = UmiCsvDocumentCreate(UMI_CSV_MAX_BYTES, &csv);
  UmiCsvCell cells[29];
  for (size_t i = 0; i < 29U; ++i)
    cells[i] = UmiCsvText(headings[i]);
  if (status == UMI_STATUS_OK)
    status = UmiCsvDocumentAppendRow(csv, cells, 29U);
  /* Repeat the comparison inputs on each observation. Filtering a spreadsheet
   * must not detach an entry from its date range, account or source limits. */
  for (size_t i = 0; status == UMI_STATUS_OK && i <= feed->summary.rows; ++i) {
    const UmiBankFeedRow *row = i == 0U ? NULL : &feed->rows[i - 1U];
    if (row != NULL &&
        (!BankStatementAccount(row, request) || !BankStatementInPeriod(row, request)))
      continue;
    for (size_t j = 0; j < 29U; ++j)
      cells[j] = UmiCsvText("");
    cells[0] = UmiCsvText(row == NULL ? "review" : "transaction");
    cells[1] = UmiCsvText(request->account);
    cells[2] = UmiCsvText(request->currency);
    cells[3] = UmiCsvText(first);
    cells[4] = UmiCsvText(last);
    for (size_t j = 0; j < 7U; ++j)
      cells[5U + j] = UmiCsvText(numbers[j]);
    cells[12] = UmiCsvText(review.balancesMatch ? "true" : "false");
    cells[13] = UmiCsvUnsigned(review.bookedRows);
    cells[14] = UmiCsvUnsigned(review.pendingRows);
    cells[15] = UmiCsvUnsigned(review.outsidePeriodRows);
    cells[16] = UmiCsvUnsigned(review.mutableBookedRows);
    cells[17] = UmiCsvUnsigned(review.unidentifiedRows);
    cells[18] = UmiCsvText(review.hasPreviousPage ? "true" : "false");
    cells[19] = UmiCsvText(review.hasNextPage ? "true" : "false");
    if (row != NULL) {
      cells[20] = UmiCsvText(row->transaction);
      cells[21] = UmiCsvText(row->bookingTime);
      cells[22] = UmiCsvText(row->amountText);
      cells[23] = UmiCsvText(row->credit ? "Credit" : "Debit");
      cells[24] = UmiCsvText(row->booked ? "Booked" : "Pending");
      cells[25] = UmiCsvText(row->mutableRecord ? "true" : "false");
      cells[26] = UmiCsvText(row->reference);
      cells[27] = UmiCsvText(row->information);
    }
    cells[28] = UmiCsvText("Imported snapshot; provider calendar dates; supplied balances; "
                           "pending excluded; completeness and authenticity unverified");
    status = UmiCsvDocumentAppendRow(csv, cells, 29U);
  }
  if (status == UMI_STATUS_OK)
    *out = csv;
  else
    UmiCsvDocumentDestroy(csv);
  return status;
}
