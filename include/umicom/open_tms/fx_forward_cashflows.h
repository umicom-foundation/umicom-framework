/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/fx_forward_cashflows.h
 *
 * PURPOSE:
 *   Capture both contractual legs of a reviewed long-base FX forward.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OPEN_TMS_FX_FORWARD_CASHFLOWS_H
#define UMICOM_OPEN_TMS_FX_FORWARD_CASHFLOWS_H

#include "umicom/finance/core/cashflow.h"
#include "umicom/open_tms/fx_forward_book.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiOpenTmsFxForwardCashflowRequest {
    const char *tradeId;
    uint64_t bookRevision;
    uint64_t tradeVersion;
    const char *confirmationId;
    uint64_t confirmationRevision;
    UmiFinancialDate paymentDate;
    UmiMoney baseNotional;
    UmiDecimal contractRate;
    uint8_t quoteScale;
    const char *receiveId;
    const char *payId;
} UmiOpenTmsFxForwardCashflowRequest;

typedef struct UmiOpenTmsFxForwardCashflows {
    UmiOpenTmsFxForward trade;
    uint64_t bookRevision;
    UmiFinancialId confirmationId;
    uint64_t confirmationRevision;
    UmiDecimal contractRate;
    UmiCashflow receive;
    UmiCashflow pay;
} UmiOpenTmsFxForwardCashflows;

/* Use exact base notional, rate and payment date from the caller's reviewed
 * confirmation. Preserve that date explicitly. The trade's legacy maturity
 * timestamp is retained for provenance. A forecast requires a confirmed
 * or settlement-pending trade. Current trade/book versions and the legacy
 * binary projections of the exact amounts must agree with the request. That
 * comparison can identify disagreement with the legacy book but cannot
 * recover original precision or authenticate the supplied confirmation.
 *
 * Receive the base currency and pay base times contractual rate in the quote
 * currency. Both cashflow IDs must be distinct. A product that cannot fit
 * the exact quote scale/capacity is refused before either leg is published.
 * Market rate, discounting and provider authorization are separate. No
 * trade, account, ledger or external payment is changed. Failure preserves
 * *out. Serialize with book writes. */
UmiStatus UmiOpenTmsFxForwardCashflowsCapture(const UmiOpenTmsFxForwardBook *book,
    const UmiOpenTmsFxForwardCashflowRequest *request, UmiOpenTmsFxForwardCashflows *out);

/* Format one fictional confirmed position through the same forecast service.
 * Use complete output or report capacity. Failure preserves the buffer. */
UmiStatus UmiOpenTmsFxForwardCashflowsExample(char *text, size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
