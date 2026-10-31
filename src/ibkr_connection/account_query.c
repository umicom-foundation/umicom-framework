/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/account_query.c
 *
 * PURPOSE:
 *   Own bounded report choices and repeat account captures without reusing request identities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/account_query.h"
#include "internal.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>
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
  UmiIbkrExecutionDisable(c);
  (void)snprintf(c->snapshot.selectedAccount, sizeof c->snapshot.selectedAccount, "%s", account);
  c->snapshot.requestIssued = true;
  c->snapshot.requestedAtMilliseconds = now;
  c->lastNow = now;
  return UMI_STATUS_OK;
}

UmiStatus UmiIbkrConnectionRefreshAccountReport(UmiIbkrConnection *c,
    const char *account, UmiIbkrAccountReportKind kind, const char *currency,
    uint64_t now)
{
    char tags[768];
    UmiStatus status = UmiIbkrAccountReportTags(kind, currency, tags, sizeof tags);
    if (status != UMI_STATUS_OK) return status;
    if (c == NULL || !UmiIbkrText(account, 64U, false) || now < c->lastNow)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (c->snapshot.state != UMI_IBKR_READY || c->snapshot.stale)
        return UMI_STATUS_INVALID_STATE;
    if (c->snapshot.requestIssued &&
        (!c->snapshot.summaryComplete || !c->snapshot.positionsComplete))
        return UMI_STATUS_BUSY;
    bool found = false;
    for (size_t i = 0; i < c->snapshot.accountCount; ++i)
        if (strcmp(account, c->snapshot.accounts[i]) == 0) found = true;
    if (!found) return UMI_STATUS_PERMISSION_DENIED;
    if (c->snapshot.protocolVersion < 151 || c->snapshot.protocolVersion > 176)
        return UMI_STATUS_UNAVAILABLE;
    /* Zero is the established unallocated state shared by quote and report
     * owners. Reserve IDs only after both frames fit in the outbound queue. */
    const uint32_t sequence = c->nextQuoteRequest ? c->nextQuoteRequest : 36000U;
    if (sequence < 36000U || sequence > (uint32_t)INT_MAX - 2U)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /* Copy caller text before clearing the snapshot: an owner may pass its
     * currently selected account directly from that copied observation. */
    char selected[64], summaryId[24], positionId[24];
    (void)snprintf(selected, sizeof selected, "%s", account);
    (void)snprintf(summaryId, sizeof summaryId, "%u", (unsigned)sequence);
    (void)snprintf(positionId, sizeof positionId, "%u", (unsigned)(sequence + 1U));
    const char *summary[] = {"62", "1", summaryId, "All", tags};
    const char *positions[] = {"74", "1", positionId, selected, ""};
    const size_t before = c->txSize;
    status = UmiIbkrQueueFields(c, summary, 5U);
    if (status == UMI_STATUS_OK) status = UmiIbkrQueueFields(c, positions, 5U);
    if (status != UMI_STATUS_OK) { c->txSize = before; return status; }

    /* The shared Framework owner commits request identity and observations
     * together. Both subscriptions are cancelled at their own end markers.
     * Applications must never infer completeness from an empty row list. */
    UmiIbkrExecutionDisable(c);
    c->accountSummaryRequest = sequence;
    c->accountPositionsRequest = sequence + 1U;
    c->nextQuoteRequest = sequence + 2U;
    c->snapshot.valueCount = 0U;
    c->snapshot.positionCount = 0U;
    memset(c->snapshot.values, 0, sizeof c->snapshot.values);
    memset(c->snapshot.positions, 0, sizeof c->snapshot.positions);
    memcpy(c->snapshot.selectedAccount, selected, strlen(selected) + 1U);
    c->snapshot.summaryComplete = false;
    c->snapshot.positionsComplete = false;
    c->snapshot.summaryAtMilliseconds = 0U;
    c->snapshot.positionsAtMilliseconds = 0U;
    c->snapshot.requestIssued = true;
    c->snapshot.requestedAtMilliseconds = now;
    c->lastNow = now;
    return UMI_STATUS_OK;
}
