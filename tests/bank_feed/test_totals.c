/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_feed/test_totals.c
 * PURPOSE: Ensure different currencies and pending entries never become one synthetic balance.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(void) {
  char *debit = BankReplace(BANK_TEST_ROW, "Credit", "Debit");
  CHECK(debit);
  char *debitId = BankReplace(debit, "credit-one", "debit-one");
  CHECK(debitId);
  char *pending = BankReplace(debitId, "BOOK", "PDNG");
  CHECK(pending);
  char *pendingId = BankReplace(pending, "debit-one", "pending-one");
  CHECK(pendingId);
  char *euro = BankReplace(BANK_TEST_ROW, "GBP", "EUR");
  CHECK(euro);
  char *euroId = BankReplace(euro, "credit-one", "euro-one");
  CHECK(euroId);
  char *otherAccount = BankReplace(BANK_TEST_ROW, "practice-account", "other-account");
  CHECK(otherAccount);
  size_t bytes = strlen(BANK_TEST_ROW) + strlen(debitId) + strlen(pendingId) + strlen(euroId) +
                 strlen(otherAccount) + 8U;
  char *rows = malloc(bytes);
  CHECK(rows);
  (void)snprintf(rows, bytes, "%s,%s,%s,%s,%s", BANK_TEST_ROW, debitId, pendingId, euroId,
                 otherAccount);
  char *text = BankFixture(rows);
  CHECK(text);
  UmiBankFeed *feed = NULL;
  CHECK(UmiBankFeedCreate(text, strlen(text), &feed) == UMI_STATUS_OK);
  UmiBankFeedTotals totals;
  CHECK(UmiBankFeedSum(feed, "practice-account", "GBP", &totals) == UMI_STATUS_OK);
  CHECK(totals.bookedRows == 2U && totals.pendingRows == 1U && totals.bookedNet.coefficient == 0);
  CHECK(totals.pendingNet.coefficient == -1250 && totals.pendingNet.scale == 2U);
  CHECK(UmiBankFeedSum(feed, "practice-account", "EUR", &totals) == UMI_STATUS_OK);
  CHECK(totals.bookedRows == 1U && totals.pendingRows == 0U &&
        totals.bookedNet.coefficient == 1250);
  CHECK(UmiBankFeedSum(feed, "other-account", "GBP", &totals) == UMI_STATUS_OK);
  CHECK(totals.bookedRows == 1U && totals.bookedNet.coefficient == 1250);
  UmiBankFeedDestroy(feed);
  free(text);
  free(rows);
  free(debit);
  free(debitId);
  free(pending);
  free(pendingId);
  free(euro);
  free(euroId);
  free(otherAccount);
  return 0;
}
