/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_feed/test_statement.c
 * PURPOSE: Exercise account isolation, period boundaries and exact balance comparisons.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/bank_feed/statement.h"
#include <stdint.h>
static const char rows[] =
    BANK_TEST_ROW ","
                  "{\"AccountId\":\"practice-account\",\"TransactionId\":\"debit-one\","
                  "\"CreditDebitIndicator\":\"Debit\",\"Status\":\"BOOK\","
                  "\"BookingDateTime\":\"2026-10-12T23:59:00-05:00\","
                  "\"Amount\":{\"Amount\":\"2.25\",\"Currency\":\"GBP\"}},"
                  "{\"AccountId\":\"practice-account\",\"TransactionId\":\"pending-one\","
                  "\"CreditDebitIndicator\":\"Debit\",\"Status\":\"PDNG\","
                  "\"BookingDateTime\":\"2026-10-12T00:00:00+14:00\","
                  "\"Amount\":{\"Amount\":\"999.00\",\"Currency\":\"GBP\"}},"
                  "{\"AccountId\":\"practice-account\",\"TransactionId\":\"earlier\","
                  "\"CreditDebitIndicator\":\"Credit\",\"Status\":\"BOOK\","
                  "\"BookingDateTime\":\"2026-10-11T23:59:00Z\","
                  "\"Amount\":{\"Amount\":\"77\",\"Currency\":\"GBP\"}},"
                  "{\"AccountId\":\"other-account\",\"TransactionId\":\"other-credit\","
                  "\"CreditDebitIndicator\":\"Credit\",\"Status\":\"BOOK\","
                  "\"BookingDateTime\":\"2026-10-12T10:00:00Z\","
                  "\"Amount\":{\"Amount\":\"400\",\"Currency\":\"GBP\"}},"
                  "{\"AccountId\":\"practice-account\",\"TransactionId\":\"euro-credit\","
                  "\"CreditDebitIndicator\":\"Credit\",\"Status\":\"BOOK\","
                  "\"BookingDateTime\":\"2026-10-12T10:00:00Z\","
                  "\"Amount\":{\"Amount\":\"700\",\"Currency\":\"EUR\"}}";
