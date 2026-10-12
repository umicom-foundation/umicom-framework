/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_feed/transactions.c
 * PURPOSE: Keep exact bank totals separate by account, currency and posting status.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "feed_private.h"
#include <stdlib.h>
#include <string.h>
/* A feed owns its rows, while callers receive copies. Releasing the feed
 * therefore cannot invalidate a row value already copied into another tool. */
void UmiBankFeedDestroy(UmiBankFeed *feed) {
  if (feed != NULL) {
    free(feed->rows);
    free(feed);
  }
}
UmiStatus UmiBankFeedRead(const UmiBankFeed *feed, UmiBankFeedSummary *out) {
  if (feed == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = feed->summary;
  return UMI_STATUS_OK;
}
UmiStatus UmiBankFeedAt(const UmiBankFeed *feed, size_t index, UmiBankFeedRow *out) {
  if (feed == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (index >= feed->summary.rows)
    return UMI_STATUS_NOT_FOUND;
  *out = feed->rows[index];
  return UMI_STATUS_OK;
}
/* Account and currency are separate identity boundaries. Booked and pending
 * values are accumulated independently so an expected payment never becomes
 * spendable money merely because it appears in the same imported page. */
UmiStatus UmiBankFeedSum(const UmiBankFeed *feed, const char *account, const char *currency,
                         UmiBankFeedTotals *out) {
  if (feed == NULL || account == NULL || currency == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiBankFeedTotals totals = {0};
  for (size_t i = 0; i < feed->summary.rows; ++i) {
    const UmiBankFeedRow *row = &feed->rows[i];
    if (strcmp(row->account, account) != 0 || strcmp(row->currency, currency) != 0)
      continue;
    UmiDecimal *value = row->booked
                            ? (row->credit ? &totals.bookedCredits : &totals.bookedDebits)
                            : (row->credit ? &totals.pendingCredits : &totals.pendingDebits);
    UmiStatus status = UmiDecimalAddExact(*value, row->amount, value);
    if (status != UMI_STATUS_OK)
      return status;
    if (row->booked)
      ++totals.bookedRows;
    else
      ++totals.pendingRows;
    if (row->mutableRecord)
      ++totals.mutableRows;
    if (row->transaction[0] == '\0')
      ++totals.unidentifiedRows;
  }
  if (totals.bookedRows + totals.pendingRows == 0U)
    return UMI_STATUS_NOT_FOUND;
  /* Debits are nonnegative magnitudes, so negating a successfully accumulated
   * total is representable. Calculate into the local result before publishing. */
  UmiDecimal debit = totals.bookedDebits;
  debit.coefficient = -debit.coefficient;
  UmiStatus status = UmiDecimalAddExact(totals.bookedCredits, debit, &totals.bookedNet);
  debit = totals.pendingDebits;
  debit.coefficient = -debit.coefficient;
  if (status == UMI_STATUS_OK)
    status = UmiDecimalAddExact(totals.pendingCredits, debit, &totals.pendingNet);
  if (status == UMI_STATUS_OK)
    *out = totals;
  return status;
}
