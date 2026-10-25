/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_messaging/test_clearing.c
 *
 * PURPOSE:
 *   Test exact-money previews across scope, duplicate and overflow boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/financial_messaging/clearing_preview.h"
static UmiUfmpClearingScope Scope(void) {
  UmiUfmpClearingScope s = {0};
  strcpy(s.owner, "Bank.B");
  strcpy(s.counterparty, "Bank.A");
  strcpy(s.settlement_account, "account.GBP");
  strcpy(s.scheme, "scheme.1");
  s.value_day = 20000;
  s.scale = 2U;
  memcpy(s.currency.code, "GBP", 4U);
  return s;
}
int main(int argc, char **argv) {
  static const char *const cases[] = {
      "balanced", "pay",      "receive",  "empty",     "duplicate",    "currency",       "scale",
      "account",  "scheme",   "day",      "owner",     "counterparty", "negative",       "zero",
      "overflow", "extremes", "capacity", "malformed", "format",       "amount-currency"};
  int selected = Case(argc, argv, cases, sizeof cases / sizeof cases[0]);
  UmiUfmpClearingScope scope = Scope();
  UmiUfmpClearingObligation rows[2] = {0};
  for (size_t i = 0U; i < 2U; ++i) {
    rows[i].scope = scope;
    rows[i].amount = (UmiMoney){10000, 2U, {{'G', 'B', 'P', '\0'}}};
    rows[i].direction = i == 0U ? UMI_UFMP_CLEARING_PAY : UMI_UFMP_CLEARING_RECEIVE;
    strcpy(rows[i].id, i == 0U ? "first" : "second");
  }
  UmiUfmpClearingPreview result;
  memset(&result, 0xA5, sizeof result);
  UmiUfmpClearingPreview before = result;
  UmiStatus expected = UMI_STATUS_OK;
  size_t count = 2U;
  switch (selected) {
  case 1:
    rows[1].amount.minor_units = 5000;
    break;
  case 2:
    rows[1].amount.minor_units = 15000;
    break;
  case 3:
    count = 0U;
    break;
  case 4:
    strcpy(rows[1].id, "first");
    expected = UMI_STATUS_ALREADY_EXISTS;
    break;
  case 5:
    memcpy(rows[1].scope.currency.code, "USD", 4U);
    expected = UMI_STATUS_INVALID_ARGUMENT;
    break;
  case 6:
    rows[1].amount.scale = 3U;
    expected = UMI_STATUS_INVALID_ARGUMENT;
    break;
  case 7:
    strcpy(rows[1].scope.settlement_account, "other");
    expected = UMI_STATUS_INVALID_ARGUMENT;
    break;
  case 8:
    strcpy(rows[1].scope.scheme, "other");
    expected = UMI_STATUS_INVALID_ARGUMENT;
    break;
  case 9:
    rows[1].scope.value_day++;
    expected = UMI_STATUS_INVALID_ARGUMENT;
    break;
  case 10:
    strcpy(rows[1].scope.owner, "Bank.C");
    expected = UMI_STATUS_INVALID_ARGUMENT;
    break;
  case 11:
    strcpy(rows[1].scope.counterparty, "Bank.C");
    expected = UMI_STATUS_INVALID_ARGUMENT;
    break;
  case 12:
    rows[1].amount.minor_units = INT64_MIN;
    expected = UMI_STATUS_INVALID_ARGUMENT;
    break;
  case 13:
    rows[1].amount.minor_units = 0;
    expected = UMI_STATUS_INVALID_ARGUMENT;
    break;
  case 14:
    rows[0].amount.minor_units = INT64_MAX;
    rows[1].direction = UMI_UFMP_CLEARING_PAY;
    expected = UMI_STATUS_CAPACITY_EXCEEDED;
    break;
  case 15:
    rows[0].amount.minor_units = INT64_MAX;
    rows[1].amount.minor_units = INT64_MAX;
    break;
  case 16:
    count = UMI_UFMP_CLEARING_MAX_OBLIGATIONS + 1U;
    expected = UMI_STATUS_CAPACITY_EXCEEDED;
    break;
  case 17:
    memset(rows[1].id, 'x', sizeof rows[1].id);
    expected = UMI_STATUS_INVALID_ARGUMENT;
    break;
  case 18: {
    char text[1024], short_text[8] = "stable";
    size_t required = 0U;
    OK(UmiUfmpClearingExampleFormat(NULL, 0U, &required));
    CHECK(required > 8U);
    CHECK(UmiUfmpClearingExampleFormat(short_text, sizeof short_text, &required) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(strcmp(short_text, "stable") == 0);
    OK(UmiUfmpClearingExampleFormat(text, sizeof text, &required));
    CHECK(strstr(text, "Net receive: -8500") != NULL);
    return 0;
  }
  case 19:
    memcpy(rows[1].amount.currency.code, "USD", 4U);
    expected = UMI_STATUS_INVALID_ARGUMENT;
    break;
  default:
    break;
  }
  CHECK(UmiUfmpClearingPreviewBuild(&scope, count == 0U ? NULL : rows, count, &result) == expected);
  if (expected != UMI_STATUS_OK)
    CHECK(memcmp(&result, &before, sizeof result) == 0);
  else {
    CHECK(result.obligation_count == count);
    CHECK(result.net_receive.minor_units == (selected == 1 ? -5000 : selected == 2 ? 5000 : 0));
    CHECK(strcmp(result.net_receive.currency.code, "GBP") == 0 && result.net_receive.scale == 2U);
  }
  return 0;
}
