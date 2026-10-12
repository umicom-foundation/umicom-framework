/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_account_report.c
 * PURPOSE: Verify bounded account report choices against the existing framed transport.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/broker_connectivity/account_query.h"
static bool Contains(const unsigned char *bytes, size_t length, const char *text) {
  size_t count = strlen(text);
  if (count > length)
    return false;
  for (size_t i = 0; i <= length - count; ++i)
    if (memcmp(bytes + i, text, count) == 0)
      return true;
  return false;
}
static int Run(const char *name) {
  char tags[768] = "unchanged";
  if (!strcmp(name, "tags")) {
    CHECK(UmiIbkrAccountReportTags(UMI_IBKR_ACCOUNT_OVERVIEW, "", tags, sizeof tags) ==
          UMI_STATUS_OK);
    CHECK(!strcmp(tags, "NetLiquidation,TotalCashValue,BuyingPower,AvailableFunds"));
    CHECK(UmiIbkrAccountReportTags(UMI_IBKR_ACCOUNT_MARGIN, "", tags, sizeof tags) ==
          UMI_STATUS_OK);
    CHECK(strstr(tags, "LookAheadExcessLiquidity") && strstr(tags, "SettledCash"));
    CHECK(UmiIbkrAccountReportTags(UMI_IBKR_ACCOUNT_CURRENCY, "GBP", tags, sizeof tags) ==
          UMI_STATUS_OK);
    CHECK(!strcmp(tags, "$LEDGER:GBP"));
    return 0;
  }
  if (!strcmp(name, "short-output")) {
    CHECK(UmiIbkrAccountReportTags(UMI_IBKR_ACCOUNT_MARGIN, "", tags, 2U) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(!strcmp(tags, "unchanged"));
    return 0;
  }
  if (!strcmp(name, "bad-currency")) {
    const char *bad[] = {"", "gbp", "GB", "GBPP", "A,B", "ALL"};
    for (size_t i = 0; i < 5U; ++i) {
      CHECK(UmiIbkrAccountReportTags(UMI_IBKR_ACCOUNT_CURRENCY, bad[i], tags, sizeof tags) ==
            UMI_STATUS_INVALID_ARGUMENT);
      CHECK(!strcmp(tags, "unchanged"));
    }
    /* ALL is deliberately not a currency choice: the provider uses it for a
     * cross-account aggregate that this selected-account inspector cannot bind. */
    CHECK(UmiIbkrAccountReportTags(UMI_IBKR_ACCOUNT_CURRENCY, bad[5], tags, sizeof tags) ==
          UMI_STATUS_INVALID_ARGUMENT);
    return 0;
  }
  if (!strcmp(name, "bad-kind")) {
    CHECK(UmiIbkrAccountReportTags((UmiIbkrAccountReportKind)99, "", tags, sizeof tags) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiIbkrAccountReportTags(UMI_IBKR_ACCOUNT_MARGIN, "GBP", tags, sizeof tags) ==
          UMI_STATUS_INVALID_ARGUMENT);
    return 0;
  }
  Fixture *f = New();
  CHECK(f);
  if (!strcmp(name, "before-ready")) {
    CHECK(UmiIbkrConnectionReadAccountReport(f->c, "DU123", UMI_IBKR_ACCOUNT_MARGIN, "", 0U) ==
          UMI_STATUS_INVALID_STATE);
    Delete(f);
    return 0;
  }
  CHECK(Connect(f) == 0);
  if (!strcmp(name, "unlisted")) {
    CHECK(UmiIbkrConnectionReadAccountReport(f->c, "DU999", UMI_IBKR_ACCOUNT_MARGIN, "", 10U) ==
          UMI_STATUS_PERMISSION_DENIED);
    CHECK(!f->c->snapshot.requestIssued);
    Delete(f);
    return 0;
  }
  if (!strcmp(name, "backwards")) {
    CHECK(UmiIbkrConnectionReadAccountReport(f->c, "DU123", UMI_IBKR_ACCOUNT_MARGIN, "", 4U) ==
          UMI_STATUS_INVALID_ARGUMENT);
    Delete(f);
    return 0;
  }
  if (!strcmp(name, "queue") || !strcmp(name, "queue-second")) {
    CHECK(UmiIbkrAccountReportTags(UMI_IBKR_ACCOUNT_MARGIN, "", tags, sizeof tags) ==
          UMI_STATUS_OK);
    /* Leave room for the summary frame alone to exercise rollback when the
     * positions request cannot fit; neither request may become visible. */
    f->c->txSize =
        !strcmp(name, "queue") ? UMI_IBKR_TX_LIMIT - 1U : UMI_IBKR_TX_LIMIT - strlen(tags) - 20U;
    size_t previous = f->c->txSize;
    CHECK(UmiIbkrConnectionReadAccountReport(f->c, "DU123", UMI_IBKR_ACCOUNT_MARGIN, "", 10U) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(f->c->txSize == previous && !f->c->snapshot.requestIssued);
    Delete(f);
    return 0;
  }
  UmiIbkrAccountReportKind kind =
      !strcmp(name, "margin") ? UMI_IBKR_ACCOUNT_MARGIN : UMI_IBKR_ACCOUNT_CURRENCY;
  CHECK(UmiIbkrConnectionReadAccountReport(f->c, "DU123", kind,
                                           kind == UMI_IBKR_ACCOUNT_CURRENCY ? "GBP" : "",
                                           10U) == UMI_STATUS_OK);
  CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
  if (!strcmp(name, "margin"))
    CHECK(Contains(f->output, f->outSize, "LookAheadMaintMarginReq"));
  else
    CHECK(Contains(f->output, f->outSize, "$LEDGER:GBP"));
  if (!strcmp(name, "read-once")) {
    CHECK(UmiIbkrConnectionReadAccountReport(f->c, "DU123", kind, "GBP", 12U) ==
          UMI_STATUS_INVALID_STATE);
  } else if (!strcmp(name, "values") || !strcmp(name, "filter") || !strcmp(name, "late") ||
             !strcmp(name, "cancel")) {
    FEED(f, "63", "1", "35001", "DU123", "CashBalance", "1234567890.123456789", "GBP");
    if (!strcmp(name, "filter")) {
      FEED(f, "63", "1", "35001", "DU456", "CashBalance", "999", "GBP");
      FEED(f, "63", "1", "35002", "DU123", "CashBalance", "888", "GBP");
    }
    FEED(f, "64", "1", "35001");
    CHECK(UmiIbkrConnectionPump(f->c, 12U) == UMI_STATUS_OK);
    CHECK(f->c->snapshot.summaryComplete && !f->c->snapshot.positionsComplete);
    CHECK(f->c->snapshot.valueCount == 1U);
    CHECK(!strcmp(f->c->snapshot.values[0].value, "1234567890.123456789"));
    if (!strcmp(name, "late")) {
      FEED(f, "63", "1", "35001", "DU123", "CashBalance", "1", "GBP");
      CHECK(UmiIbkrConnectionPump(f->c, 13U) == UMI_STATUS_OK);
      CHECK(!strcmp(f->c->snapshot.values[0].value, "1234567890.123456789"));
    }
    if (!strcmp(name, "cancel")) {
      size_t prior = f->outSize;
      CHECK(UmiIbkrConnectionPump(f->c, 13U) == UMI_STATUS_OK);
      const unsigned char cancel[] = {0U, 0U,  0U,  11U, '6', '3', 0U, '1',
                                      0U, '3', '5', '0', '0', '1', 0U};
      CHECK(f->outSize - prior == sizeof cancel &&
            !memcmp(f->output + prior, cancel, sizeof cancel));
    }
  } else
    CHECK(!strcmp(name, "margin") || !strcmp(name, "currency"));
  Delete(f);
  return 0;
}
int main(int argc, char **argv) { return argc == 2 ? Run(argv[1]) : 2; }
