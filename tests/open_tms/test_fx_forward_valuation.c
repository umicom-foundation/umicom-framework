/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_fx_forward_valuation.c
 *
 * PURPOSE:
 *   Exercise owned forward valuation, quote evidence and stale-input refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/open_tms/fx_forward_valuation.h"

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        exit(1); \
    } \
} while (0)

static void Refuse(const UmiOpenTmsFxForwardBook *book, const UmiOpenTmsFxForwardValuationRequest *request,
    UmiStatus expected)
{
    UmiOpenTmsFxForwardValuation unchanged;
    unsigned char before[sizeof unchanged];
    memset(&unchanged, 0x5a, sizeof unchanged);
    memcpy(before, &unchanged, sizeof before);
    CHECK(UmiOpenTmsFxForwardValuationCapture(book, request, &unchanged) == expected);
    CHECK(memcmp(before, &unchanged, sizeof before) == 0);
}

int main(void)
{
    UmiOpenTmsFxForwardBook *book = malloc(sizeof *book);
    CHECK(book != NULL);
    umi_open_tms_fx_forward_book_init(book);
    const UmiOpenTmsFxForward trade = {
        .tradeId = "synthetic", .accountId = "practice", .counterpartyId = "fictional",
        .baseCurrency = "GBP", .quoteCurrency = "USD",
        .baseNotional = 1000.0, .forwardRate = 1.25, .marketForwardRate = 1.375,
        .state = UMI_OPEN_TMS_FX_FORWARD_CONFIRMED, .version = 3U
    };
    CHECK(umi_open_tms_fx_forward_book_add(book, &trade) == UMI_STATUS_OK);
    char quote_id[] = "forward-close";
    char curve_id[] = "usd-curve";
    const UmiOpenTmsFxForwardValuationRequest request = {
        .trade_id = trade.tradeId, .expected_book_revision = book->revision,
        .expected_trade_version = trade.version, .as_of = {2026, 11, 19},
        .forward_quote_id = quote_id, .forward_quote_revision = 7U, .observed_forward_rate = 1.375,
        .discount_curve_id = curve_id, .discount_curve_revision = 11U, .discount_factor = 0.5
    };
    UmiOpenTmsFxForwardValuation valuation;
    CHECK(UmiOpenTmsFxForwardValuationCapture(book, &request, &valuation) == UMI_STATUS_OK);
    CHECK(valuation.quote_currency_value == 62.5 && valuation.discount_factor == 0.5);
    CHECK(strcmp(valuation.trade.quoteCurrency, "USD") == 0);
    CHECK(valuation.book_revision == book->revision && valuation.trade.version == trade.version);
    CHECK(valuation.as_of.year == 2026 && valuation.as_of.month == 11U && valuation.as_of.day == 19U);
    CHECK(valuation.convention == UMI_OPEN_TMS_FX_FORWARD_LONG_BASE_DISCOUNTED);
    CHECK(valuation.forward_quote_revision == 7U && valuation.discount_curve_revision == 11U);
    quote_id[0] = 'F';
    curve_id[0] = 'U';
    CHECK(strcmp(valuation.forward_quote_id.value, "forward-close") == 0);
    CHECK(strcmp(valuation.discount_curve_id.value, "usd-curve") == 0);
    CHECK(valuation.trade.marketForwardRate == 1.375 && book->trades[0].state == UMI_OPEN_TMS_FX_FORWARD_CONFIRMED);

    UmiOpenTmsFxForwardValuationRequest bad = request;
    bad.expected_book_revision += 1U;
    Refuse(book, &bad, UMI_STATUS_BUSY);
    bad = request; bad.expected_trade_version += 1U;
    Refuse(book, &bad, UMI_STATUS_BUSY);
    bad = request; bad.observed_forward_rate = 1.5;
    Refuse(book, &bad, UMI_STATUS_BUSY);
    bad = request; bad.trade_id = "missing";
    Refuse(book, &bad, UMI_STATUS_NOT_FOUND);
    bad = request; bad.forward_quote_id = "";
    Refuse(book, &bad, UMI_STATUS_INVALID_ARGUMENT);
    bad = request; bad.forward_quote_revision = 0U;
    Refuse(book, &bad, UMI_STATUS_INVALID_ARGUMENT);
    bad = request; bad.discount_curve_revision = 0U;
    Refuse(book, &bad, UMI_STATUS_INVALID_ARGUMENT);
    bad = request; bad.discount_factor = NAN;
    Refuse(book, &bad, UMI_STATUS_INVALID_ARGUMENT);
    bad = request; bad.as_of = (UmiFinancialDate){2026, 2, 30};
    Refuse(book, &bad, UMI_STATUS_INVALID_ARGUMENT);
    book->trades[0].marketForwardRate = 0.0;
    Refuse(book, &request, UMI_STATUS_UNAVAILABLE);
    CHECK(valuation.trade.marketForwardRate == 1.375);
    book->trades[0].marketForwardRate = 3.0;
    book->trades[0].forwardRate = 1.0;
    book->trades[0].baseNotional = DBL_MAX;
    bad = request; bad.observed_forward_rate = 3.0; bad.discount_factor = 1.0;
    Refuse(book, &bad, UMI_STATUS_CAPACITY_EXCEEDED);
    book->count = UMI_OPEN_TMS_FX_FORWARD_CAPACITY + 1U;
    Refuse(book, &request, UMI_STATUS_INVALID_STATE);
    Refuse(NULL, &request, UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiOpenTmsFxForwardValuationCapture(book, &request, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    free(book);
    return 0;
}
