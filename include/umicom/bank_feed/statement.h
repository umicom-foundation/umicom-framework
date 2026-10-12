/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/bank_feed/statement.h
 * PURPOSE: Compare a dated account snapshot with explicitly supplied statement balances.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_FEED_STATEMENT_H
#define UMICOM_BANK_FEED_STATEMENT_H
#include "umicom/bank_feed/transactions.h"
#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiBankStatementRequest {
  const char *account;
  const char *currency;
  UmiFinancialDate firstDate, lastDate;
  UmiDecimal openingBalance, closingBalance;
} UmiBankStatementRequest;
typedef struct UmiBankStatementResult {
  size_t bookedRows, pendingRows, outsidePeriodRows;
  size_t mutableBookedRows, unidentifiedRows;
  bool hasPreviousPage, hasNextPage, balancesMatch;
  UmiDecimal credits, debits, netMovement, expectedClosing, difference;
} UmiBankStatementResult;
/** Parse exactly YYYY-MM-DD as a valid Gregorian date, years 1600..9999.
 * Failure preserves out. No locale, timezone conversion or clock is used. */
UmiStatus UmiBankStatementDate(const char *text, UmiFinancialDate *out);
/** Copy an immutable imported feed into a separately owned snapshot.
 * The original can be released immediately. Failure clears *out. */
UmiStatus UmiBankFeedClone(const UmiBankFeed *feed, UmiBankFeed **out);
/** Compare one account and currency across an inclusive booking-date period.
 * Dates use the provider's written calendar date, not UTC conversion. Opening
 * means immediately before firstDate; closing means after lastDate. Only booked
 * entries affect opening + credits - debits. Pending entries are counted only.
 * difference is supplied closing minus expected closing. A zero difference is
 * an arithmetic match, never proof of completeness, authenticity or settlement.
 * Page flags apply to the entire import. unidentifiedRows counts selected booked
 * and pending entries; mutableBookedRows counts booked entries only.
 * Account/currency with no imported observations returns NOT_FOUND, whereas a
 * known account with no entries in the period returns zero movement. Overflow
 * and invalid input preserve out. No file write, network call or posting occurs. */
UmiStatus UmiBankStatementReview(const UmiBankFeed *feed, const UmiBankStatementRequest *request,
                                 UmiBankStatementResult *out);
/** Export a fresh review and the selected period's booked and pending entries.
 * Each row repeats scope and page limitations. Formula-like text is escaped by
 * the common CSV writer. The owned document contains private account data; the
 * caller chooses where to save it. Failure clears *out and never returns a
 * partial report. Does not accept a caller-supplied result that could be stale. */
UmiStatus UmiBankStatementExportCsv(const UmiBankFeed *feed, const UmiBankStatementRequest *request,
                                    UmiCsvDocument **out);
#ifdef __cplusplus
}
#endif
#endif
