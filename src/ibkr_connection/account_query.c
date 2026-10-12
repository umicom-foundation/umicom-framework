/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/account_query.c
 * PURPOSE: Expand account inspection without a second socket or unbounded request interface.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/account_query.h"
#include "internal.h"
#include <stdio.h>
#include <string.h>
/* Keep provider tag spelling in one reusable owner. UI labels and imported
 * text cannot inject additional fields into the socket protocol. */
UmiStatus UmiIbkrAccountReportTags(UmiIbkrAccountReportKind kind, const char *currency, char *out,
                                   size_t capacity) {
  if (currency == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  const char *tags = NULL;
  char ledger[16];
  if (kind == UMI_IBKR_ACCOUNT_CURRENCY) {
    if (!UmiIbkrText(currency, 4U, false) || strlen(currency) != 3U || strcmp(currency, "ALL") == 0)
      return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0; i < 3U; ++i)
      if (currency[i] < 'A' || currency[i] > 'Z')
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)snprintf(ledger, sizeof ledger, "$LEDGER:%s", currency);
    tags = ledger;
  } else {
    if (currency[0] != '\0')
      return UMI_STATUS_INVALID_ARGUMENT;
    switch (kind) {
    case UMI_IBKR_ACCOUNT_OVERVIEW:
      tags = "NetLiquidation,TotalCashValue,BuyingPower,AvailableFunds";
      break;
    case UMI_IBKR_ACCOUNT_MARGIN:
      tags = "AccountType,NetLiquidation,TotalCashValue,SettledCash,AccruedCash,"
             "BuyingPower,EquityWithLoanValue,GrossPositionValue,InitMarginReq,"
             "MaintMarginReq,AvailableFunds,ExcessLiquidity,Cushion,FullInitMarginReq,"
             "FullMaintMarginReq,FullAvailableFunds,FullExcessLiquidity,LookAheadNextChange,"
             "LookAheadInitMarginReq,LookAheadMaintMarginReq,LookAheadAvailableFunds,"
             "LookAheadExcessLiquidity,HighestSeverity,DayTradesRemaining,Leverage";
      break;
    default:
      return UMI_STATUS_INVALID_ARGUMENT;
    }
  }
  size_t bytes = strlen(tags) + 1U;
  if (bytes > capacity)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  memmove(out, tags, bytes);
  return UMI_STATUS_OK;
}
UmiStatus UmiIbkrConnectionReadAccountReport(UmiIbkrConnection *c, const char *account,
                                             UmiIbkrAccountReportKind kind, const char *currency,
                                             uint64_t now) {
  char tags[768];
  UmiStatus status = UmiIbkrAccountReportTags(kind, currency, tags, sizeof tags);
  if (status != UMI_STATUS_OK)
    return status;
  if (kind == UMI_IBKR_ACCOUNT_OVERVIEW)
    return UmiIbkrConnectionReadAccount(c, account, now);
  if (!c || !UmiIbkrText(account, 64U, false) || now < c->lastNow)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (c->snapshot.state != UMI_IBKR_READY || c->snapshot.requestIssued)
    return UMI_STATUS_INVALID_STATE;
  bool found = false;
  for (size_t i = 0; i < c->snapshot.accountCount; ++i)
    if (strcmp(account, c->snapshot.accounts[i]) == 0)
      found = true;
  if (!found)
    return UMI_STATUS_PERMISSION_DENIED;
  /* Use existing request IDs and decoder: it discards other accounts and
   * cancels each response at its own end marker. Roll back the queue if the
   * second frame cannot fit, before publishing selected-account identity. */
  const char *summary[] = {"62", "1", "35001", "All", tags}, *positions[] = {"61", "1"};
  size_t previous = c->txSize;
  status = UmiIbkrQueueFields(c, summary, 5U);
  if (status == UMI_STATUS_OK)
    status = UmiIbkrQueueFields(c, positions, 2U);
  if (status != UMI_STATUS_OK) {
    c->txSize = previous;
    return status;
  }
  (void)snprintf(c->snapshot.selectedAccount, sizeof c->snapshot.selectedAccount, "%s", account);
  c->snapshot.requestIssued = true;
  c->snapshot.requestedAtMilliseconds = now;
  c->lastNow = now;
  return UMI_STATUS_OK;
}
