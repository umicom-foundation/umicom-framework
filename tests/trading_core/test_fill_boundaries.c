/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_fill_boundaries.c
 * PURPOSE: Exercise fill initialization and overflow-free weighted averages.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/execution_aggregation.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                                  \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
/* Compare all public fields so refused operations cannot partly update a fill
 * or aggregate. Each case starts with one valid, independently owned record. */
static int Run(const char *mode)
{
    UmiTradingExecutionFill fill = {0};
    UmiFinancialId execution = {0}, order = {0};
    CHECK(umi_trading_core_id_assign(&execution, "exec") == UMI_STATUS_OK);
    CHECK(umi_trading_core_id_assign(&order, "client") == UMI_STATUS_OK);
    CHECK(umi_trading_execution_fill_init(&fill, &execution, &order, 3, 120, 10) == UMI_STATUS_OK);
    UmiTradingExecutionAggregation aggregate = {5, 100, 1U};
    if (!strcmp(mode, "alias-identities"))
    {
        CHECK(umi_trading_execution_fill_init(&fill, &fill.execution_id, &fill.client_order_id, 7, 130, 20) ==
              UMI_STATUS_OK);
        CHECK(!strcmp(fill.execution_id.value, "exec") && !strcmp(fill.client_order_id.value, "client"));
        CHECK(fill.quantity_lots == 7 && fill.price_ticks == 130 && fill.event_time_ms == 20);
        return 0;
    }
    if (!strcmp(mode, "invalid-init"))
    {
        UmiTradingExecutionFill saved = fill;
        CHECK(umi_trading_execution_fill_init(&fill, &execution, &order, 0, 120, 10) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(&fill, &saved, sizeof fill));
        return 0;
    }
    if (!strcmp(mode, "unterminated-execution") || !strcmp(mode, "unterminated-order") ||
        !strcmp(mode, "last-terminator"))
    {
        memset(execution.value, 'x', sizeof execution.value);
        if (!strcmp(mode, "last-terminator"))
        {
            execution.value[sizeof execution.value - 1U] = '\0';
            CHECK(umi_trading_execution_fill_init(&fill, &execution, &order, 1, 1, 0) == UMI_STATUS_OK);
            CHECK(umi_trading_execution_fill_valid(&fill));
            return 0;
        }
        if (!strcmp(mode, "unterminated-order"))
        {
            order = execution;
            execution = fill.execution_id;
        }
        UmiTradingExecutionFill saved = fill;
        CHECK(umi_trading_execution_fill_init(&fill, &execution, &order, 1, 1, 0) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(&fill, &saved, sizeof fill));
        if (!strcmp(mode, "unterminated-order"))
            fill.client_order_id = order;
        else
            fill.execution_id = execution;
        CHECK(!umi_trading_execution_fill_valid(&fill));
        return 0;
    }
    UmiStatus expected = UMI_STATUS_OK;
    int64_t price = 107, lots = 8;
    if (!strcmp(mode, "weighted-up"))
    {
    }
    else if (!strcmp(mode, "weighted-down"))
    {
        aggregate.average_price_ticks = 120;
        fill.price_ticks = 100;
        price = 113;
    }
    else if (!strcmp(mode, "large-up") || !strcmp(mode, "large-down"))
    {
        aggregate.total_lots = 1;
        fill.quantity_lots = INT64_MAX - 1;
        aggregate.average_price_ticks = !strcmp(mode, "large-up") ? 1 : INT64_MAX;
        fill.price_ticks = !strcmp(mode, "large-up") ? INT64_MAX : 1;
        price = !strcmp(mode, "large-up") ? INT64_MAX - 1 : 2;
        lots = INT64_MAX;
    }
    else if (!strcmp(mode, "same-price"))
    {
        fill.price_ticks = 100;
        price = 100;
    }
    else if (!strcmp(mode, "quantity-overflow"))
    {
        aggregate.total_lots = INT64_MAX;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (!strcmp(mode, "empty-corrupt"))
    {
        aggregate.fill_count = 0U;
        expected = UMI_STATUS_INVALID_STATE;
    }
    else if (!strcmp(mode, "negative-total"))
    {
        aggregate.total_lots = -3;
        expected = UMI_STATUS_INVALID_STATE;
    }
    else if (!strcmp(mode, "negative-average"))
    {
        aggregate.average_price_ticks = INT64_MIN;
        expected = UMI_STATUS_INVALID_STATE;
    }
    else if (!strcmp(mode, "count-inconsistent"))
    {
        aggregate.fill_count = 6U;
        expected = UMI_STATUS_INVALID_STATE;
    }
    else if (!strcmp(mode, "invalid-fill"))
    {
        fill.quantity_lots = 0;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (!strcmp(mode, "first-large"))
    {
        umi_trading_execution_aggregation_init(&aggregate);
        fill.quantity_lots = INT64_MAX;
        fill.price_ticks = INT64_MAX;
        price = INT64_MAX;
        lots = INT64_MAX;
    }
    else if (!strcmp(mode, "rounding"))
    {
        fill.price_ticks = 101;
        price = 100;
    }
    else
        return 2;
    UmiTradingExecutionAggregation saved = aggregate;
    CHECK(umi_trading_execution_aggregation_add(&aggregate, &fill) == expected);
    if (expected != UMI_STATUS_OK)
        CHECK(!memcmp(&aggregate, &saved, sizeof aggregate));
    else
        CHECK(aggregate.average_price_ticks == price && aggregate.total_lots == lots &&
              aggregate.fill_count == saved.fill_count + 1U);
    return 0;
}
int main(int argc, char **argv) { return argc == 2 ? Run(argv[1]) : 2; }
