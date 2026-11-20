/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/fx_forward_valuation.h
 *
 * PURPOSE:
 *   Capture a reviewed FX-forward valuation with explicit market input versions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OPEN_TMS_FX_FORWARD_VALUATION_H
#define UMICOM_OPEN_TMS_FX_FORWARD_VALUATION_H

#include "umicom/finance/core/types.h"
#include "umicom/open_tms/fx_forward_book.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiOpenTmsFxForwardValuationConvention {
    UMI_OPEN_TMS_FX_FORWARD_LONG_BASE_DISCOUNTED = 1
} UmiOpenTmsFxForwardValuationConvention;

typedef struct UmiOpenTmsFxForwardValuationRequest {
    const char *trade_id;
    uint64_t expected_book_revision;
    uint64_t expected_trade_version;
    UmiFinancialDate as_of;
    const char *forward_quote_id;
    uint64_t forward_quote_revision;
    double observed_forward_rate;
    const char *discount_curve_id;
    uint64_t discount_curve_revision;
    double discount_factor;
} UmiOpenTmsFxForwardValuationRequest;

typedef struct UmiOpenTmsFxForwardValuation {
    UmiOpenTmsFxForward trade;
    uint64_t book_revision;
    UmiFinancialDate as_of;
    UmiFinancialId forward_quote_id;
    uint64_t forward_quote_revision;
    UmiFinancialId discount_curve_id;
    uint64_t discount_curve_revision;
    double discount_factor;
    UmiOpenTmsFxForwardValuationConvention convention;
    double quote_currency_value;
} UmiOpenTmsFxForwardValuation;

/* Review a selected version of a local trade and own the values used to price
 * it. The observed quote rate must equal the trade's current market rate.
 * A changed revision, trade version or market rate returns BUSY. Missing
 * market data returns UNAVAILABLE. Source identifiers and positive revisions
 * are supplied by the market-data owner. The service records those claims
 * and performs no market lookup, posting or settlement. The output is a
 * quote-currency valuation using the long-base convention, with no rounding
 * to ledger units. Failure preserves the output. Serialize with book edits. */
UmiStatus UmiOpenTmsFxForwardValuationCapture(const UmiOpenTmsFxForwardBook *book,
    const UmiOpenTmsFxForwardValuationRequest *request, UmiOpenTmsFxForwardValuation *out);

#ifdef __cplusplus
}
#endif

#endif
