/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/fx_forward_valuation.c
 *
 * PURPOSE:
 *   Bind a discounted forward valuation to the trade and market inputs used.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/open_tms/fx_forward_valuation.h"

#include "umicom/finance/quant/fx_forward_pricer.h"
#include <math.h>

UmiStatus UmiOpenTmsFxForwardValuationCapture(const UmiOpenTmsFxForwardBook *book,
    const UmiOpenTmsFxForwardValuationRequest *request, UmiOpenTmsFxForwardValuation *out)
{
    if (book == NULL || request == NULL || out == NULL || request->trade_id == NULL ||
        request->trade_id[0] == '\0' || request->forward_quote_revision == 0U ||
        request->discount_curve_revision == 0U || !umi_financial_date_is_valid(request->as_of) ||
        !isfinite(request->observed_forward_rate) || request->observed_forward_rate <= 0.0 ||
        !isfinite(request->discount_factor) || request->discount_factor < 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    const UmiOpenTmsFxForward *trade = umi_open_tms_fx_forward_book_find(book, request->trade_id);
    if (trade == NULL) {
        const UmiStatus book_status = umi_open_tms_fx_forward_book_validate(book);
        return book_status == UMI_STATUS_OK ? UMI_STATUS_NOT_FOUND : book_status;
    }
    if (book->revision != request->expected_book_revision || trade->version != request->expected_trade_version) {
        return UMI_STATUS_BUSY;
    }
    if (trade->marketForwardRate == 0.0) {
        return UMI_STATUS_UNAVAILABLE;
    }
    if (trade->marketForwardRate != request->observed_forward_rate) {
        return UMI_STATUS_BUSY;
    }

    /* Stage provenance, quote-currency units and the exact trade observation
     * together. A later journal review can consume an owned valuation without
     * reading newer inputs or recalculating the price at a different rate. */
    UmiOpenTmsFxForwardValuation candidate = {
        .trade = *trade, .book_revision = book->revision, .as_of = request->as_of,
        .forward_quote_revision = request->forward_quote_revision,
        .discount_curve_revision = request->discount_curve_revision, .discount_factor = request->discount_factor,
        .convention = UMI_OPEN_TMS_FX_FORWARD_LONG_BASE_DISCOUNTED
    };
    UmiStatus status = umi_financial_id_assign(&candidate.forward_quote_id, request->forward_quote_id);
    if (status == UMI_STATUS_OK) {
        status = umi_financial_id_assign(&candidate.discount_curve_id, request->discount_curve_id);
    }
    if (status == UMI_STATUS_OK && (!umi_financial_id_is_valid(&candidate.forward_quote_id) ||
        !umi_financial_id_is_valid(&candidate.discount_curve_id))) {
        status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (status == UMI_STATUS_OK) {
        status = umi_quant_fx_forward_pricer_value(trade->baseNotional, trade->forwardRate,
            trade->marketForwardRate, request->discount_factor, &candidate.quote_currency_value);
    }
    if (status == UMI_STATUS_OK) {
        *out = candidate;
    }
    return status;
}
