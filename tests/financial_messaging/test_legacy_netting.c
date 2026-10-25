/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_messaging/test_legacy_netting.c
 *
 * PURPOSE:
 *   Refuse invalid signed gross amounts before legacy netting arithmetic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/finance/treasury/settlement_netting.h"
int main(int argc, char **argv) {
  static const char *const cases[] = {
      "normal",           "pay-max",      "receive-max", "negative-pay",
      "negative-receive", "malformed-id", "null",        "zero"};
  int selected = Case(argc, argv, cases, sizeof cases / sizeof cases[0]);
  UmiTreasurySettlementNetting value = {0};
  strcpy(value.id, "scope.1");
  value.pay_minor = 100;
  value.receive_minor = 150;
  if (selected == 1) {
    value.pay_minor = INT64_MAX;
    value.receive_minor = 0;
  }
  if (selected == 2) {
    value.pay_minor = 0;
    value.receive_minor = INT64_MAX;
  }
  if (selected == 3) {
    value.pay_minor = INT64_MIN;
    value.receive_minor = INT64_MAX;
  }
  if (selected == 4) {
    value.pay_minor = INT64_MAX;
    value.receive_minor = INT64_MIN;
  }
  if (selected == 5)
    memset(value.id, 'x', sizeof value.id);
  if (selected == 6) {
    int64_t unchanged = 321;
    CHECK(UmiTreasurySettlementNettingNetChecked(NULL, &unchanged) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiTreasurySettlementNettingNetChecked(&value, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(unchanged == 321 && umi_treasury_settlement_netting_net_minor(NULL) == 0);
    return 0;
  }
  if (selected == 7)
    value.pay_minor = value.receive_minor = 0;
  int64_t result = 321;
  if (selected >= 3 && selected <= 5) {
    CHECK(UmiTreasurySettlementNettingNetChecked(&value, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(result == 321 && umi_treasury_settlement_netting_net_minor(&value) == 0);
  } else {
    OK(UmiTreasurySettlementNettingNetChecked(&value, &result));
    CHECK(result == value.receive_minor - value.pay_minor);
    CHECK(umi_treasury_settlement_netting_net_minor(&value) == result);
  }
  return 0;
}
