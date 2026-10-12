/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/account_query.h
 * PURPOSE: Select bounded TWS account summaries while retaining the established session owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_ACCOUNT_QUERY_H
#define UMICOM_BROKER_CONNECTIVITY_ACCOUNT_QUERY_H
#include "umicom/broker_connectivity/connection.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiIbkrAccountReportKind {
  UMI_IBKR_ACCOUNT_OVERVIEW = 0,
  UMI_IBKR_ACCOUNT_MARGIN,
  UMI_IBKR_ACCOUNT_CURRENCY
} UmiIbkrAccountReportKind;
/** Prepare documented TWS summary tags. Currency must be three uppercase ASCII
 * letters for CURRENCY, and empty for the other kinds. The reserved ALL
 * aggregate is refused, even though ALL is also a real currency code. It is a request, not
 * proof of provider support. Failure preserves output. The tag list is bounded;
 * arbitrary wire text from widgets or imported files is never accepted. */
UmiStatus UmiIbkrAccountReportTags(UmiIbkrAccountReportKind kind, const char *currency, char *out,
                                   size_t capacity);
/** Request one selected account's report and positions through the existing
 * session owner. OVERVIEW retains the original ReadAccount behavior. MARGIN
 * includes balances, current and look-ahead requirements; CURRENCY requests
 * $LEDGER:CURRENCY cash fields. Completion remains independent from positions.
 * Existing account, time and one-read guards apply. Values remain provider
 * text, not a bank balance or permission to trade. Failure queues neither
 * request. Serialise calls on the connection's owning thread. */
UmiStatus UmiIbkrConnectionReadAccountReport(UmiIbkrConnection *connection, const char *account,
                                             UmiIbkrAccountReportKind kind, const char *currency,
                                             uint64_t nowMilliseconds);
#ifdef __cplusplus
}
#endif
#endif
