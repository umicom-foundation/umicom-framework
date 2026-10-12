/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_feed/test_transactions.c
 * PURPOSE: Exercise malformed imports, exact totals, privacy boundaries and page-limited reports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include <stdint.h>
static int Refusal(const char *name) {
  const char *find = NULL, *replacement = NULL;
  if (!strcmp(name, "negative")) {
    find = "12.50";
    replacement = "-12.50";
  } else if (!strcmp(name, "exponent")) {
    find = "12.50";
    replacement = "1.25e1";
  } else if (!strcmp(name, "fraction")) {
    find = "12.50";
    replacement = "1.000001";
  } else if (!strcmp(name, "integer-bound")) {
    find = "12.50";
    replacement = "10000000000000";
  } else if (!strcmp(name, "numeric-kind")) {
    find = "\"12.50\"";
    replacement = "12.50";
  } else if (!strcmp(name, "currency")) {
    find = "GBP";
    replacement = "gbp";
  } else if (!strcmp(name, "status")) {
    find = "BOOK";
    replacement = "REJECTED";
  } else if (!strcmp(name, "direction")) {
    find = "Credit";
    replacement = "Either";
  } else if (!strcmp(name, "duplicate-key")) {
    find = "\"Status\":\"BOOK\"";
    replacement = "\"Status\":\"BOOK\",\"Status\":\"PDNG\"";
  } else if (!strcmp(name, "escaped-key")) {
    find = "\"Status\":\"BOOK\"";
    replacement = "\"Status\":\"BOOK\",\"\\u0053tatus\":\"PDNG\"";
  } else if (!strcmp(name, "null-id")) {
    find = "\"credit-one\"";
    replacement = "null";
  } else if (!strcmp(name, "bad-date")) {
    find = "2026-10-12";
    replacement = "2026-02-30";
  } else if (!strcmp(name, "bad-offset")) {
    find = "10:00:00Z";
    replacement = "10:00:00+14:01";
  } else if (!strcmp(name, "no-zone")) {
    find = "10:00:00Z";
    replacement = "10:00:00";
  } else if (!strcmp(name, "control")) {
    find = "Example credit";
    replacement = "Example\\ncredit";
  } else if (!strcmp(name, "nul")) {
    find = "Example credit";
    replacement = "Example\\u0000credit";
  } else if (!strcmp(name, "mutability")) {
    find = "\"Status\":\"BOOK\"";
    replacement = "\"Status\":\"BOOK\",\"TransactionMutability\":\"perhaps\"";
  } else
    return -1;
  char *row = BankReplace(BANK_TEST_ROW, find, replacement);
  CHECK(row);
  char *text = BankFixture(row);
  CHECK(text);
  UmiBankFeed *feed = NULL;
  CHECK(UmiBankFeedCreate(text, strlen(text), &feed) != UMI_STATUS_OK && feed == NULL);
  free(text);
  free(row);
  return 0;
}
static int Run(const char *name) {
  int refusal = Refusal(name);
  if (refusal >= 0)
    return refusal;
  char *text = BankFixture(BANK_TEST_ROW);
  CHECK(text);
  UmiBankFeed *feed = NULL;
  if (!strcmp(name, "empty")) {
    CHECK(UmiBankFeedCreate("{\"Data\":{}}", 11U, &feed) == UMI_STATUS_OK);
    UmiBankFeedSummary summary;
    CHECK(UmiBankFeedRead(feed, &summary) == UMI_STATUS_OK && summary.rows == 0U);
  } else if (!strcmp(name, "byte-limit")) {
    CHECK(UmiBankFeedCreate(text, UMI_BANK_FEED_MAX_BYTES + 1U, &feed) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
  } else if (!strcmp(name, "wrong-array")) {
    const char *bad = "{\"Data\":{\"Transaction\":{}}}";
    CHECK(UmiBankFeedCreate(bad, strlen(bad), &feed) == UMI_STATUS_PARSE_ERROR);
  } else if (!strcmp(name, "trailing")) {
    char *bad = malloc(strlen(text) + 2U);
    CHECK(bad);
    (void)snprintf(bad, strlen(text) + 2U, "%s!", text);
    CHECK(UmiBankFeedCreate(bad, strlen(bad), &feed) != UMI_STATUS_OK);
    free(bad);
  } else if (!strcmp(name, "row-limit") || !strcmp(name, "duplicate-id") ||
             !strcmp(name, "overflow")) {
    size_t count = !strcmp(name, "row-limit") ? UMI_BANK_FEED_MAX_ROWS + 1U
                                              : (!strcmp(name, "overflow") ? 10U : 2U);
    char *row = BankReplace(BANK_TEST_ROW, "\"TransactionId\":\"credit-one\",", "");
    CHECK(row);
    char *large = BankReplace(row, "12.50", "9999999999999.99999");
    CHECK(large);
    const char *chosen =
        !strcmp(name, "duplicate-id") ? BANK_TEST_ROW : (!strcmp(name, "overflow") ? large : row);
    size_t capacity = count * (strlen(chosen) + 1U) + 1U;
    char *rows = calloc(capacity, 1U);
    CHECK(rows);
    for (size_t i = 0; i < count; ++i) {
      if (i)
        strcat(rows, ",");
      strcat(rows, chosen);
    }
    free(text);
    text = BankFixture(rows);
    CHECK(text);
    free(rows);
    free(row);
    free(large);
    UmiStatus status = UmiBankFeedCreate(text, strlen(text), &feed);
    if (!strcmp(name, "overflow")) {
      CHECK(status == UMI_STATUS_OK);
      UmiBankFeedTotals totals = {0};
      unsigned char old[sizeof totals];
      memcpy(old, &totals, sizeof totals);
      CHECK(UmiBankFeedSum(feed, "practice-account", "GBP", &totals) ==
            UMI_STATUS_CAPACITY_EXCEEDED);
      CHECK(memcmp(old, &totals, sizeof totals) == 0);
    } else
      CHECK(status == (!strcmp(name, "row-limit") ? UMI_STATUS_CAPACITY_EXCEEDED
                                                  : UMI_STATUS_ALREADY_EXISTS));
  } else {
    char *replacement = NULL;
    if (!strcmp(name, "pending"))
      replacement = BankReplace(text, "BOOK", "PDNG");
    else if (!strcmp(name, "legacy"))
      replacement = BankReplace(text, "BOOK", "Booked");
    else if (!strcmp(name, "mutable"))
      replacement = BankReplace(text, "\"Status\":\"BOOK\"",
                                "\"Status\":\"BOOK\",\"TransactionMutability\":\"Mutable\"");
    else if (!strcmp(name, "offset"))
      replacement = BankReplace(text, "10:00:00Z", "10:00:00.123456789-05:30");
    else if (!strcmp(name, "missing-id"))
      replacement = BankReplace(text, "\"TransactionId\":\"credit-one\",", "");
    else if (!strcmp(name, "unicode"))
      replacement = BankReplace(text, "Example credit", "Caf\\u00e9");
    else if (!strcmp(name, "formula"))
      replacement = BankReplace(text, "Example credit", "=HYPERLINK(\\\"example\\\")");
    else if (!strcmp(name, "pagination"))
      replacement = BankReplace(
          text, "{\"Data\":",
          "{\"Links\":{\"Next\":\"https://invalid.example/next\",\"Prev\":\"previous\"},\"Data\":");
    else if (!strcmp(name, "discard-extensions"))
      replacement =
          BankReplace(text, "{\"Data\":", "{\"AccessToken\":\"secret-must-not-export\",\"Data\":");
    if (replacement) {
      free(text);
      text = replacement;
    }
    CHECK(UmiBankFeedCreate(text, strlen(text), &feed) == UMI_STATUS_OK);
    UmiBankFeedSummary summary;
    CHECK(UmiBankFeedRead(feed, &summary) == UMI_STATUS_OK && summary.rows == 1U);
    UmiBankFeedRow row;
    CHECK(UmiBankFeedAt(feed, 0U, &row) == UMI_STATUS_OK);
    UmiBankFeedTotals totals;
    CHECK(UmiBankFeedSum(feed, row.account, row.currency, &totals) == UMI_STATUS_OK);
    if (!strcmp(name, "pending"))
      CHECK(!row.booked && row.mutableRecord && totals.pendingCredits.coefficient == 1250);
    else if (!strcmp(name, "mutable"))
      CHECK(row.booked && row.mutableRecord && totals.mutableRows == 1U);
    else if (!strcmp(name, "missing-id"))
      CHECK(summary.missingIdentifiers == 1U && totals.unidentifiedRows == 1U);
    else if (!strcmp(name, "pagination"))
      CHECK(summary.hasNextPage && summary.hasPreviousPage);
    else if (!strcmp(name, "unicode"))
      CHECK(!strcmp(row.information, "Caf\xc3\xa9"));
    else if (!strcmp(name, "ownership")) {
      memset(text, 'x', strlen(text));
      CHECK(UmiBankFeedAt(feed, 0U, &row) == UMI_STATUS_OK &&
            !strcmp(row.account, "practice-account"));
    } else if (!strcmp(name, "unchanged")) {
      unsigned char previous[sizeof row];
      memcpy(previous, &row, sizeof row);
      CHECK(UmiBankFeedAt(feed, 99U, &row) == UMI_STATUS_NOT_FOUND);
      CHECK(memcmp(previous, &row, sizeof row) == 0);
      unsigned char previousTotals[sizeof totals];
      memcpy(previousTotals, &totals, sizeof totals);
      CHECK(UmiBankFeedSum(feed, "other", "GBP", &totals) == UMI_STATUS_NOT_FOUND);
      CHECK(memcmp(previousTotals, &totals, sizeof totals) == 0);
    } else if (!strcmp(name, "values") || !strcmp(name, "legacy") || !strcmp(name, "offset"))
      CHECK(row.amount.coefficient == 1250 && row.amount.scale == 2U &&
            totals.bookedNet.coefficient == 1250);
    else if (strcmp(name, "formula") && strcmp(name, "discard-extensions")) {
      free(text);
      UmiBankFeedDestroy(feed);
      return 2;
    }
    UmiCsvDocument *csv = NULL;
    CHECK(UmiBankFeedExportCsv(feed, &csv) == UMI_STATUS_OK);
    CHECK(UmiCsvDocumentRows(csv) == 3U);
    if (!strcmp(name, "formula"))
      CHECK(strstr(UmiCsvDocumentData(csv), "'=HYPERLINK") != NULL);
    if (!strcmp(name, "discard-extensions"))
      CHECK(strstr(UmiCsvDocumentData(csv), "secret-must-not-export") == NULL);
    UmiBankFeedDestroy(feed);
    feed = NULL;
    CHECK(strstr(UmiCsvDocumentData(csv), "practice-account") != NULL);
    UmiCsvDocumentDestroy(csv);
  }
  UmiBankFeedDestroy(feed);
  free(text);
  return 0;
}
int main(int argc, char **argv) { return argc == 2 ? Run(argv[1]) : 2; }