static int Arithmetic(void) {
  UmiDecimal out = {42, 0}, a = {INT64_MIN, 0}, b = {INT64_MIN, 0};
  CHECK(UmiDecimalSubtractExact(a, b, &out) == UMI_STATUS_OK && out.coefficient == 0);
  a.coefficient = INT64_MAX;
  b.coefficient = -1;
  out.coefficient = 42;
  CHECK(UmiDecimalSubtractExact(a, b, &out) == UMI_STATUS_CAPACITY_EXCEEDED);
  CHECK(out.coefficient == 42);
  a.coefficient = INT64_MIN;
  b.coefficient = 1;
  CHECK(UmiDecimalSubtractExact(a, b, &out) == UMI_STATUS_CAPACITY_EXCEEDED);
  CHECK(out.coefficient == 42);
  a = (UmiDecimal){125, 2};
  b = (UmiDecimal){2, 1};
  CHECK(UmiDecimalSubtractExact(a, b, &out) == UMI_STATUS_OK);
  CHECK(out.coefficient == 105 && out.scale == 2);
  a = (UmiDecimal){INT64_MAX, 0};
  b = (UmiDecimal){0, 1};
  CHECK(UmiDecimalSubtractExact(a, b, &out) == UMI_STATUS_CAPACITY_EXCEEDED);
  CHECK(out.coefficient == 105 && out.scale == 2);
  a.scale = 10;
  CHECK(UmiDecimalSubtractExact(a, b, &out) == UMI_STATUS_INVALID_ARGUMENT);
  CHECK(UmiDecimalSubtractExact(b, b, NULL) == UMI_STATUS_INVALID_ARGUMENT);
  return 0;
}
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *mode = argv[1];
  if (!strcmp(mode, "arithmetic"))
    return Arithmetic();
  if (!strcmp(mode, "dates")) {
    UmiFinancialDate date = {2000, 1, 1};
    CHECK(UmiBankStatementDate("2024-02-29", &date) == UMI_STATUS_OK);
    const char *invalid[] = {"2023-02-29",  "1900-02-29",  "1599-12-31", "10000-01-01",
                             "2026-13-01",  "2026-01-00",  "2026-04-31", "2026-1-01",
                             " 2026-01-01", "2026-01-01Z", NULL};
    for (size_t i = 0; i < sizeof invalid / sizeof invalid[0]; ++i) {
      CHECK(UmiBankStatementDate(invalid[i], &date) == UMI_STATUS_INVALID_ARGUMENT);
      CHECK(date.year == 2024 && date.month == 2 && date.day == 29);
    }
    CHECK(UmiBankStatementDate("2000-02-29", &date) == UMI_STATUS_OK);
    CHECK(UmiBankStatementDate("2026-01-01", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
  }
  char *document = BankFixture(rows);
  CHECK(document);
  const char *find = NULL, *replacement = NULL;
  if (!strcmp(mode, "mutable")) {
    find = "\"Status\":\"BOOK\"";
    replacement = "\"Status\":\"BOOK\",\"StatementReference\":[\"ignored\"],"
                  "\"TransactionMutability\":\"Mutable\"";
  } else if (!strcmp(mode, "missing-id")) {
    find = "\"TransactionId\":\"credit-one\",";
    replacement = "";
  } else if (!strcmp(mode, "formula")) {
    find = "Example credit";
    replacement = "=1+2";
  } else if (!strcmp(mode, "pagination")) {
    find = "{\"Data\":";
    replacement = "{\"Links\":{\"Next\":\"https://example.invalid/next\",\"Prev\":\"https://"
                  "example.invalid/prev\"},\"Data\":";
  }
  if (find != NULL) {
    char *changed = BankReplace(document, find, replacement);
    free(document);
    document = changed;
    CHECK(document);
  }
  UmiBankFeed *feed = NULL;
  CHECK(UmiBankFeedCreate(document, strlen(document), &feed) == UMI_STATUS_OK);
  free(document);
  UmiBankStatementRequest request = {"practice-account", "GBP",      {2026, 10, 12},
                                     {2026, 10, 12},     {10000, 2}, {11025, 2}};
  UmiStatus expected = UMI_STATUS_OK;
  if (!strcmp(mode, "mismatch"))
    request.closingBalance.coefficient = 11026;
  else if (!strcmp(mode, "empty-period")) {
    request.firstDate.day = 13;
    request.lastDate.day = 13;
    request.closingBalance = request.openingBalance;
  } else if (!strcmp(mode, "range")) {
    request.firstDate.day = 13;
    expected = UMI_STATUS_INVALID_ARGUMENT;
  } else if (!strcmp(mode, "invalid-date")) {
    request.firstDate.month = 2;
    request.firstDate.day = 30;
    expected = UMI_STATUS_INVALID_ARGUMENT;
  } else if (!strcmp(mode, "currency")) {
    request.currency = "gbp";
    expected = UMI_STATUS_INVALID_ARGUMENT;
  } else if (!strcmp(mode, "not-found")) {
    request.account = "absent";
    expected = UMI_STATUS_NOT_FOUND;
  } else if (!strcmp(mode, "overflow")) {
    request.openingBalance = (UmiDecimal){INT64_MAX, 2};
    expected = UMI_STATUS_CAPACITY_EXCEEDED;
  } else if (!strcmp(mode, "difference-overflow")) {
    request.openingBalance = (UmiDecimal){INT64_MIN, 2};
    expected = UMI_STATUS_CAPACITY_EXCEEDED;
  } else if (!strcmp(mode, "scale")) {
    request.openingBalance.scale = 10;
    expected = UMI_STATUS_INVALID_ARGUMENT;
  } else if (!strcmp(mode, "negative-opening")) {
    request.openingBalance = (UmiDecimal){-2000, 2};
    request.closingBalance = (UmiDecimal){-975, 2};
  } else if (!strcmp(mode, "mixed-scale")) {
    request.openingBalance = (UmiDecimal){100, 0};
  } else if (!strcmp(mode, "clone")) {
    UmiBankFeed *copy = NULL;
    CHECK(UmiBankFeedClone(feed, &copy) == UMI_STATUS_OK);
    UmiBankFeedDestroy(feed);
    feed = copy;
  }
  UmiBankStatementResult output;
  memset(&output, 0x5a, sizeof output);
  UmiBankStatementResult untouched;
  memcpy(&untouched, &output, sizeof output);
  CHECK(UmiBankStatementReview(feed, &request, &output) == expected);
  if (expected != UMI_STATUS_OK) {
    CHECK(memcmp(&untouched, &output, sizeof output) == 0);
    UmiCsvDocument *csv = (UmiCsvDocument *)(uintptr_t)1;
    CHECK(UmiBankStatementExportCsv(feed, &request, &csv) == expected && csv == NULL);
  } else {
    CHECK(output.balancesMatch == (strcmp(mode, "mismatch") != 0));
    if (!strcmp(mode, "empty-period")) {
      CHECK(output.bookedRows == 0 && output.pendingRows == 0);
      CHECK(output.outsidePeriodRows == 4 && output.netMovement.coefficient == 0);
    } else {
      CHECK(output.bookedRows == 2 && output.pendingRows == 1);
      CHECK(output.outsidePeriodRows == 1 && output.netMovement.coefficient == 1025);
      CHECK(output.credits.coefficient == 1250 && output.debits.coefficient == 225);
    }
    if (!strcmp(mode, "mutable"))
      CHECK(output.mutableBookedRows == 1);
    if (!strcmp(mode, "missing-id"))
      CHECK(output.unidentifiedRows == 1);
    if (!strcmp(mode, "pagination"))
      CHECK(output.hasPreviousPage && output.hasNextPage);
    if (!strcmp(mode, "mismatch"))
      CHECK(output.difference.coefficient == 1);
    UmiCsvDocument *csv = NULL;
    CHECK(UmiBankStatementExportCsv(feed, &request, &csv) == UMI_STATUS_OK);
    const char *text = UmiCsvDocumentData(csv);
    CHECK(strstr(text, "reported_closing") &&
          strstr(text, "completeness and authenticity unverified"));
    CHECK(!strstr(text, "other-credit") && !strstr(text, "euro-credit") &&
          !strstr(text, "\"earlier\""));
    CHECK(UmiCsvDocumentRows(csv) == (!strcmp(mode, "empty-period") ? 2U : 5U));
    if (!strcmp(mode, "formula"))
      CHECK(strstr(text, "'=1+2"));
    UmiCsvDocumentDestroy(csv);
  }
  CHECK(UmiBankStatementReview(NULL, &request, &output) == UMI_STATUS_INVALID_ARGUMENT);
  CHECK(UmiBankStatementReview(feed, NULL, &output) == UMI_STATUS_INVALID_ARGUMENT);
  CHECK(UmiBankStatementReview(feed, &request, NULL) == UMI_STATUS_INVALID_ARGUMENT);
  CHECK(UmiBankStatementExportCsv(feed, &request, NULL) == UMI_STATUS_INVALID_ARGUMENT);
  UmiBankFeed *copy = (UmiBankFeed *)(uintptr_t)1;
  CHECK(UmiBankFeedClone(NULL, &copy) == UMI_STATUS_INVALID_ARGUMENT && copy == NULL);
  UmiBankFeedDestroy(feed);
  return 0;
}
