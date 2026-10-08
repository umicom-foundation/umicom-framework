/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_price_validation_boundaries.c
 * PURPOSE: Reject invalid caller-owned trading records before arithmetic or order routing.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/trading/core/order_validation.h"
#include "umicom/trading/core/volatility_interrupt.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                       \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
static int Order(const char *mode)
{
    UmiTradingOrderIdentity identity;
    UmiInstrument instrument = {0};
    UmiTradingOrderInstruction instruction;
    UmiTradingTickSizeRule tick;
    UmiTradingLotSizeRule lot;
    UmiTradingPriceBand band;
    CHECK(umi_trading_order_identity_init(&identity, "example-order") == UMI_STATUS_OK);
    CHECK(umi_trading_core_id_assign(&instrument.instrument_id, "example-instrument") == UMI_STATUS_OK);
    CHECK(umi_trading_order_instruction_init(&instruction, &identity, &instrument, UMI_SIDE_BUY,
                                             UMI_ORDER_STOP_LIMIT, UMI_TIF_DAY, 10, 1000,
                                             1010) == UMI_STATUS_OK);
    CHECK(umi_trading_tick_size_rule_init(&tick, 100, 2000, 5) == UMI_STATUS_OK);
    CHECK(umi_trading_lot_size_rule_init(&lot, 1, 1, 100) == UMI_STATUS_OK);
    CHECK(umi_trading_price_band_init(&band, 1000, 1000U, 1000U) == UMI_STATUS_OK);
    const char *reason = NULL;
    if (!strcmp(mode, "valid-stop"))
        reason = "accepted";
    else if (!strcmp(mode, "stop-tick"))
    {
        instruction.stop_ticks = 1011;
        reason = "invalid-stop-tick";
    }
    else if (!strcmp(mode, "stop-range"))
    {
        instruction.stop_ticks = 2005;
        reason = "stop-outside-tick-range";
    }
    else if (!strcmp(mode, "limit-range"))
    {
        instruction.limit_ticks = 95;
        reason = "outside-tick-range";
    }
    else if (!strcmp(mode, "stop-band"))
    {
        instruction.stop_ticks = 1200;
        reason = "stop-outside-band";
    }
    else if (!strcmp(mode, "unknown-type"))
    {
        instruction.order_type = (UmiOrderType)99;
        reason = "invalid-input";
    }
    else if (!strcmp(mode, "unknown-tif"))
    {
        instruction.tif = (UmiTimeInForce)99;
        reason = "invalid-input";
    }
    else if (!strcmp(mode, "negative-price"))
    {
        instruction.limit_ticks = -1;
        reason = "invalid-input";
    }
    else if (!strcmp(mode, "unterminated-id"))
    {
        memset(instruction.identity.client_order_id.value, 'x',
               sizeof instruction.identity.client_order_id.value);
        reason = "invalid-input";
    }
    else if (!strcmp(mode, "invalid-band"))
    {
        band.reference_price = INT64_MIN;
        reason = "invalid-input";
    }
    else if (!strcmp(mode, "market"))
    {
        instruction.order_type = UMI_ORDER_MARKET;
        instruction.limit_ticks = 0;
        instruction.stop_ticks = 0;
        reason = "accepted";
    }
    else
        return 2;
    UmiTradingCoreDecision result = umi_trading_order_validation_check(&instruction, &tick, &lot, &band);
    CHECK(strcmp(result.reason, reason) == 0);
    CHECK(result.allowed == (strcmp(reason, "accepted") == 0));
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    if (!strcmp(mode, "zero-tick"))
    {
        UmiTradingTickSizeRule rule = {0};
        CHECK(!umi_trading_tick_size_rule_aligned(&rule));
        rule.minimum_price = INT64_MIN;
        rule.maximum_price = INT64_MAX;
        rule.tick_size = -1;
        CHECK(!umi_trading_tick_size_rule_aligned(&rule));
        CHECK(!umi_trading_tick_size_rule_aligned(NULL));
        return 0;
    }
    if (!strcmp(mode, "band-record"))
    {
        UmiTradingPriceBand band = {0};
        band.reference_price = INT64_MIN;
        band.lower_bps = 1U;
        band.upper_bps = 1U;
        CHECK(!umi_trading_price_band_contains(&band, INT64_MAX));
        band.reference_price = INT64_MAX;
        band.lower_bps = 10001U;
        CHECK(!umi_trading_price_band_contains(&band, 0));
        CHECK(umi_trading_price_band_init(&band, INT64_MAX, 10000U, 10000U) == UMI_STATUS_OK);
        CHECK(umi_trading_price_band_contains(&band, 0));
        CHECK(umi_trading_price_band_contains(&band, INT64_MAX));
        return 0;
    }
    if (!strcmp(mode, "volatility-record"))
    {
        UmiTradingVolatilityInterrupt interrupt = {0};
        interrupt.reference_ticks = INT64_MIN;
        interrupt.trigger_bps = 1U;
        interrupt.auction_required = true;
        CHECK(!umi_trading_volatility_interrupt_evaluate(&interrupt, INT64_MAX));
        CHECK(interrupt.auction_required);
        CHECK(umi_trading_volatility_interrupt_init(&interrupt, INT64_MAX, 10000U) == UMI_STATUS_OK);
        CHECK(!umi_trading_volatility_interrupt_evaluate(&interrupt, 1));
        CHECK(!interrupt.auction_required);
        CHECK(umi_trading_volatility_interrupt_init(&interrupt, 1, 1U) == UMI_STATUS_OK);
        CHECK(umi_trading_volatility_interrupt_evaluate(&interrupt, INT64_MAX));
        return 0;
    }
    return Order(mode);
}
