/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_state_domain.c
 * PURPOSE: Verify only declared canonical and exchange order states can transition, including self-transitions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/trading/order_state.h"
#include "umicom/trading/core/order_state_machine.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                       \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    if (!strcmp(argv[1], "canonical-invalid"))
    {
        const int invalid[] = {-1, 7, INT_MAX};
        for (size_t i = 0U; i < sizeof invalid / sizeof invalid[0]; ++i)
        {
            UmiOrderStatus state = (UmiOrderStatus)invalid[i];
            CHECK(!UmiOrderStatusValid(state));
            CHECK(!umi_order_transition_allowed(state, state));
            CHECK(!umi_order_transition_allowed(UMI_ORDER_NEW, state));
            CHECK(!umi_order_transition_allowed(state, UMI_ORDER_NEW));
        }
    }
    else if (!strcmp(argv[1], "core-invalid"))
    {
        const int invalid[] = {-1, 8, INT_MAX};
        for (size_t i = 0U; i < sizeof invalid / sizeof invalid[0]; ++i)
        {
            UmiTradingCoreOrderState state = (UmiTradingCoreOrderState)invalid[i];
            CHECK(!umi_trading_order_state_machine_allowed(state, state));
            CHECK(umi_trading_order_state_machine_apply(&state, state) == UMI_STATUS_INVALID_STATE);
            CHECK(state == (UmiTradingCoreOrderState)invalid[i]);
        }
    }
    else if (!strcmp(argv[1], "declared"))
    {
        const UmiOrderStatus states[] = {
            UMI_ORDER_NEW,    UMI_ORDER_VALIDATED, UMI_ORDER_ACCEPTED, UMI_ORDER_PARTIALLY_FILLED,
            UMI_ORDER_FILLED, UMI_ORDER_CANCELLED, UMI_ORDER_REJECTED};
        for (size_t i = 0U; i < sizeof states / sizeof states[0]; ++i)
            CHECK(UmiOrderStatusValid(states[i]) && umi_order_transition_allowed(states[i], states[i]));
        CHECK(umi_order_transition_allowed(UMI_ORDER_ACCEPTED, UMI_ORDER_PARTIALLY_FILLED));
        CHECK(!umi_order_transition_allowed(UMI_ORDER_FILLED, UMI_ORDER_ACCEPTED));
        UmiTradingCoreOrderState state = UMI_TRADING_CORE_ORDER_PENDING_CANCEL;
        CHECK(umi_trading_order_state_machine_apply(&state, UMI_TRADING_CORE_ORDER_FILLED) == UMI_STATUS_OK);
        CHECK(!umi_trading_order_state_machine_allowed(state, UMI_TRADING_CORE_ORDER_OPEN));
    }
    else
        return 2;
    return 0;
}
