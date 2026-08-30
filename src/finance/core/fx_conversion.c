/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/fx_conversion.c
 *
 * PURPOSE:
 *   Implement deterministic FX money conversion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/fx_conversion.h"

#include <string.h>

/* Convert fixed minor units into a real amount using the stored decimal scale. */
static double umi_fx_conversion_real_amount(UmiMoney money)
{
    double divisor = 1.0;
    uint8_t index;
    for (index = 0U; index < money.scale; ++index) {
        divisor *= 10.0;
    }
    return (double)money.minor_units / divisor;
}

/* Convert a real amount into deterministic rounded minor units without libm. */
static int64_t umi_fx_conversion_minor_units(double amount, uint8_t scale)
{
    double multiplier = 1.0;
    double scaled;
    uint8_t index;
    for (index = 0U; index < scale; ++index) {
        multiplier *= 10.0;
    }
    scaled = amount * multiplier;
    return (int64_t)(scaled >= 0.0 ? scaled + 0.5 : scaled - 0.5);
}

/* Convert base money to quote money through an explicitly ordered FX rate. */
UmiStatus umi_fx_conversion_base_to_quote(UmiMoney input,
                                          const UmiExchangeRate *rate,
                                          uint8_t output_scale,
                                          UmiMoney *out_money)
{
    if (out_money == NULL || !umi_exchange_rate_is_valid(rate) ||
        strcmp(input.currency.code, rate->pair.base.code) != 0 ||
        output_scale > 9U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    out_money->minor_units = umi_fx_conversion_minor_units(
        umi_fx_conversion_real_amount(input) * rate->rate,
        output_scale
    );
    out_money->scale = output_scale;
    out_money->currency = rate->pair.quote;
    return UMI_STATUS_OK;
}

/* Convert quote money back to base money through an explicitly ordered FX rate. */
UmiStatus umi_fx_conversion_quote_to_base(UmiMoney input,
                                          const UmiExchangeRate *rate,
                                          uint8_t output_scale,
                                          UmiMoney *out_money)
{
    if (out_money == NULL || !umi_exchange_rate_is_valid(rate) ||
        strcmp(input.currency.code, rate->pair.quote.code) != 0 ||
        output_scale > 9U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    out_money->minor_units = umi_fx_conversion_minor_units(
        umi_fx_conversion_real_amount(input) / rate->rate,
        output_scale
    );
    out_money->scale = output_scale;
    out_money->currency = rate->pair.base;
    return UMI_STATUS_OK;
}
