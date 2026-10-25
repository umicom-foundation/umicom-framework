/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_messaging/clearing_example.c
 *
 * PURPOSE:
 *   Expose one shared exact-money clearing example to financial application consoles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/financial_messaging/clearing_preview.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
UmiStatus UmiUfmpClearingExampleFormat(char *output, size_t capacity, size_t *required) {
  if (required == NULL || (output == NULL && capacity != 0U))
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiUfmpClearingScope scope = {0};
  strcpy(scope.owner, "synthetic.Bank.B");
  strcpy(scope.counterparty, "synthetic.Bank.A");
  strcpy(scope.settlement_account, "synthetic.GBP.account");
  strcpy(scope.scheme, "synthetic.bilateral");
  scope.value_day = 20000;
  memcpy(scope.currency.code, "GBP", 4U);
  scope.scale = 2U;
  UmiUfmpClearingObligation rows[3] = {0};
  const int64_t amounts[] = {10000, 2500, 4000};
  for (size_t i = 0U; i < 3U; ++i) {
    rows[i].scope = scope;
    rows[i].direction = i == 2U ? UMI_UFMP_CLEARING_RECEIVE : UMI_UFMP_CLEARING_PAY;
    rows[i].amount = (UmiMoney){amounts[i], 2U, {{'G', 'B', 'P', '\0'}}};
    (void)snprintf(rows[i].id, sizeof rows[i].id, "obligation.%zu", i + 1U);
  }
  UmiUfmpClearingPreview result;
  UmiStatus status = UmiUfmpClearingPreviewBuild(&scope, rows, 3U, &result);
  if (status != UMI_STATUS_OK)
    return status;
  char text[1024];
  int length =
      snprintf(text, sizeof text,
               "SYNTHETIC CLEARING PREVIEW ONLY\n"
               "Scope: Bank.B / Bank.A / GBP / synthetic account and scheme\n"
               "Scale: 2 (all amounts below are integer minor units)\n"
               "Obligations: %zu\nGross pay: %" PRId64 "\nGross receive: %" PRId64
               "\nNet receive: %" PRId64 "\n"
               "A negative net means a proposed pay position.\n"
               "No FX conversion, legal netting approval, payment or settlement occurred.\n",
               result.obligation_count, result.gross_pay.minor_units,
               result.gross_receive.minor_units, result.net_receive.minor_units);
  if (length < 0 || (size_t)length >= sizeof text)
    return UMI_STATUS_INTERNAL_ERROR;
  *required = (size_t)length;
  if (output == NULL)
    return UMI_STATUS_OK;
  if (capacity <= (size_t)length)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  memcpy(output, text, (size_t)length + 1U);
  return UMI_STATUS_OK;
}
