/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_feed/statement.c
 * PURPOSE: Review an immutable statement period with exact arithmetic and explicit scope.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "statement_private.h"
#include <stdlib.h>
#include <string.h>

UmiStatus UmiBankStatementDate(const char *text, UmiFinancialDate *out) {
  if (text == NULL || out == NULL || strlen(text) != 10U || text[4] != '-' || text[7] != '-')
    return UMI_STATUS_INVALID_ARGUMENT;
  unsigned digits[10] = {0};
  for (size_t i = 0; i < 10U; ++i) {
    if (i == 4U || i == 7U)
      continue;
    if (text[i] < '0' || text[i] > '9')
      return UMI_STATUS_INVALID_ARGUMENT;
    digits[i] = (unsigned)(text[i] - '0');
  }
  UmiFinancialDate date = {
      (int32_t)(digits[0] * 1000U + digits[1] * 100U + digits[2] * 10U + digits[3]),
      (uint8_t)(digits[5] * 10U + digits[6]), (uint8_t)(digits[8] * 10U + digits[9])};
  if (date.year < 1600 || date.year > 9999 || !umi_financial_date_is_valid(date))
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = date;
  return UMI_STATUS_OK;
}
bool BankStatementAccount(const UmiBankFeedRow *row, const UmiBankStatementRequest *request) {
  return strcmp(row->account, request->account) == 0 &&
         strcmp(row->currency, request->currency) == 0;
}
bool BankStatementInPeriod(const UmiBankFeedRow *row, const UmiBankStatementRequest *request) {
  char text[11];
  memcpy(text, row->bookingTime, 10U);
  text[10] = '\0';
  UmiFinancialDate date;
  return UmiBankStatementDate(text, &date) == UMI_STATUS_OK &&
         umi_financial_date_compare(date, request->firstDate) >= 0 &&
         umi_financial_date_compare(date, request->lastDate) <= 0;
}
UmiStatus UmiBankFeedClone(const UmiBankFeed *feed, UmiBankFeed **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = NULL;
  if (feed == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiBankFeed *copy = calloc(1U, sizeof *copy);
  if (copy == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  if (feed->summary.rows != 0U) {
    /* Feed creation bounds the row count, so multiplication cannot wrap.
     * Separate storage lets review windows outlive their source selection. */
    copy->rows = malloc(feed->summary.rows * sizeof *copy->rows);
    if (copy->rows == NULL) {
      free(copy);
      return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy->rows, feed->rows, feed->summary.rows * sizeof *copy->rows);
  }
  copy->summary = feed->summary;
  *out = copy;
  return UMI_STATUS_OK;
}
UmiStatus UmiBankStatementReview(const UmiBankFeed *feed, const UmiBankStatementRequest *request,
                                 UmiBankStatementResult *out) {
  if (feed == NULL || request == NULL || out == NULL || request->account == NULL ||
      request->account[0] == '\0' || request->currency == NULL || strlen(request->currency) != 3U ||
      request->openingBalance.scale > 9U || request->closingBalance.scale > 9U ||
      request->firstDate.year < 1600 || request->firstDate.year > 9999 ||
      request->lastDate.year < 1600 || request->lastDate.year > 9999 ||
      !umi_financial_date_is_valid(request->firstDate) ||
      !umi_financial_date_is_valid(request->lastDate) ||
      umi_financial_date_compare(request->firstDate, request->lastDate) > 0)
    return UMI_STATUS_INVALID_ARGUMENT;
  for (size_t i = 0; i < 3U; ++i)
    if (request->currency[i] < 'A' || request->currency[i] > 'Z')
      return UMI_STATUS_INVALID_ARGUMENT;
  UmiBankStatementResult result = {0};
  bool found = false;
  result.hasPreviousPage = feed->summary.hasPreviousPage;
  result.hasNextPage = feed->summary.hasNextPage;
  for (size_t i = 0; i < feed->summary.rows; ++i) {
    const UmiBankFeedRow *row = &feed->rows[i];
    if (!BankStatementAccount(row, request))
      continue;
    found = true;
    if (!BankStatementInPeriod(row, request)) {
      ++result.outsidePeriodRows;
      continue;
    }
    if (row->transaction[0] == '\0')
      ++result.unidentifiedRows;
    if (!row->booked) {
      ++result.pendingRows;
      continue;
    }
    ++result.bookedRows;
    if (row->mutableRecord)
      ++result.mutableBookedRows;
    UmiDecimal *total = row->credit ? &result.credits : &result.debits;
    UmiStatus status = UmiDecimalAddExact(*total, row->amount, total);
    if (status != UMI_STATUS_OK)
      return status;
  }
  if (!found)
    return UMI_STATUS_NOT_FOUND;
  /* Publish only after every exact operation succeeds. Callers never receive
   * a half-updated comparison when a large imported sum exceeds capacity. */
  UmiStatus status = UmiDecimalSubtractExact(result.credits, result.debits, &result.netMovement);
  if (status == UMI_STATUS_OK)
    status =
        UmiDecimalAddExact(request->openingBalance, result.netMovement, &result.expectedClosing);
  if (status == UMI_STATUS_OK)
    status = UmiDecimalSubtractExact(request->closingBalance, result.expectedClosing,
                                     &result.difference);
  if (status == UMI_STATUS_OK) {
    result.balancesMatch = result.difference.coefficient == 0;
    *out = result;
  }
  return status;
}
